/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 22.05.2004, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

///////////////////////////////////////////////////////////////
// IMPLEMENTATION OF NON-INLINE METHODS DEFINED IN THE CLASS //
//                   Legendre_polynomial                     //
///////////////////////////////////////////////////////////////

/////////////
// HEADERS //
/////////////

#include "MuonCalibMath/Legendre_polynomial.h"

namespace MuonCalib {

//*****************************************************************************

/////////////////////////////////////
// SET LEGENDRE-POLYNOMIAL POINTER //
/////////////////////////////////////

 Legendre_polynomial *Legendre_polynomial::s_polynomial_pointer=0;

//*****************************************************************************

////////////////////////////////////
// METHOD get_Legendre_polynomial //
////////////////////////////////////

 Legendre_polynomial * Legendre_polynomial::get_Legendre_polynomial(void) {

    if (s_polynomial_pointer == 0) {
      s_polynomial_pointer = new Legendre_polynomial();
    }
    return s_polynomial_pointer;

  }

//*****************************************************************************

/////////////////////////
// PRIVATE CONSTRUCTOR //
/////////////////////////

    Legendre_polynomial::Legendre_polynomial(void) {
    }

}
