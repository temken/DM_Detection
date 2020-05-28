#include "Experiments.hpp"

//Headers from libphysica library
#include "Natural_Units.hpp"
#include "Utilities.hpp"

namespace obscura
{
using namespace libphysica::natural_units;

//1. Nuclear recoil experiments
DM_Detector_Nucleus DAMIC_N()
{
	double DAMIC_exposure			   = 0.107 * kg * day;
	std::vector<Element> DAMIC_targets = {Get_Element(14)};
	double DAMIC_threshold			   = 0.55 * keV;
	double DAMIC_Emax				   = 7.0 * keV;
	unsigned int DAMIC_observed_events = 106;

	DM_Detector_Nucleus detector("DAMIC-N", DAMIC_exposure, DAMIC_targets);
	detector.Use_Energy_Threshold(DAMIC_threshold, DAMIC_Emax);
	detector.Set_Observed_Events(DAMIC_observed_events);

	return detector;
}

DM_Detector_Nucleus XENON1T_N()
{
	double XENON1T_exposure				 = 34.2 * day * 1042 * kg;
	std::vector<Element> XENON1T_targets = {Get_Element(54)};
	double XENON1T_threshold			 = 5.0 * keV;
	double XENON1T_Emax					 = 40.0 * keV;
	double XENON1T_efficiency			 = 0.82;
	unsigned int XENON1T_observed_events = 0;

	DM_Detector_Nucleus detector("XENON1T-N", XENON1T_exposure, XENON1T_targets);
	detector.Set_Flat_Efficiency(XENON1T_efficiency);
	detector.Use_Energy_Threshold(XENON1T_threshold, XENON1T_Emax);
	detector.Set_Observed_Events(XENON1T_observed_events);

	return detector;
}

DM_Detector_Nucleus CRESST_II()
{
	double CRESST_II_exposure					= 52.15 * kg * day;
	std::vector<Element> CRESST_II_targets		= {Get_Element(8), Get_Element(20), Get_Element(74)};	//CaOW
	std::vector<double> CRESST_II_target_ratios = {4, 1, 1};
	double CRESST_II_threshold					= 307 * eV;
	double CRESST_II_Emax						= 40.0 * keV;
	double CRESST_II_resolution					= CRESST_II_threshold / 5.0;
	std::vector<std::string> efficiency_files	= {PROJECT_DIR "data/CRESST-II/Lise_eff_AR_O.dat", PROJECT_DIR "data/CRESST-II/Lise_eff_AR_Ca.dat", PROJECT_DIR "data/CRESST-II/Lise_eff_AR_W.dat"};

	DM_Detector_Nucleus detector("CRESST-II", CRESST_II_exposure, CRESST_II_targets, CRESST_II_target_ratios);
	std::vector<double> energy_events = libphysica::Import_List(PROJECT_DIR "data/CRESST-II/Lise_AR.dat", keV);
	energy_events.push_back(CRESST_II_threshold);
	energy_events.push_back(CRESST_II_Emax);
	detector.Use_Maximum_Gap(energy_events);
	detector.Set_Resolution(CRESST_II_resolution);
	detector.Import_Efficiency(efficiency_files, keV);

	return detector;
}

DM_Detector_Nucleus CRESST_III()
{
	double CRESST_III_exposure					 = 5.594 * kg * day;
	std::vector<Element> CRESST_III_targets		 = {Get_Element(8), Get_Element(20), Get_Element(74)};	 //CaOW
	std::vector<double> CRESST_III_target_ratios = {4, 1, 1};
	double CRESST_III_threshold					 = 30.1 * eV;
	double CRESST_III_Emax						 = 16 * keV;
	double CRESST_III_resolution				 = 4.6 * eV;
	double CRESST_III_efficiency				 = 0.5;
	std::vector<std::string> efficiency_files	 = {PROJECT_DIR "data/CRESST-III/C3P1_DetA_eff_AR_O.dat", PROJECT_DIR "data/CRESST-III/C3P1_DetA_eff_AR_Ca.dat", PROJECT_DIR "data/CRESST-III/C3P1_DetA_eff_AR_W.dat"};

	DM_Detector_Nucleus detector("CRESST-III", CRESST_III_exposure, CRESST_III_targets, CRESST_III_target_ratios);
	detector.Set_Flat_Efficiency(CRESST_III_efficiency);
	std::vector<double> energy_events = libphysica::Import_List(PROJECT_DIR "data/CRESST-III/C3P1_DetA_AR.dat", keV);
	energy_events.push_back(CRESST_III_threshold);
	energy_events.push_back(CRESST_III_Emax);
	detector.Use_Maximum_Gap(energy_events);
	detector.Set_Resolution(CRESST_III_resolution);
	detector.Import_Efficiency(efficiency_files, keV);

	return detector;
}

DM_Detector_Nucleus CRESST_surface()
{
	double CRESST_surface_exposure					 = 0.046 * gram * day;
	std::vector<Element> CRESST_surface_targets		 = {Get_Element(8), Get_Element(13)};
	std::vector<double> CRESST_surface_target_ratios = {3, 2};
	double CRESST_surface_threshold					 = 19.7 * eV;
	double CRESST_surface_Emax						 = 600 * eV;
	double CRESST_surface_resolution				 = 3.74 * eV;

	DM_Detector_Nucleus detector("CRESST-surface", CRESST_surface_exposure, CRESST_surface_targets, CRESST_surface_target_ratios);
	std::vector<double> energy_events = libphysica::Import_List(PROJECT_DIR "data/CRESST-surface/data.txt", keV);
	energy_events.push_back(CRESST_surface_threshold);
	energy_events.push_back(CRESST_surface_Emax);
	detector.Use_Maximum_Gap(energy_events);
	detector.Set_Resolution(CRESST_surface_resolution);

	return detector;
}

//2. Electron recoil experiments - Ionization
DM_Detector_Ionization XENON10_e()
{
	std::string target_name							   = "Xenon";
	double exposure									   = 15 * kg * day;
	double flat_efficiency							   = 0.92;
	std::vector<unsigned long int> observed_event_bins = {126, 60, 12, 3, 2, 0, 2};
	double muPE										   = 27.0;
	double sigPE									   = 6.7;
	std::vector<unsigned int> S2_bin_ranges			   = {14, 41, 68, 95, 122, 149, 176, 203};
	std::string trigger_efficiency					   = PROJECT_DIR "data/XENON10e/PE_Trigger_Efficiency.txt";

	DM_Detector_Ionization detector("XENON10-e", exposure, target_name);
	detector.Set_Flat_Efficiency(flat_efficiency);
	detector.Use_PE_Bins(muPE, sigPE, S2_bin_ranges);
	detector.Set_Observed_Events(observed_event_bins);
	detector.Import_Trigger_Efficiency_PE(trigger_efficiency);

	return detector;
}

DM_Detector_Ionization XENON100_e()
{
	std::string target_name							   = "Xenon";
	double exposure									   = 30 * kg * yr;
	std::vector<unsigned long int> observed_event_bins = {794, 1218, 924, 776, 669, 630, 528, 488, 433, 387};
	double muPE										   = 19.7;
	double sigPE									   = 6.2;
	std::vector<unsigned int> S2_bin_ranges			   = {80, 90, 110, 130, 150, 170, 190, 210, 230, 250, 270};
	std::string trigger_efficiency					   = PROJECT_DIR "data/XENON100e/PE_Trigger_Efficiency.txt";
	std::string acceptance_efficiency				   = PROJECT_DIR "data/XENON100e/PE_Acceptance_Efficiency.txt";

	DM_Detector_Ionization detector("XENON100-e", exposure, target_name);
	detector.Use_PE_Bins(muPE, sigPE, S2_bin_ranges);
	detector.Set_Observed_Events(observed_event_bins);
	detector.Import_Trigger_Efficiency_PE(trigger_efficiency);
	detector.Import_Acceptance_Efficiency_PE(acceptance_efficiency);

	return detector;
}

DM_Detector_Ionization XENON1T_e()
{
	std::string target_name							   = "Xenon";
	double exposure									   = 80755.2 * kg * day;
	std::vector<unsigned long int> observed_event_bins = {8, 7, 2, 1};
	double muPE										   = 33.0;
	double sigPE									   = 7.0;
	std::vector<unsigned int> S2_bin_ranges			   = {150, 200, 250, 300, 350};
	std::string trigger_efficiency					   = PROJECT_DIR "data/XENON1Te/XENON1T_TotalEfficiency.txt";

	DM_Detector_Ionization detector("XENON1T-e", exposure, target_name);
	detector.Use_PE_Bins(muPE, sigPE, S2_bin_ranges);
	detector.Set_Observed_Events(observed_event_bins);
	detector.Import_Trigger_Efficiency_PE(trigger_efficiency);

	return detector;
}

DM_Detector_Ionization DarkSide_50_e()
{
	std::string target_name							   = "Argon";
	double exposure									   = 6786.0 * kg * day;
	unsigned int ne_threshold						   = 3;
	std::vector<unsigned long int> observed_event_bins = {6131, 673, 252, 227, 198, 199, 189, 247, 230, 261, 249, 329, 336};

	DM_Detector_Ionization detector("DarkSide-50-e", exposure, target_name);
	detector.Use_Electron_Bins(ne_threshold, 13);
	detector.Set_Observed_Events(observed_event_bins);

	return detector;
}

//3. Electron recoil experiments - Semiconductor
DM_Detector_Semiconductor protoSENSEI_at_Surface()
{
	double SENSEI_surface_exposure								  = 0.07 * gram * 456 * minute;
	unsigned int SENSEI_surface_Q_threshold						  = 1;
	unsigned int SENSEI_surface_N_bins							  = 5;
	std::vector<double> SENSEI_surface_efficiencies				  = {0.668, 0.41, 0.32, 0.27, 0.24};
	std::vector<unsigned long int> SENSEI_surface_observed_events = {140302, 4676, 131, 1, 0};

	DM_Detector_Semiconductor detector("protoSENSEI@surface", SENSEI_surface_exposure, "Si");
	detector.Use_Q_Bins(SENSEI_surface_Q_threshold, SENSEI_surface_N_bins);
	detector.Set_Observed_Events(SENSEI_surface_observed_events);
	detector.Set_Bin_Efficiencies(SENSEI_surface_efficiencies);

	return detector;
}

DM_Detector_Semiconductor protoSENSEI_at_MINOS()
{
	double SENSEI_exposure								  = 0.246 * gram * day;
	unsigned int SENSEI_Q_threshold						  = 1;
	unsigned int SENSEI_N_bins							  = 3;
	std::vector<double> SENSEI_efficiencies				  = {1.0, 0.62, 0.48};
	std::vector<unsigned long int> SENSEI_observed_events = {8516, 87, 0};

	DM_Detector_Semiconductor detector("protoSENSEI@MINOS", SENSEI_exposure, "Si");
	detector.Use_Q_Bins(SENSEI_Q_threshold, SENSEI_N_bins);
	detector.Set_Observed_Events(SENSEI_observed_events);
	detector.Set_Bin_Efficiencies(SENSEI_efficiencies);

	return detector;
}

DM_Detector_Semiconductor SENSEI_at_MINOS()
{
	double SENSEI_exposure								  = 9.1 * gram * day;
	unsigned int SENSEI_Q_threshold						  = 1;
	unsigned int SENSEI_N_bins							  = 4;
	std::vector<double> SENSEI_efficiencies				  = {1.38 / 9.1, 2.09 / 9.1, 9.03 / 9.1, 1.0};
	std::vector<unsigned long int> SENSEI_observed_events = {1312, 5, 0, 0};

	DM_Detector_Semiconductor detector("SENSEI@MINOS", SENSEI_exposure, "Si");
	detector.Use_Q_Bins(SENSEI_Q_threshold, SENSEI_N_bins);
	detector.Set_Observed_Events(SENSEI_observed_events);
	detector.Set_Bin_Efficiencies(SENSEI_efficiencies);

	return detector;
}

DM_Detector_Semiconductor CDMS_HVeV()
{
	double SuperCDMS_exposure								 = 0.487 * gram * day;
	double SuperCDMS_flat_efficiency						 = 0.9545;
	unsigned int SuperCDMS_Q_threshold						 = 1;
	unsigned int SuperCDMS_N_bins							 = 6;
	std::vector<double> SuperCDMS_efficiencies				 = {0.88, 0.91, 0.91, 0.91, 0.91, 0.91};
	std::vector<unsigned long int> SuperCDMS_observed_events = {53000, 400, 74, 18, 7, 14};

	DM_Detector_Semiconductor detector("CMDS-HVeV", SuperCDMS_exposure, "Si");
	detector.Set_Flat_Efficiency(SuperCDMS_flat_efficiency);
	detector.Use_Q_Bins(SuperCDMS_Q_threshold, SuperCDMS_N_bins);
	detector.Set_Observed_Events(SuperCDMS_observed_events);
	detector.Set_Bin_Efficiencies(SuperCDMS_efficiencies);

	return detector;
}

}	// namespace obscura