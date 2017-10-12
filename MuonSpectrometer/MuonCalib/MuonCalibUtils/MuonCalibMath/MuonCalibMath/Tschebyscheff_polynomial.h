/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Tschebyscheff_polynomialHXX
#define Tschebyscheff_polynomialHXX

/////////////
// HEADERS //
/////////////

#include "math.h"

namespace MuonCalib {

/**
@class  Tschebyscheff_polynomial
This class provides a tschebyscheff polynomial of order k. 
This class is a monoton/singleton object.
@author Oliver.Kortner@cern.ch
@date 27.12.2004
*/

  class Tschebyscheff_polynomial {

  private:
    static Tschebyscheff_polynomial *s_polynomial_pointer; //!< pointer to the monoton
    Tschebyscheff_polynomial(void); //!< constructor

  public:
    // Methods
    static Tschebyscheff_polynomial * get_Tschebyscheff_polynomial(void); //!< get a pointer to the Tschebyscheff polynomial
    /** get the value of the Tschebyscheff polynomial of order order at 
	x (-1 <= x <= 1) */
    inline double value(const unsigned int & order,
			const double & x) const;
  };

}

//////////////////////////////////////////////////
// INCLUDE THE IMPLEMENTATION OF INLINE METHODS //
//////////////////////////////////////////////////

#include "Tschebyscheff_polynomial.ixx"

#endif
