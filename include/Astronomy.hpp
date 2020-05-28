#ifndef __Astronomy_hpp_
#define __Astronomy_hpp_

//Headers from libphysica library
#include "Linear_Algebra.hpp"

namespace obscura
{

//1. Transform between astronomical coordinate systems.
extern libphysica::Vector Transform_Equat_to_Gal(const libphysica::Vector& v_Equat, double T = 0.0);
extern libphysica::Vector Transform_GeoEcl_to_Gal(const libphysica::Vector& v_GeoEcl, double T = 0.0);
extern libphysica::Vector Transform_HelEcl_to_Gal(const libphysica::Vector& v_HelEcl, double T = 0.0);
extern libphysica::Vector Transform_Gal_to_Equat(const libphysica::Vector& v_Gal, double T = 0.0);

//2. Times
//Fractional days since epoch J2000.0.
extern double Fractional_Days_since_J2000(int day, int month, int year, double hour = 0.0, double minute = 0.0, double second = 0.0);
extern double Local_Apparent_Sidereal_Time(double nJ2000, double longitude);

//3. Sun's and earth's velocity in the galactic frame
extern libphysica::Vector Sun_Velocity();
extern libphysica::Vector Earth_Velocity(double nJ2000);

}	// namespace obscura

#endif