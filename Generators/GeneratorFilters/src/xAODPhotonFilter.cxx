/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Will pass if there are the specified number of photons with pT and eta in the specified range
#include "GeneratorFilters/xAODPhotonFilter.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

xAODPhotonFilter::xAODPhotonFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("PtMin", m_Ptmin = 10000.);
  declareProperty("PtMax", m_Ptmax = 100000000.);
  declareProperty("EtaCut", m_EtaRange = 2.50);
  declareProperty("NPhotons", m_NPhotons = 2);

  // Backward compatibility aliases
  declareProperty("Ptcut", m_Ptmin = 10000.);
}


StatusCode xAODPhotonFilter::filterEvent() {
  
  // Retrieve Photon container
  const xAOD::TruthParticleContainer* xTruthParticleContainer;
  if (evtStore()->retrieve(xTruthParticleContainer, "TruthPhotons").isFailure()) {
      ATH_MSG_ERROR("No TruthParticle collection with name " << "TruthPhotons" << " found in StoreGate!");
      return StatusCode::FAILURE;
  }

  int NPhotons = 0;
  unsigned int nParticles = xTruthParticleContainer->size();
  for (unsigned int iPart=0; iPart<nParticles; ++iPart) {
    const xAOD::TruthParticle* part = (*xTruthParticleContainer)[iPart];

    if (part->status()==1 && part->absPdgId()==22) //photon
        if(  part->pt()>= m_Ptmin && part->pt()< m_Ptmax && part->abseta() <= m_EtaRange )
        {
          NPhotons++;
          if (NPhotons >= m_NPhotons)
          {
            setFilterPassed(true);
            return StatusCode::SUCCESS;
          }
        }
            
  }
  setFilterPassed(false);
  return StatusCode::SUCCESS;

}
