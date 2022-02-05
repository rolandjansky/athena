/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 23.03.2005, AUTHOR: OLIVER KORTNER
// Modified: 31.05.2006 by O. Kortner: major redesign:
//                                     driftvelocity implemented,
//                                     resolution has been removed,
//                                     set-method have been removed.
//           04.06.2006 by O. Kortner: bug in constructor fixed.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "MdtCalibData/RtChebyshev.h"

#include <TString.h>  // for Form

using namespace MuonCalib;

//*****************************************************************************

//////////////////
// METHOD _init //
//////////////////

void RtChebyshev::_init(void) {
    // check for consistency //
    if (nPar() < 3) {
        throw std::runtime_error(Form("File: %s, Line: %d\nRtChebyshev::_init() - Not enough parameters!", __FILE__, __LINE__));
    }
    if (parameters()[0] >= parameters()[1]) {
        throw std::runtime_error(
            Form("File: %s, Line: %d\nRtChebyshev::_init() - Lower time boundary >= upper time boundary!", __FILE__, __LINE__));
    }

    // pointer to the chebyshev service //
    m_Chebyshev = Tschebyscheff_polynomial::get_Tschebyscheff_polynomial();

    return;
}  // end RtChebyshev::_init

//*****************************************************************************

/////////////////
// METHOD name //
/////////////////
std::string RtChebyshev::name(void) const { return std::string("RtChebyshev"); }

//*****************************************************************************

///////////////////
// METHOD radius //
///////////////////
double RtChebyshev::radius(double t) const {
    ////////////////////////
    // INITIAL TIME CHECK //
    ////////////////////////
    if (t < parameters()[0]) return 0.0;
    if (t > parameters()[1]) return 14.6;
    // if x is out of bounds, return 99999 //
    //	if (t<parameters()[0] || t>parameters()[1]) {
    //		return 99999.0;
    //	}

    ///////////////
    // VARIABLES //
    ///////////////
    // argument of the Chebyshev polynomials
    double x(2 * (t - 0.5 * (parameters()[1] + parameters()[0])) / (parameters()[1] - parameters()[0]));
    double rad(0.0);  // auxiliary radius

    ////////////////////
    // CALCULATE r(t) //
    ////////////////////
    for (unsigned int k = 0; k < nPar() - 2; k++) { rad = rad + parameters()[k + 2] * m_Chebyshev->value(k, x); }

    return rad >= 0 ? rad : 0;
}

//*****************************************************************************

//////////////////////////
// METHOD driftvelocity //
//////////////////////////
double RtChebyshev::driftvelocity(double t) const { return (radius(t + 1.0) - radius(t)); }

//*****************************************************************************

///////////////////
// METHOD tLower //
///////////////////
double RtChebyshev::tLower(void) const { return parameters()[0]; }

//*****************************************************************************

///////////////////
// METHOD tUpper //
///////////////////
double RtChebyshev::tUpper(void) const { return parameters()[1]; }

//*****************************************************************************

/////////////////////////////////
// METHOD numberOfRtParameters //
/////////////////////////////////
unsigned int RtChebyshev::numberOfRtParameters(void) const { return nPar() - 2; }

//*****************************************************************************

/////////////////////////
// METHOD rtParameters //
/////////////////////////
std::vector<double> RtChebyshev::rtParameters(void) const {
    std::vector<double> alpha(nPar() - 2);
    for (unsigned int k = 0; k < alpha.size(); k++) { alpha[k] = parameters()[k + 2]; }

    return alpha;
}

//*****************************************************************************

/////////////////////////////
// METHOD get_reduced_time //
/////////////////////////////
double RtChebyshev::get_reduced_time(const double& t) const {
    return 2 * (t - 0.5 * (parameters()[1] + parameters()[0])) / (parameters()[1] - parameters()[0]);
}
