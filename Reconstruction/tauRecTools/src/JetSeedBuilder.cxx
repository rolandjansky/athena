/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "JetSeedBuilder.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauJet.h"

//______________________________________________________________________________
JetSeedBuilder::JetSeedBuilder(const std::string& name) :
  TauRecToolBase(name) {
}

//______________________________________________________________________________
JetSeedBuilder::~JetSeedBuilder() {
}

//______________________________________________________________________________
StatusCode JetSeedBuilder::execute(xAOD::TauJet& pTau) const {

  if (! pTau.jetLink().isValid()) {
    ATH_MSG_ERROR("Tau jet link is invalid.");
    return StatusCode::FAILURE;
  }
  const xAOD::Jet* jetSeed = pTau.jet();

  ATH_MSG_DEBUG("seed is Jet with"
		<< " pt=" << jetSeed->pt()
		<< " eta=" << jetSeed->eta()
		<< " phi=" << jetSeed->phi());

  // Do not set eta phi for negative energy jet in trigger
  // For other case, pt is reset to be at least 1e-7. 
  if (inTrigger() && jetSeed->e() < 0) {
    ATH_MSG_DEBUG("TauJet eta/phi will be set in Level2 Trigger for negative energy jet");
    pTau.setP4(jetSeed->pt(),pTau.eta(),pTau.phi(),0.0);
  } 
  else {
	if ( jetSeed->pt() > 1e-7) {
	  pTau.setP4(jetSeed->pt(), jetSeed->eta(), jetSeed->phi(), 0.0);
    }
	else {
	  pTau.setP4(static_cast<float>(1e-7), jetSeed->eta(), jetSeed->phi(), 0.0);
    }
  
    pTau.setP4(xAOD::TauJetParameters::JetSeed, jetSeed->pt(), jetSeed->eta(), jetSeed->phi(), jetSeed->m());
  }

  return StatusCode::SUCCESS;
}

#endif
