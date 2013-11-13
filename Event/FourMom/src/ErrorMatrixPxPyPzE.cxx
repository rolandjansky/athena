/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/ErrorMatrixPxPyPzE.h"

  /// Default constructor creates zeroed 4x4 matrix 
ErrorMatrixPxPyPzE::ErrorMatrixPxPyPzE() : Base(4)
{}  

  /// Constructor from HepSymMatrix, checks dimension
ErrorMatrixPxPyPzE::ErrorMatrixPxPyPzE( const CLHEP::HepSymMatrix& mat) :
  Base(mat) 
{}
