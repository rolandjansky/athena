/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ErrorMatrixPxPyPzE_H
#define ErrorMatrixPxPyPzE_H

#include "FourMom/ErrorMatrixBase.h"

#include <cmath>

class ErrorMatrixPxPyPzE : private ErrorMatrixBase< ErrorMatrixPxPyPzE> {
public:
  
  typedef ErrorMatrixBase< ErrorMatrixPxPyPzE> Base;

  /// Enumerator for error parameterization 
  enum Index {ipx=0,ipy=1,ipz=2,ie=3};

  /// Default constructor creates zeroed 4x4 matrix 
  ErrorMatrixPxPyPzE();  

  /// Constructor from HepSymMatrix, checks dimension
  ErrorMatrixPxPyPzE( const CLHEP::HepSymMatrix& mat);

  using Base::operator();
  using Base::hsm;
  using Base::num_row;
  using Base::num_col;

  double pxError() const {
    return std::sqrt( impl()(ipx,ipx));
  }


};

#endif
