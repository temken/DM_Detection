#include "obscura/Direct_Detection_ER.hpp"

#include "libphysica/Integration.hpp"
#include "libphysica/Natural_Units.hpp"
#include "libphysica/Statistics.hpp"
#include "libphysica/Utilities.hpp"

namespace obscura
{
using namespace libphysica::natural_units;

//1. Event spectra and rates
double dRdEe_Ionization_ER(double Ee, const DM_Particle& DM, DM_Distribution& DM_distr, double m_nucleus, Atomic_Electron& shell)
{
	double N_T		= 1.0 / m_nucleus;
	double vMax		= DM_distr.Maximum_DM_Speed();
	double E_DM_max = DM.mass / 2.0 * vMax * vMax;
	if(E_DM_max < shell.binding_energy)
		return 0.0;

	double qMin = DM.mass * vMax - sqrt(DM.mass * DM.mass * vMax * vMax - 2.0 * DM.mass * shell.binding_energy);
	double qMax = DM.mass * vMax + sqrt(DM.mass * DM.mass * vMax * vMax - 2.0 * DM.mass * shell.binding_energy);
	if(qMin > shell.q_max)
		return 0.0;
	else if(qMax > shell.q_max)
		qMax = shell.q_max;

	std::vector<double> q_grid = libphysica::Log_Space(qMin, qMax, 100);
	double d_lnq			   = log(q_grid[1] / q_grid[0]);
	double integral			   = 0.0;
	for(auto& q : q_grid)
	{
		double vMin = vMinimal_Electrons(q, shell.binding_energy + Ee, DM.mass);
		if(vMin < vMax)
		{
			if(DM.DD_use_eta_function && DM_distr.DD_use_eta_function)
			{
				double vDM = 1.0e-3;   // cancels
				integral += 2.0 * d_lnq * q * q * DM.d2Sigma_dq2_dEe_Ionization(q, Ee, vDM, shell) * vDM * vDM * DM_distr.DM_density / DM.mass * DM_distr.Eta_Function(vMin);
			}
			else
			{
				auto integrand = [&DM_distr, &DM, q, Ee, &shell](double v) {
					return DM_distr.Differential_DM_Flux(v, DM.mass) * DM.d2Sigma_dq2_dEe_Ionization(q, Ee, v, shell);
				};
				integral += 2.0 * d_lnq * q * q * libphysica::Integrate(integrand, vMin, vMax);
			}
		}
	}
	return N_T * integral;
}

double dRdEe_Ionization_ER(double Ee, const DM_Particle& DM, DM_Distribution& DM_distr, Atom& atom)
{
	double result	 = 0.0;
	double m_nucleus = atom.nucleus.Average_Nuclear_Mass();
	for(auto& electron : atom.electrons)
		result += dRdEe_Ionization_ER(Ee, DM, DM_distr, m_nucleus, electron);
	return result;
}

DM_Detector_Ionization_ER::DM_Detector_Ionization_ER()
: DM_Detector_Ionization("Electron recoil experiment", kg * day, "Electrons", "Xe") {}
DM_Detector_Ionization_ER::DM_Detector_Ionization_ER(std::string label, double expo, std::string atom)
: DM_Detector_Ionization(label, expo, "Electrons", atom) {}
DM_Detector_Ionization_ER::DM_Detector_Ionization_ER(std::string label, double expo, std::vector<std::string> atoms, std::vector<double> mass_fractions)
: DM_Detector_Ionization(label, expo, "Electrons", atoms, mass_fractions)
{
}

double DM_Detector_Ionization_ER::dRdE_Ionization(double E, const DM_Particle& DM, DM_Distribution& DM_distr, const Nucleus& nucleus, Atomic_Electron& shell)
{
	return flat_efficiency * dRdEe_Ionization_ER(E, DM, DM_distr, nucleus.Average_Nuclear_Mass(), shell);
}

}	// namespace obscura
