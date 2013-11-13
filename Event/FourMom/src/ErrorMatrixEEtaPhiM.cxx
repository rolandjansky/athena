/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/ErrorMatrixEEtaPhiM.h"

  /// Default constructor creates zeroed 4x4 matrix 
ErrorMatrixEEtaPhiM::ErrorMatrixEEtaPhiM() : Base(4)
{}  

  /// Constructor from HepSymMatrix, checks dimension
ErrorMatrixEEtaPhiM::ErrorMatrixEEtaPhiM( const CLHEP::HepSymMatrix& mat) :
  Base(mat) 
{}
