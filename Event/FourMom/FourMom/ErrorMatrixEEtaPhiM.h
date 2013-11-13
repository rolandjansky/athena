/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ErrorMatrixEEtaPhiM_H
#define ErrorMatrixEEtaPhiM_H

#include "ErrorMatrixBase.h"

#include <cmath>

class ErrorMatrixEEtaPhiM : private ErrorMatrixBase< ErrorMatrixEEtaPhiM> {
public:
  
  typedef ErrorMatrixBase< ErrorMatrixEEtaPhiM> Base;

  /// Enumerator for error parameterization 
  enum Index {ie=0,ieta=1,iphi=2,im=3};

  /// Default constructor creates zeroed 4x4 matrix 
  ErrorMatrixEEtaPhiM();  

  /// Constructor from HepSymMatrix, checks dimension
  ErrorMatrixEEtaPhiM( const CLHEP::HepSymMatrix& mat);

  using Base::operator();
  using Base::hsm;
  using Base::num_row;
  using Base::num_col;
};

#endif
