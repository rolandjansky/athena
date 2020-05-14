/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibData/RtResolutionChebyshev.h"
#include <TString.h> // for Form

using namespace MuonCalib;

//*****************************************************************************

//////////////////
// METHOD _init //
//////////////////
void RtResolutionChebyshev::_init(void) {

  // check for consistency //
  if (nPar()<3) {
    throw std::runtime_error(Form("File: %s, Line: %d\nRtResolutionChebyshev::_init() - Not enough parameters!", __FILE__, __LINE__));
  }
  if (parameters()[0]>=parameters()[1]) {
    throw std::runtime_error(Form("File: %s, Line: %d\nRtResolutionChebyshev::_init() - Lower time boundary >= upper time boundary!", __FILE__, __LINE__));
  }

  // pointer to the chebyshev service //
  m_Chebyshev = Tschebyscheff_polynomial::get_Tschebyscheff_polynomial();

  return;
}

//*****************************************************************************

/////////////////
// METHOD name //
/////////////////
std::string RtResolutionChebyshev::name(void) const {
  return std::string("RtResolutionChebyshev");
}

//*****************************************************************************

///////////////////
// METHOD radius //
///////////////////
double RtResolutionChebyshev::resolution(double t, double /*bgRate*/) const {

////////////////////////
// INITIAL TIME CHECK //
////////////////////////

  if(t!=tLower() && t!=tUpper()) {
//get resolution for tmin and tmax to get reasonable boundary conditrions
    double res_min(resolution(tLower())), res_max(resolution(tUpper()));

// if x is out of bounds, return 99999 //
    if (t<parameters()[0])
      return res_min;
		
    if(t>parameters()[1]) 
      return res_max;
  }
///////////////
// VARIABLES //
///////////////
// argument of the Chebyshev polynomials
  double x(2*(t-0.5*(parameters()[1]+parameters()[0]))/
	   (parameters()[1]-parameters()[0])); 
  double resol(0.0); // auxiliary resolution

////////////////////
// CALCULATE r(t) //
////////////////////
  for (unsigned int k=0; k<nPar()-2; k++) {
    resol = resol+parameters()[k+2]*m_Chebyshev->value(k, x);
  }

  return resol;
}

//*****************************************************************************

///////////////////
// METHOD tLower //
///////////////////
double RtResolutionChebyshev::tLower(void) const {
  return parameters()[0];
}

//*****************************************************************************

///////////////////
// METHOD tUpper //
///////////////////
double RtResolutionChebyshev::tUpper(void) const {
  return parameters()[1];
}

//*****************************************************************************

//////////////////////////////////
// METHOD numberOfResParameters //
//////////////////////////////////
unsigned int RtResolutionChebyshev::numberOfResParameters(void) const {
  return nPar()-2;
}

//*****************************************************************************

//////////////////////////
// METHOD resParameters //
//////////////////////////
std::vector<double> RtResolutionChebyshev::resParameters(void) const {
  std::vector<double> alpha(nPar()-2);
  for (unsigned int k=0; k<alpha.size(); k++) {
    alpha[k] = parameters()[k+2];
  }

  return alpha;
}

//*****************************************************************************

/////////////////////////////
// METHOD get_reduced_time //
/////////////////////////////
inline double RtResolutionChebyshev::get_reduced_time(const double & t) const {
  return 2*(t-0.5*(parameters()[1]+parameters()[0]))/
    (parameters()[1]-parameters()[0]);
}
