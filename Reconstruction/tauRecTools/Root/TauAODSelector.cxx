/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// local include(s)
#include "tauRecTools/TauAODSelector.h"


TauAODSelector::TauAODSelector(const std::string& name) 
  : TauRecToolBase(name) {
  declareProperty("MinTauPt", m_minTauPt = 0.);
}


StatusCode TauAODSelector::execute(xAOD::TauJet& tau) const {

  bool passThinning = true;

  // selection criteria that taus must pass in order to be written to AOD
  if(tau.pt() < m_minTauPt) passThinning = false;

  static const SG::AuxElement::Accessor<char> acc_passThinning("passThinning");
  acc_passThinning(tau) = passThinning;
  
  return StatusCode::SUCCESS;
}
