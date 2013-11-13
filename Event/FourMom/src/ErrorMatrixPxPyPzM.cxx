/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/ErrorMatrixPxPyPzM.h"

  /// Default constructor creates zeroed 4x4 matrix 
ErrorMatrixPxPyPzM::ErrorMatrixPxPyPzM() : Base(4)
{}  

  /// Constructor from HepSymMatrix, checks dimension
ErrorMatrixPxPyPzM::ErrorMatrixPxPyPzM( const CLHEP::HepSymMatrix& mat) :
  Base(mat) 
{}
