/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 29.06.2006, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

#include "MuonCalibMath/LegendrePolynomial.h"
#include "cmath"

//::::::::::::::::::::::::
//:: NAMESPACE SETTINGS ::
//::::::::::::::::::::::::

using namespace MuonCalib;

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF METHODS DEFINED IN THE CLASS LegendrePolynomial ::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//*****************************************************************************

//::::::::::::::::::
//:: METHOD value ::
//::::::::::::::::::

double LegendrePolynomial::value(const int & k, const double & x) const {


//////////////////////////////////////////////////////////////////////////////
// LOWER ORDER LEGENDRE POLYNOMIALS ARE GIVEN EXPLICITLY, HIGHER ORDERS ARE //
// CALCULATE RECURSIVELY.                                                   //
//////////////////////////////////////////////////////////////////////////////

	switch(k) {
		case 0:
			return 1;
			break;
		case 1:
			return x;
			break;
		case 2:
			return 0.5*(3.0*x*x-1.0);
			break;
		case 3:
			return 0.5*(5.0*x*x*x-3.0*x);
			break;
		case 4:
			return (35.0*std::pow(x, 4)-30.0*x*x+3.0)/8.0;
			break;
		case 5:
			return (63.0*std::pow(x, 5)-70.0*x*x*x+15.0*x)/8.0;
			break;
		case 6:
			return (231.0*std::pow(x, 6)-315.0*std::pow(x, 4)+105.0*x*x-5.0)
									/16.0;
			break;
		case 7:
			return (429.0*std::pow(x, 7)-693*std::pow(x, 5)+315.0*x*x*x
				-35.0*x)/16.0;
			break;
		default:
			return (2.0*k-1)*x*value(k-1, x)/
					static_cast<double>(k)
				- (k-1)*value(k-2, x)/
					static_cast<double>(k);
			break;
	}

}
