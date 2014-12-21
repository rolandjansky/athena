/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 07.04.2005, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/////////////////////////////////////////////////////
// IMPLEMENTATION OF THE CLASS ChebyshevPolynomial //
/////////////////////////////////////////////////////

#include "MuonCalibMath/ChebyshevPolynomial.h"

namespace MuonCalib {

//*****************************************************************************

//////////////////
// METHOD value //
//////////////////

  double ChebyshevPolynomial::value(const int & order, const double & x) const {

    switch(order) {
    case 0:
      return 1;
      break;
    case 1:
      return x;
      break;
    case 2:
    default:
      return 2.0*x*value(order-1, x) - value(order-2, x);
      break;
    }

  }

}
