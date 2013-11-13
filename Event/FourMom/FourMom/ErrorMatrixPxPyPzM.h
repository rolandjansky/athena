/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ErrorMatrixPxPyPzM_H
#define ErrorMatrixPxPyPzM_H

#include "FourMom/ErrorMatrixBase.h"

#include <cmath>

class ErrorMatrixPxPyPzM : private ErrorMatrixBase< ErrorMatrixPxPyPzM> {
public:
  
  typedef ErrorMatrixBase< ErrorMatrixPxPyPzM> Base;

  /// Enumerator for error parameterization 
  enum Index {ipx=0,ipy=1,ipz=2,ie=3};

  /// Default constructor creates zeroed 4x4 matrix 
  ErrorMatrixPxPyPzM();  

  /// Constructor from HepSymMatrix, checks dimension
  ErrorMatrixPxPyPzM( const CLHEP::HepSymMatrix& mat);

  using Base::operator();
  using Base::hsm;
  using Base::num_row;
  using Base::num_col;

  double pxError() const {
    return std::sqrt( impl()(ipx,ipx));
  }


};

#endif
