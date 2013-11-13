/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ErrorMatrixPtEtaPhiM_H
#define ErrorMatrixPtEtaPhiM_H

#include "ErrorMatrixBase.h"

#include <cmath>

class ErrorMatrixPtEtaPhiM : private ErrorMatrixBase< ErrorMatrixPtEtaPhiM> {
public:
  
  typedef ErrorMatrixBase< ErrorMatrixPtEtaPhiM> Base;

  /// Enumerator for error parameterization 
  enum Index {ipt=0,ieta=1,iphi=2,im=3};

  /// Default constructor creates zeroed 4x4 matrix 
  ErrorMatrixPtEtaPhiM();  

  /// Constructor from HepSymMatrix, checks dimension
  ErrorMatrixPtEtaPhiM( const CLHEP::HepSymMatrix& mat);

  using Base::operator();


};

#endif
