/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 27.12.2004, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

///////////////////////////////////////////////////////////////
// IMPLEMENTATION OF NON-INLINE METHODS DEFINED IN THE CLASS //
//               Tschebyscheff_polynomial                    //
///////////////////////////////////////////////////////////////

/////////////
// HEADERS //
/////////////

#include "MuonCalibMath/Tschebyscheff_polynomial.h"

namespace MuonCalib {
  //*****************************************************************************

    /////////////////////////////////////
// SET LEGENDRE-POLYNOMIAL POINTER //
/////////////////////////////////////

 Tschebyscheff_polynomial *Tschebyscheff_polynomial::polynomial_pointer=0;

  //*****************************************************************************

    ////////////////////////////////////
    // METHOD get_Legendre_polynomial //
    ////////////////////////////////////

  Tschebyscheff_polynomial * 
  Tschebyscheff_polynomial::get_Tschebyscheff_polynomial(void) {

    if (polynomial_pointer == 0) {
      polynomial_pointer = new Tschebyscheff_polynomial();
    }
    return polynomial_pointer;

  }

  //*****************************************************************************

    /////////////////////////
    // PRIVATE CONSTRUCTOR //
    /////////////////////////

    Tschebyscheff_polynomial::Tschebyscheff_polynomial(void) {
    }

}
