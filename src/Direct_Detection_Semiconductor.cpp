#include "Direct_Detection_Semiconductor.hpp"

#include <cmath>
#include <numeric>

#include "libphysica/Natural_Units.hpp"
#include "libphysica/Numerics.hpp"

namespace obscura
{
using namespace libphysica::natural_units;

//1. Event spectra and rates

double Minimum_Electron_Energy(int Q, const Semiconductor& target)
{
	return target.epsilon * (Q - 1) + target.energy_gap;
}

unsigned int Electron_Hole_Pairs(double Ee, const Semiconductor& target)
{
	return std::floor((Ee - target.energy_gap) / target.epsilon + 1);
}

double dRdEe_Semiconductor(double Ee, const DM_Particle& DM, DM_Distribution& DM_distr, Semiconductor& target_crystal)
{
	double prefactor = 4.0 / target_crystal.M_cell * aEM * mElectron * mElectron;
	double sum		 = 0.0;
	for(int qi = 0; qi < 900; qi++)
	{
		double q	= (qi + 1) * target_crystal.dq;
		double vMin = vMinimal_Electrons(q, Ee, DM.mass);
		double vMax = DM_distr.Maximum_DM_Speed();
		if(vMin > vMax)
			continue;
		else if(DM.DD_use_eta_function && DM_distr.DD_use_eta_function)
		{
			double vDM = 1e-3;	 //cancels in v^2 * dSigma/dq^2
			sum += target_crystal.dq / q / q * DM_distr.DM_density / DM.mass * DM_distr.Eta_Function(vMin) * target_crystal.Crystal_Form_Factor(q, Ee) * vDM * vDM * DM.dSigma_dq2_Electron(q, vDM);
		}
		else
		{
			auto integrand = [&DM_distr, &DM, q](double v) {
				return DM_distr.Differential_DM_Flux(v, DM.mass) * DM.dSigma_dq2_Electron(q, v);
			};
			double eps = libphysica::Find_Epsilon(integrand, vMin, vMax, 1.0e-4);
			sum += target_crystal.dq / q / q * target_crystal.Crystal_Form_Factor(q, Ee) * libphysica::Integrate(integrand, vMin, vMax, eps);
		}
	}
	return prefactor * sum;
}

double R_Q_Semiconductor(int Q, const DM_Particle& DM, DM_Distribution& DM_distr, Semiconductor& target_crystal)
{
	//Energy threshold
	double Emin = Minimum_Electron_Energy(Q, target_crystal);
	double Emax = Minimum_Electron_Energy(Q + 1, target_crystal);
	//Integrate over energies
	double sum = 0.0;
	for(int Ei = (Emin / target_crystal.dE); Ei < 500; Ei++)
	{
		double E = (Ei + 1) * target_crystal.dE;
		if(E > Emax)
			break;
		sum += target_crystal.dE * dRdEe_Semiconductor(E, DM, DM_distr, target_crystal);
	}
	return sum;
}

double R_total_Semiconductor(int Qthreshold, const DM_Particle& DM, DM_Distribution& DM_distr, Semiconductor& target_crystal)
{
	//Energy threshold
	double E_min = Minimum_Electron_Energy(Qthreshold, target_crystal);
	//Integrate over energies
	double sum = 0.0;
	for(int Ei = (E_min / target_crystal.dE); Ei < 500; Ei++)
	{
		double E = (Ei + 1) * target_crystal.dE;
		sum += target_crystal.dE * dRdEe_Semiconductor(E, DM, DM_distr, target_crystal);
	}
	return sum;
}

//2. Electron recoil direct detection experiment with semiconductor target
DM_Detector_Semiconductor::DM_Detector_Semiconductor()
: DM_Detector("Semiconductor experiment", gram * year, "Electrons"), semiconductor_target(Semiconductor("Si"))
{
}

DM_Detector_Semiconductor::DM_Detector_Semiconductor(std::string label, double expo, std::string crys)
: DM_Detector(label, expo, "Electrons"), semiconductor_target(Semiconductor(crys))
{
}

//DM functions
double DM_Detector_Semiconductor::Maximum_Energy_Deposit(const DM_Particle& DM, const DM_Distribution& DM_distr) const
{
	return DM.mass / 2.0 * pow(DM_distr.Maximum_DM_Speed(), 2.0);
}

double DM_Detector_Semiconductor::Minimum_DM_Mass(DM_Particle& DM, const DM_Distribution& DM_distr) const
{
	return 2.0 * energy_threshold * pow(DM_distr.Maximum_DM_Speed(), -2.0);
}

double DM_Detector_Semiconductor::Minimum_DM_Speed(const DM_Particle& DM) const
{
	return sqrt(2.0 * energy_threshold / DM.mass);
}

double DM_Detector_Semiconductor::dRdE(double E, const DM_Particle& DM, DM_Distribution& DM_distr)
{
	return flat_efficiency * dRdEe_Semiconductor(E, DM, DM_distr, semiconductor_target);
}

double DM_Detector_Semiconductor::DM_Signals_Total(const DM_Particle& DM, DM_Distribution& DM_distr)
{
	double N = 0;
	if(statistical_analysis == "Binned Poisson")
	{
		std::vector<double> binned_events = DM_Signals_Binned(DM, DM_distr);
		N								  = std::accumulate(binned_events.begin(), binned_events.end(), 0.0);
	}
	else if(using_energy_threshold || statistical_analysis == "Maximum Gap")
	{
		std::function<double(double)> spectrum = [this, &DM, &DM_distr](double E) {
			return dRdE(E, DM, DM_distr);
		};
		double epsilon = libphysica::Find_Epsilon(spectrum, energy_threshold, energy_max, 1e-6);
		N			   = exposure * libphysica::Integrate(spectrum, energy_threshold, energy_max, epsilon);
	}
	else if(using_Q_threshold)
	{
		N = exposure * flat_efficiency * R_total_Semiconductor(Q_threshold, DM, DM_distr, semiconductor_target);
	}
	return N;
}

std::vector<double> DM_Detector_Semiconductor::DM_Signals_Binned(const DM_Particle& DM, DM_Distribution& DM_distr)
{
	if(statistical_analysis != "Binned Poisson")
	{
		std::cerr << "Error in obscura::DM_Detector_Semiconductor::DM_Signals_Binned(const DM_Particle&, DM_Distribution&): The statistical analysis is " << statistical_analysis << ", not 'Binned Poisson'." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	else if(using_energy_bins)
	{
		return DM_Signals_Energy_Bins(DM, DM_distr);
	}
	else if(using_Q_bins)
	{
		return DM_Signals_Q_Bins(DM, DM_distr);
	}
	else
	{
		std::cerr << "Error in obscura::DM_Detector_Semiconductor::DM_Signals_Binned(): Statistical analysis is 'Binned Poisson' but no bins have been defined. This should not happen ever." << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

//Q spectrum
void DM_Detector_Semiconductor::Use_Q_Threshold(unsigned int Q_thr)
{
	Initialize_Poisson();
	using_Q_threshold = true;
	Q_threshold		  = Q_thr;
	energy_threshold  = Minimum_Electron_Energy(Q_threshold, semiconductor_target);
	energy_max		  = Minimum_Electron_Energy(semiconductor_target.Q_max, semiconductor_target);
}

void DM_Detector_Semiconductor::Use_Q_Bins(unsigned int Q_thr, unsigned int N_bins)
{
	using_Q_bins	   = true;
	Q_threshold		   = Q_thr;
	unsigned int Q_max = (N_bins == 0) ? semiconductor_target.Q_max : N_bins + Q_threshold - 1;
	N_bins			   = Q_max - Q_threshold + 1;
	Initialize_Binned_Poisson(N_bins);

	energy_threshold = Minimum_Electron_Energy(Q_threshold, semiconductor_target);
	energy_max		 = Minimum_Electron_Energy(Q_max, semiconductor_target);
}

std::vector<double> DM_Detector_Semiconductor::DM_Signals_Q_Bins(const DM_Particle& DM, DM_Distribution& DM_distr)
{
	if(!using_Q_bins)
	{
		std::cerr << "Error in obscura::DM_Detector_Semiconductor::DM_Signals_Q_Bins(const DM_Particle&,DM_Distribution&): Not using Q bins." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	else
	{
		std::vector<double> signals;
		for(unsigned int Q = Q_threshold; Q < Q_threshold + number_of_bins; Q++)
		{
			signals.push_back(exposure * flat_efficiency * bin_efficiencies[Q - 1] * R_Q_Semiconductor(Q, DM, DM_distr, semiconductor_target));
		}
		return signals;
	}
}

void DM_Detector_Semiconductor::Print_Summary(int MPI_rank) const
{
	Print_Summary_Base();
	std::cout << std::endl
			  << "\tElectron recoil experiment (semiconductor)." << std::endl
			  << "\tTarget:\t\t\t" << semiconductor_target.name << " semiconductor" << std::endl;
	if(using_Q_threshold || using_Q_bins)
		std::cout << "\teh pair threshold:\t" << Q_threshold << std::endl
				  << "----------------------------------------" << std::endl
				  << std::endl;
}

}	// namespace obscura
