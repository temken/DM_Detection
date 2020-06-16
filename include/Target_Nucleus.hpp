#ifndef __Target_Nucleus_hpp_
#define __Target_Nucleus_hpp_

#include <string>
#include <vector>

#include "version.hpp"

namespace obscura
{

//1. Kinematic functions
extern double vMinimal_Nucleus(double ER, double mDM, double mNucleus);
extern double Maximum_Nuclear_Recoil_Energy(double vDM, double mDM, double mNucleus);

//2. Class for nuclear isotopes.
struct Isotope
{
	unsigned int Z, A;
	double abundance;
	double spin;
	double sp, sn;

	std::string name;
	double mass;

	Isotope();
	Isotope(unsigned int z, unsigned int a, double abund = 1.0, double Spin = 0.0, double Sp = 0.0, double Sn = 0.0);

	double Thomas_Fermi_Radius() const;

	//Nuclear form factor for SI interactions
	double Helm_Form_Factor(double q) const;

	//Nuclear form factor for SD interactions
	// to do

	void Print_Summary(unsigned int MPI_rank) const;
};

//3. Class for elements containing all isotopes occuring in nature
struct Element
{
	std::vector<Isotope> isotopes;
	std::string name;

	Element();
	Element(const std::vector<Isotope>& iso);
	Element(const Isotope& iso);

	unsigned int Number_of_Isotopes() const;

	// void Add_Isotope(Isotope& isotope);

	Isotope Get_Isotope(unsigned int A) const;

	Isotope& operator[](int i)
	{
		return isotopes[i];
	}
	const Isotope& operator[](int i) const
	{
		return isotopes[i];
	}

	double Average_Nuclear_Mass() const;

	void Print_Summary(unsigned int MPI_rank = 0) const;
};

//4. Nuclear data
extern void Import_Nuclear_Data();
extern std::vector<Element> Elements;
extern Isotope Get_Isotope(unsigned int Z, unsigned int A);
extern Element Get_Element(unsigned int Z);
extern Element Get_Element(std::string name);

}	// namespace obscura

#endif
