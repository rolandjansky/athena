/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DITAUREC_DITAUCANDIDATEDATA_H
#define DITAUREC_DITAUCANDIDATEDATA_H

#include "xAODJet/JetContainer.h"
#include "xAODTau/DiTauJetContainer.h"
#include "xAODTau/DiTauJetAuxContainer.h"
#include "fastjet/PseudoJet.hh"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "CaloEvent/CaloCell.h"

class DiTauCandidateData {
 public:
  xAOD::DiTauJet* xAODDiTau;
  xAOD::DiTauJetContainer* xAODDiTauContainer;
  xAOD::DiTauJetAuxContainer* diTauAuxContainer;

  const xAOD::Jet* seed;
  const xAOD::JetContainer* seedContainer;
  std::vector<fastjet::PseudoJet> subjets;
  std::vector<const CaloCell*> subjetCells;

  std::vector<const xAOD::Electron*> electrons;
  std::vector<const xAOD::Muon*> muons;

  float Rjet;
  float Rsubjet;
  float Rcore;

};

#endif // DITAUREC_DITAUCANDIDATEDATA_H
