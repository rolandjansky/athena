/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ChebyshevPolynomialHXX
#define ChebyshevPolynomialHXX

/////////////
// HEADERS //
/////////////

#include "math.h"
#include "MuonCalibMath/BaseFunction.h"

namespace MuonCalib {

/**

@class ChebyshevPolynomial 

This class class provides a Chebyshev polynomial of order k. 

@author Oliver.Kortner@cern.ch

@date 07.04.2005

*/

  class ChebyshevPolynomial : public BaseFunction {

  public:
    // Constructor
    ChebyshevPolynomial(void) {}		//!< default constructor

    // Methods
    /** get the value of the Chebyshev polynomial of k-th order at x, 
	(-1 <= x <= 1) */
    double value(const int & k, const double & x) const;

  };

}

#endif
