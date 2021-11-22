/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/xAODMETFilter.h"
#include "TruthUtils/PIDHelpers.h"


xAODMETFilter::xAODMETFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("METCut",m_METmin = 10000.);
  // Normally we'd include them, but this is unstable if using EvtGen
  declareProperty("UseNeutrinosFromHadrons",m_useHadronicNu = false);
}


StatusCode xAODMETFilter::filterEvent() {
    
  // Retrieve TruthMET container from xAOD MET slimmer, contains (MC::isGenStable() && MC::isNonInteracting()) particles
  const xAOD::TruthParticleContainer* xTruthParticleContainer;
  if (evtStore()->retrieve(xTruthParticleContainer, "TruthMET").isFailure()) {
      ATH_MSG_ERROR("No TruthParticle collection with name " << "TruthMET" << " found in StoreGate!");
      return StatusCode::FAILURE;
  }

  double sumx(0), sumy(0);
  unsigned int nParticles = xTruthParticleContainer->size();
  for (unsigned int iPart=0; iPart<nParticles; ++iPart) {
    const xAOD::TruthParticle* missingETparticle = (*xTruthParticleContainer)[iPart];
    if (!m_useHadronicNu && MC::PID::isNeutrino(missingETparticle->pdgId()) &&
    !(missingETparticle->auxdata<bool>("isFromWZ") || missingETparticle->auxdata<bool>("isFromTau")) ) continue;
    sumx += missingETparticle->px();
    sumy += missingETparticle->py();
  }

  double met = std::sqrt(sumx*sumx + sumy*sumy);
  setFilterPassed(met >= m_METmin);
  return StatusCode::SUCCESS;
}

