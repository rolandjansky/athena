/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 28.06.2007, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_SimplePolynomialH
#define MuonCalib_SimplePolynomialH

//::::::::::::::::::::::::::::
//:: CLASS SimplePolynomial ::
//::::::::::::::::::::::::::::

/// \class SimplePolynomial
///
/// This class provides the simple polynomials x^k (k=0, 1, ...) as base
/// functions for fits.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 28.06.2007

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

#include "MuonCalibMath/BaseFunction.h"

namespace MuonCalib {

class SimplePolynomial : public BaseFunction {

public:
// Constructor //
	SimplePolynomial(void) {}
	///< Default constructor.

// Method //
	double value(const int & k, const double & x) const;
					///< get x^k

};

}

#endif
