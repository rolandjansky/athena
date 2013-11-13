/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ErrorMatrixPtCotThPhiM_H
#define ErrorMatrixPtCotThPhiM_H

#include "ErrorMatrixBase.h"

#include <cmath>

class ErrorMatrixPtCotThPhiM : private ErrorMatrixBase< ErrorMatrixPtCotThPhiM> {
public:
  
  typedef ErrorMatrixBase< ErrorMatrixPtCotThPhiM> Base;

  /// Enumerator for error parameterization 
  enum Index {ipt=0,icotth=1,iphi=2,im=3};

  /// Default constructor creates zeroed 4x4 matrix 
  ErrorMatrixPtCotThPhiM();  

  /// Constructor from HepSymMatrix, checks dimension
  ErrorMatrixPtCotThPhiM( const CLHEP::HepSymMatrix& mat);

  using Base::operator();


};

#endif
