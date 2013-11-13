/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/ErrorMatrixPtCotThPhiM.h"

  /// Default constructor creates zeroed 4x4 matrix 
ErrorMatrixPtCotThPhiM::ErrorMatrixPtCotThPhiM() : Base(4)
{}  

  /// Constructor from HepSymMatrix, checks dimension
ErrorMatrixPtCotThPhiM::ErrorMatrixPtCotThPhiM( const CLHEP::HepSymMatrix& mat) :
  Base(mat) 
{}
