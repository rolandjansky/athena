/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 05.06.2006, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

///////////////////////////////////////////////////////////////
// IMPLEMENTATION OF NON-INLINE METHODS DEFINED IN THE CLASS //
//                    RtResolutionChebyshev                  //
///////////////////////////////////////////////////////////////

#include "MdtCalibData/RtResolutionChebyshev.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

using namespace std;
using namespace MuonCalib;

//*****************************************************************************

//////////////////
// METHOD _init //
//////////////////
void RtResolutionChebyshev::_init(void) {
  MsgStream log(Athena::getMessageSvc(), "RtResolutionChebyshev");

// check for consistency //
  if (nPar()<3) {
    log<<MSG::ERROR << "Not enough parameters!"<<endmsg;
    exit(1);
  }
  if (parameters()[0]>=parameters()[1]) {
    log<<MSG::ERROR << "Lower time boundary >= upper time boundary!"<<endmsg;
    exit(1);
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
  return string("RtResolutionChebyshev");
}

//*****************************************************************************

///////////////////
// METHOD radius //
///////////////////
double RtResolutionChebyshev::resolution(double t, double /*bgRate*/) const {

////////////////////////////
// IGNORE BACKGROUND RATE //
////////////////////////////
/*	if (bgRate>0) {
		cerr << "\n"
			<< "Class RtResolutionChebyshev, method resolution: "
			<< "WARNING!\n"
			<< "Given background rate is ignored.\n";
	}*/

////////////////////////
// INITIAL TIME CHECK //
////////////////////////

  if(t!=tLower() && t!=tUpper()) {
//get resolution for tmin and tmax to get reasonable boundary conditrions
    double res_min(resolution(tLower())), res_max(resolution(tUpper()));
    //		cout<<res_min<<" "<<res_max<<endl;

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
  vector<double> alpha(nPar()-2);
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
