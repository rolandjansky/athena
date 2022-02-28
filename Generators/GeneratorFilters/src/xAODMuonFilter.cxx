/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/xAODMuonFilter.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

xAODMuonFilter::xAODMuonFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("Ptcut",m_Ptmin = 10000.);
  declareProperty("Etacut",m_EtaRange = 10.0);
}


StatusCode xAODMuonFilter::filterEvent() {
  // Retrieve TruthMuons container
  const xAOD::TruthParticleContainer* xTruthParticleContainer;
  if (evtStore()->retrieve(xTruthParticleContainer, "TruthMuons").isFailure()) {
      ATH_MSG_ERROR("No TruthParticle collection with name " << "TruthMuons" << " found in StoreGate!");
      return StatusCode::FAILURE;
  }

  unsigned int nParticles = xTruthParticleContainer->size();
  for (unsigned int iPart=0; iPart<nParticles; ++iPart) {
    const xAOD::TruthParticle* part = (*xTruthParticleContainer)[iPart];

    if (part->status()==1 && part->absPdgId()==13) //muon
        if(  part->pt()>= m_Ptmin && part->abseta() <= m_EtaRange )
            return StatusCode::SUCCESS;
  }
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
