///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METHelpers.h
// Header file for METHelpers functions
// Author: T.J.Khoo<khoo@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef METUTILITIES_MET_METHELPERS_H
#define METUTILITIES_MET_METHELPERS_H 1

#include "xAODMuon/MuonContainer.h"
#include "xAODJet/JetContainer.h"

namespace met {

  void addGhostMuonsToJets(const xAOD::MuonContainer& muons, xAOD::JetContainer& jets);
  // void addGhostElecsToJets(const xAOD::ElectronContainer& elecs, xAOD::JetContainer& jets);
  
}

#endif
