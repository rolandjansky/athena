/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 02.07.2006, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef LegendrePolynomialH
#define LegendrePolynomialH

//::::::::::::::::::::::::::::::
//:: CLASS LegendrePolynomial ::
//::::::::::::::::::::::::::::::

namespace MuonCalib {
/// \class LegendrePolynomial
/// This class provides a legendre polynomial of order k. It is derived from
/// BaseFunction.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 02.07.2006
}

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// standard C++ //
#include "math.h"

// MuonCalib //
#include "MuonCalibMath/BaseFunction.h"


namespace MuonCalib {

class LegendrePolynomial : public BaseFunction {

public:
// Constructor //
	LegendrePolynomial(void) {}
	///< Default constructor

// Methods //
	double value(const int & k, const double & x) const;

};

}

#endif
