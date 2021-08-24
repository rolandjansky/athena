/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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


//*****************************************************************************

////////////////////////////////////
// METHOD get_Legendre_polynomial //
////////////////////////////////////

 const Legendre_polynomial * Legendre_polynomial::get_Legendre_polynomial(void) {
    static const Legendre_polynomial s_polynomial; 
    return &s_polynomial;

  }

//*****************************************************************************

/////////////////////////
// PRIVATE CONSTRUCTOR //
/////////////////////////

    Legendre_polynomial::Legendre_polynomial(void) {
    }

}
