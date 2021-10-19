/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 22.05.2004, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

///////////////////////////////
// CLASS Legendre_polynomial //
///////////////////////////////

///////////////////////////////////////////////////////////
// THIS CLASS PROVIDES A LEGENDRE POLYNOMIAL OF ORDER k. //
// THIS CLASS IS A MONOTON/SINGLETON OBJECT.             //
///////////////////////////////////////////////////////////


#ifndef Legendre_polynomialHXX
#define Legendre_polynomialHXX

//////////////////
// HEADER FILES //
//////////////////

// CLHEP //
//#include "CLHEP/config/CLHEP.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

// standard C++ libraries //
#include <iostream>
#include <iomanip>
#include <fstream>

namespace MuonCalib {

/**
@class  Legendre_polynomial
This class provides a legendre polynomial of order k. 
This class is a monoton/singleton object.
@author Oliver.Kortner@cern.ch
@date 22.05.2004
*/
  class Legendre_polynomial {

  private:
    Legendre_polynomial(void); //!< constructor

  public:
    // Methods
    /** get a pointer to the Legendre polynomial */
    static const Legendre_polynomial * get_Legendre_polynomial(void);

    /** get the value of the Legendre polynomial of order m_order at x */
    inline double value(const int & order,
			   const double & x) const;

  };

}

//////////////////////////////////////////////////
// INCLUDE THE IMPLEMENTATION OF INLINE METHODS //
//////////////////////////////////////////////////

#include "Legendre_polynomial.ixx"

#endif
