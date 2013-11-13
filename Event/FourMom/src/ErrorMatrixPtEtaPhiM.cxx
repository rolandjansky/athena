/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/ErrorMatrixPtEtaPhiM.h"

  /// Default constructor creates zeroed 4x4 matrix 
ErrorMatrixPtEtaPhiM::ErrorMatrixPtEtaPhiM() : Base(4)
{}  

  /// Constructor from HepSymMatrix, checks dimension
ErrorMatrixPtEtaPhiM::ErrorMatrixPtEtaPhiM( const CLHEP::HepSymMatrix& mat) :
  Base(mat) 
{}
