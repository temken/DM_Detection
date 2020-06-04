#include "Configuration.hpp"

#include <fstream>
#include <sys/stat.h>	 //required to create a folder
#include <sys/types.h>	 // required for stat.h

//Headers from libphysica library
#include "Utilities.hpp"

#include "DM_Particle_Standard.hpp"
#include "Direct_Detection_Ionization.hpp"
#include "Direct_Detection_Nucleus.hpp"
#include "Direct_Detection_Semiconductor.hpp"
#include "Experiments.hpp"
#include "version.hpp"

namespace obscura
{
using namespace libconfig;
using namespace libphysica::natural_units;

// Read in the configuration file
Configuration::Configuration()
: ID("default")
{
}

Configuration::Configuration(std::string cfg_filename, int MPI_rank)
: cfg_file(cfg_filename), results_path("./")
{
	// 1. Read the cfg file.
	Read_Config_File();

	// 2. Find the run ID, create a folder and copy the cfg file.
	Initialize_Result_Folder(MPI_rank);

	// 3. DM particle
	Construct_DM_Particle();

	// 4. DM Distribution
	Construct_DM_Distribution();

	// 5. DM-detection experiment
	Construct_DM_Detector();

	// 6. Computation of exclusion limits
	Initialize_Parameters();
}

void Configuration::Initialize_Result_Folder(int MPI_rank)
{
	try
	{
		ID = config.lookup("ID").c_str();
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "Error in obscura::Configuration::Initialize_Result_Folder(): No 'ID' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}

	Create_Result_Folder(MPI_rank);
	Copy_Config_File(MPI_rank);
}

void Configuration::Create_Result_Folder(int MPI_rank)
{
	if(MPI_rank == 0)
	{
		//1. Create the /results/ folder if necessary
		std::string results_folder = TOP_LEVEL_DIR "results";
		mode_t nMode			   = 0733;	 // UNIX style permissions
		int nError_1			   = 0;
#if defined(_WIN32)
		nError_1 = _mkdir(results_folder.c_str());	 // can be used on Windows
#else
		nError_1 = mkdir(results_folder.c_str(), nMode);   // can be used on non-Windows
#endif

		//2. Create a /result/<ID>/ folder for result files.
		results_path = results_folder + "/" + ID + "/";
		int nError	 = 0;
#if defined(_WIN32)
		nError = _mkdir(results_path.c_str());	 // can be used on Windows
#else
		nError	 = mkdir(results_path.c_str(), nMode);	   // can be used on non-Windows
#endif
		if(nError != 0)
		{
			std::cerr << "\nWarning in Configuration::Create_Result_Folder(int): The folder exists already, data will be overwritten." << std::endl
					  << std::endl;
		}
	}
}
void Configuration::Copy_Config_File(int MPI_rank)
{
	if(MPI_rank == 0)
	{
		std::ifstream inFile;
		std::ofstream outFile;
		inFile.open(cfg_file);
		outFile.open(TOP_LEVEL_DIR "results/" + ID + "/" + ID + ".cfg");
		outFile << "// " << PROJECT_NAME << "-v" << PROJECT_VERSION << "\tgit:" << GIT_BRANCH << "/" << GIT_COMMIT_HASH << std::endl;
		outFile << inFile.rdbuf();
		inFile.close();
		outFile.close();
	}
}

void Configuration::Print_Summary(int MPI_rank)
{
	if(MPI_rank == 0)
	{
		std::cout << SEPARATOR
				  << "Summary of obscura configuration" << std::endl
				  << std::endl
				  << "Config file:\t" << cfg_file << std::endl
				  << "ID:\t\t" << ID << std::endl;
		DM->Print_Summary(MPI_rank);
		DM_distr->Print_Summary(MPI_rank);
		DM_detector->Print_Summary(MPI_rank);
		std::cout << "Direct detection constraints" << std::endl
				  << "\tCertainty level [%]:\t" << 100.0 * constraints_certainty << std::endl
				  << "\tMass range [GeV]:\t[" << constraints_mass_min << "," << constraints_mass_max << "]" << std::endl
				  << "\tMass steps:\t\t" << constraints_masses
				  << SEPARATOR
				  << std::endl;
	}
}

void Configuration::Read_Config_File()
{
	try
	{
		config.readFile(cfg_file.c_str());
	}
	catch(const FileIOException& fioex)
	{
		std::cerr << "Error in obscura::Configuration::Read_Config_File(): I/O error while reading configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	catch(const ParseException& pex)
	{
		std::cerr << "Error in obscura::Configuration::Read_Config_File(): Configurate file parse error at " << pex.getFile() << ":" << pex.getLine() << " - " << pex.getError() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

void Configuration::Construct_DM_Particle()
{
	double DM_mass, DM_spin, DM_fraction;
	bool DM_light;
	//3.1 General properties
	try
	{
		DM_mass = config.lookup("DM_mass");
		DM_mass *= GeV;
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DM_mass' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	try
	{
		DM_spin = config.lookup("DM_spin");
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DM_spin' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	try
	{
		DM_fraction = config.lookup("DM_fraction");
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DM_fraction' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	try
	{
		DM_light = config.lookup("DM_light");
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DM_light' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}

	//3.2 DM interactions
	std::string DM_interaction;
	try
	{
		DM_interaction = config.lookup("DM_interaction").c_str();
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DM_interaction' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}

	//3.2.1 SI and SD
	if(DM_interaction == "SI" || DM_interaction == "SD")
		Configuration::Construct_DM_Particle_Standard(DM_interaction);
	else
	{
		std::cerr << "Error in obscura::Configuration::Construct_DM_Particle(): 'DM_interaction' setting " << DM_interaction << " in configuration file not recognized." << std::endl;
		std::exit(EXIT_FAILURE);
	}

	DM->Set_Mass(DM_mass);
	DM->Set_Spin(DM_spin);
	DM->Set_Fractional_Density(DM_fraction);
	DM->Set_Low_Mass_Mode(DM_light);
}

void Configuration::Construct_DM_Particle_Standard(std::string DM_interaction)
{
	//SI
	if(DM_interaction == "SI")
	{
		DM = new DM_Particle_SI();

		//DM form factor
		std::string DM_form_factor;
		double DM_mediator_mass = -1.0;
		try
		{
			DM_form_factor = config.lookup("DM_form_factor").c_str();
		}
		catch(const SettingNotFoundException& nfex)
		{
			std::cerr << "Error in Configuration::Construct_DM_Particle_Standard(): No 'DM_form_factor' setting in configuration file." << std::endl;
			std::exit(EXIT_FAILURE);
		}
		if(DM_form_factor == "General")
		{
			try
			{
				DM_mediator_mass = config.lookup("DM_mediator_mass");
				DM_mediator_mass *= MeV;
			}
			catch(const SettingNotFoundException& nfex)
			{
				std::cerr << "Error in Configuration::Construct_DM_Particle_Standard(): No 'DM_mediator_mass' setting in configuration file." << std::endl;
				std::exit(EXIT_FAILURE);
			}
		}
		dynamic_cast<DM_Particle_SI*>(DM)->Set_FormFactor_DM(DM_form_factor, DM_mediator_mass);
	}

	//SD
	else if(DM_interaction == "SD")
	{
		DM = new DM_Particle_SD();
	}

	//SI and SD
	bool DM_isospin_conserved;
	try
	{
		DM_isospin_conserved = config.lookup("DM_isospin_conserved");
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "Error in Configuration::Construct_DM_Particle_Standard(): No 'DM_isospin_conserved' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	double fp_rel, fn_rel;
	if(DM_isospin_conserved)
	{
		fp_rel = 1.0;
		fn_rel = 1.0;
	}
	else
	{
		try
		{
			fp_rel = config.lookup("DM_relative_couplings")[0];
			fn_rel = config.lookup("DM_relative_couplings")[1];
		}
		catch(const SettingNotFoundException& nfex)
		{
			std::cerr << "Error in Configuration::Construct_DM_Particle_Standard(): No 'DM_relative_couplings' setting in configuration file." << std::endl;
			std::exit(EXIT_FAILURE);
		}
	}
	dynamic_cast<DM_Particle_Standard*>(DM)->Fix_Coupling_Ratio(fp_rel, fn_rel);

	double DM_cross_section_nucleon;
	try
	{
		DM_cross_section_nucleon = config.lookup("DM_cross_section_nucleon");
		DM_cross_section_nucleon *= cm * cm;
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "Error in Configuration::Construct_DM_Particle_Standard(): No 'DM_cross_section_nucleon' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	DM->Set_Interaction_Parameter(DM_cross_section_nucleon, "Nuclei");

	double DM_cross_section_electron;
	try
	{
		DM_cross_section_electron = config.lookup("DM_cross_section_electron");
		DM_cross_section_electron *= cm * cm;
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "Error in Configuration::Construct_DM_Particle_Standard(): No 'DM_cross_section_electron' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	DM->Set_Sigma_Electron(DM_cross_section_electron);
}

void Configuration::Construct_DM_Distribution()
{
	std::string DM_distribution;
	try
	{
		DM_distribution = config.lookup("DM_distribution").c_str();
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DM_distribution' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}

	if(DM_distribution == "SHM")
		Construct_DM_Distribution_SHM();
	else
	{
		std::cerr << "Error in obscura::Configuration::Construct_DM_Distribution(): 'DM_distribution' setting " << DM_distribution << " in configuration file not recognized." << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

void Configuration::Construct_DM_Distribution_SHM()
{
	double DM_local_density, SHM_v0, SHM_vEarth, SHM_vEscape;
	try
	{
		DM_local_density = config.lookup("DM_local_density");
		DM_local_density *= GeV / cm / cm / cm;
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "Error in Construct_DM_Distribution_SHM(): No 'DM_local_density' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	try
	{
		SHM_v0 = config.lookup("SHM_v0");
		SHM_v0 *= km / sec;
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "Error in Construct_DM_Distribution_SHM(): No 'SHM_v0' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	try
	{
		SHM_vEarth = config.lookup("SHM_vEarth");
		SHM_vEarth *= km / sec;
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "Error in Construct_DM_Distribution_SHM(): No 'SHM_vEarth' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	try
	{
		SHM_vEscape = config.lookup("SHM_vEscape");
		SHM_vEscape *= km / sec;
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "Error in Construct_DM_Distribution_SHM(): No 'SHM_vEscape' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	DM_distr = new Standard_Halo_Model(DM_local_density, SHM_v0, SHM_vEarth, SHM_vEscape);
}

void Configuration::Construct_DM_Detector()
{
	std::string DD_experiment;
	try
	{
		DD_experiment = config.lookup("DD_experiment").c_str();
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DD_experiment' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}

	// User-defined experiments:
	if(DD_experiment == "Nuclear recoil")
		Construct_DM_Detector_Nuclear();
	else if(DD_experiment == "Ionization")
		Construct_DM_Detector_Ionization();
	else if(DD_experiment == "Semiconductor")
		Construct_DM_Detector_Semiconductor();

	// Supported experiments:
	else if(DD_experiment == "DAMIC-N")
		DM_detector = new DM_Detector_Nucleus(DAMIC_N());
	else if(DD_experiment == "XENON1T-N")
		DM_detector = new DM_Detector_Nucleus(XENON1T_N());
	else if(DD_experiment == "CRESST-II")
		DM_detector = new DM_Detector_Nucleus(CRESST_II());
	else if(DD_experiment == "CRESST-surface")
		DM_detector = new DM_Detector_Nucleus(CRESST_surface());
	else if(DD_experiment == "CRESST-III")
		DM_detector = new DM_Detector_Nucleus(CRESST_III());

	else if(DD_experiment == "XENON10-e")
		DM_detector = new DM_Detector_Ionization(XENON10_e());
	else if(DD_experiment == "XENON100-e")
		DM_detector = new DM_Detector_Ionization(XENON100_e());
	else if(DD_experiment == "XENON1T-e")
		DM_detector = new DM_Detector_Ionization(XENON1T_e());
	else if(DD_experiment == "DarkSide-50-e")
		DM_detector = new DM_Detector_Ionization(DarkSide_50_e());

	else if(DD_experiment == "protoSENSEI@surface")
		DM_detector = new DM_Detector_Semiconductor(protoSENSEI_at_Surface());
	else if(DD_experiment == "protoSENSEI@MINOS")
		DM_detector = new DM_Detector_Semiconductor(protoSENSEI_at_MINOS());
	else if(DD_experiment == "SENSEI@MINOS")
		DM_detector = new DM_Detector_Semiconductor(SENSEI_at_MINOS());
	else if(DD_experiment == "CDMS-HVeV")
		DM_detector = new DM_Detector_Semiconductor(CDMS_HVeV());
	else
	{
		std::cerr << "Error in obscura::Configuration::Construct_DM_Detector(): Experiment " << DD_experiment << " not recognized." << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

void Configuration::Construct_DM_Detector_Nuclear()
{
	std::vector<Element> DD_targets_nuclear;
	std::vector<double> DD_targets_nuclear_abundances;
	try
	{
		int element_count = config.lookup("DD_targets_nuclear").getLength();
		for(int j = 0; j < element_count; j++)
		{
			double abund = config.lookup("DD_targets_nuclear")[j][0];
			int Z		 = config.lookup("DD_targets_nuclear")[j][1];
			DD_targets_nuclear_abundances.push_back(abund);
			DD_targets_nuclear.push_back(Get_Element(Z));
		}
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DD_targets_nuclear' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}

	double DD_threshold_nuclear, DD_Emax_nuclear, DD_exposure_nuclear, DD_efficiency_nuclear, DD_expected_background_nuclear;
	unsigned int DD_observed_events_nuclear;
	try
	{
		DD_threshold_nuclear = config.lookup("DD_threshold_nuclear");
		DD_threshold_nuclear *= keV;
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DD_threshold_nuclear' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	try
	{
		DD_Emax_nuclear = config.lookup("DD_Emax_nuclear");
		DD_Emax_nuclear *= keV;
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DD_Emax_nuclear' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	try
	{
		DD_exposure_nuclear = config.lookup("DD_exposure");
		DD_exposure_nuclear *= kg * yr;
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DD_exposure' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	try
	{
		DD_efficiency_nuclear = config.lookup("DD_efficiency");
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DD_efficiency' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	try
	{
		DD_observed_events_nuclear = config.lookup("DD_observed_events");
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DD_observed_events' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	try
	{
		DD_expected_background_nuclear = config.lookup("DD_expected_background");
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DD_expected_background' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	DM_detector = new DM_Detector_Nucleus("Nuclear recoil", DD_exposure_nuclear, DD_targets_nuclear, DD_targets_nuclear_abundances);
	DM_detector->Set_Flat_Efficiency(DD_efficiency_nuclear);
	DM_detector->Use_Energy_Threshold(DD_threshold_nuclear, DD_Emax_nuclear);
	DM_detector->Set_Observed_Events(DD_observed_events_nuclear);
	DM_detector->Set_Expected_Background(DD_expected_background_nuclear);
}

void Configuration::Construct_DM_Detector_Ionization()
{
	std::string DD_target_ionization;
	unsigned int DD_threshold_ionization, DD_observed_events_ionization;
	double DD_exposure_ionization, DD_efficiency_ionization, DD_expected_background_ionization;
	try
	{
		DD_target_ionization = config.lookup("DD_target_electron").c_str();
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DD_target_electron' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	try
	{
		DD_threshold_ionization = config.lookup("DD_threshold_electron");
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DD_threshold_electron' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	try
	{
		DD_exposure_ionization = config.lookup("DD_exposure");
		DD_exposure_ionization *= kg * yr;
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DD_exposure' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	try
	{
		DD_efficiency_ionization = config.lookup("DD_efficiency");
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DD_efficiency' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	try
	{
		DD_observed_events_ionization = config.lookup("DD_observed_events");
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DD_observed_events' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	try
	{
		DD_expected_background_ionization = config.lookup("DD_expected_background");
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DD_expected_background' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}

	DM_detector = new DM_Detector_Ionization("Ionization", DD_exposure_ionization, DD_target_ionization);
	DM_detector->Set_Flat_Efficiency(DD_efficiency_ionization);
	dynamic_cast<DM_Detector_Ionization*>(DM_detector)->Use_Electron_Threshold(DD_threshold_ionization);
	DM_detector->Set_Observed_Events(DD_observed_events_ionization);
	DM_detector->Set_Expected_Background(DD_expected_background_ionization);
}

void Configuration::Construct_DM_Detector_Semiconductor()
{
	std::string DD_target_semiconductor;
	unsigned int DD_threshold_semiconductor, DD_observed_events_semiconductor;
	double DD_exposure_semiconductor, DD_efficiency_semiconductor, DD_expected_background_semiconductor;
	try
	{
		DD_target_semiconductor = config.lookup("DD_target_electron").c_str();
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DD_target_electron' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	try
	{
		DD_threshold_semiconductor = config.lookup("DD_threshold_electron");
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DD_threshold_electron' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	try
	{
		DD_exposure_semiconductor = config.lookup("DD_exposure");
		DD_exposure_semiconductor *= kg * yr;
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DD_exposure' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	try
	{
		DD_efficiency_semiconductor = config.lookup("DD_efficiency");
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DD_efficiency' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	try
	{
		DD_observed_events_semiconductor = config.lookup("DD_observed_events");
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DD_observed_events' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	try
	{
		DD_expected_background_semiconductor = config.lookup("DD_expected_background");
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "No 'DD_expected_background' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}

	DM_detector = new DM_Detector_Semiconductor("Semiconductor", DD_exposure_semiconductor, DD_target_semiconductor);
	DM_detector->Set_Flat_Efficiency(DD_efficiency_semiconductor);
	dynamic_cast<DM_Detector_Semiconductor*>(DM_detector)->Use_Q_Threshold(DD_threshold_semiconductor);
	DM_detector->Set_Observed_Events(DD_observed_events_semiconductor);
	DM_detector->Set_Expected_Background(DD_expected_background_semiconductor);
}

void Configuration::Initialize_Parameters()
{
	try
	{
		constraints_certainty = config.lookup("constraints_certainty");
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "Error in Configuration::Initialize_Parameters(): No 'constraints_certainty' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}

	try
	{
		constraints_mass_min = config.lookup("constraints_mass_min");
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "Error in Configuration::Initialize_Parameters(): No 'constraints_mass_min' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}

	try
	{
		constraints_mass_max = config.lookup("constraints_mass_max");
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "Error in Configuration::Initialize_Parameters(): No 'constraints_mass_max' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}

	try
	{
		constraints_masses = config.lookup("constraints_masses");
	}
	catch(const SettingNotFoundException& nfex)
	{
		std::cerr << "Error in Configuration::Initialize_Parameters(): No 'constraints_masses' setting in configuration file." << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

}	// namespace obscura
