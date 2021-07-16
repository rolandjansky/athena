/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

    ////////////////////////////////////
    // METHOD get_Legendre_polynomial //
    ////////////////////////////////////

  const Tschebyscheff_polynomial * 
  Tschebyscheff_polynomial::get_Tschebyscheff_polynomial(void) {
    static const Tschebyscheff_polynomial s_polynomial;
    return &s_polynomial;
  }

  //*****************************************************************************

    /////////////////////////
    // PRIVATE CONSTRUCTOR //
    /////////////////////////

    Tschebyscheff_polynomial::Tschebyscheff_polynomial(void) {
    }

}
