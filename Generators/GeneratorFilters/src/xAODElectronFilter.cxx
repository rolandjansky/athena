/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/xAODElectronFilter.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

#include "GaudiKernel/ServiceHandle.h"

xAODElectronFilter::xAODElectronFilter(const std::string &name, ISvcLocator *pSvcLocator)
    : GenFilter(name, pSvcLocator)
{
  declareProperty("Ptcut", m_Ptmin = 10000.);
  declareProperty("Etacut", m_EtaRange = 10.0);
}

StatusCode xAODElectronFilter::filterEvent() {
  // Retrieve full TruthParticle container
  const xAOD::TruthParticleContainer *xTruthParticleContainer;
  if (evtStore()->retrieve(xTruthParticleContainer, "TruthElectrons").isFailure())
  {
    ATH_MSG_ERROR("No TruthParticle collection with name "
                  << "TruthElectrons"
                  << " found in StoreGate!");
    return StatusCode::FAILURE;
  }

  unsigned int nParticles = xTruthParticleContainer->size();
  for (unsigned int iPart = 0; iPart < nParticles; ++iPart)
  {
    const xAOD::TruthParticle *part = (*xTruthParticleContainer)[iPart];

     //electron
    if (part->pt() >= m_Ptmin && part->abseta() <= m_EtaRange)
      return StatusCode::SUCCESS;
  }
  
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}

