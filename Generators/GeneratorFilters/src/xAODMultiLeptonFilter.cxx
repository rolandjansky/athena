/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/xAODMultiLeptonFilter.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"


xAODMultiLeptonFilter::xAODMultiLeptonFilter(const std::string& name, ISvcLocator* pSvcLocator)
: GenFilter(name, pSvcLocator)
{
  declareProperty("Ptcut", m_Ptmin = 10000.);
  declareProperty("Etacut", m_EtaRange = 10.0);
  declareProperty("NLeptons", m_NLeptons = 4);
}


StatusCode xAODMultiLeptonFilter::filterEvent() {
  
  // Retrieve TruthElectrons  container
  const xAOD::TruthParticleContainer* xTruthParticleContainerElectron;
  if (evtStore()->retrieve(xTruthParticleContainerElectron, "TruthElectrons").isFailure()) {
      ATH_MSG_ERROR("No TruthParticle collection with name " << "TruthElectrons" << " found in StoreGate!");
      return StatusCode::FAILURE;
  }
  // Retrieve TruthMuons container
  const xAOD::TruthParticleContainer* xTruthParticleContainerMuon;
  if (evtStore()->retrieve(xTruthParticleContainerMuon, "TruthMuons").isFailure()) {
      ATH_MSG_ERROR("No TruthParticle collection with name " << "TruthMuons" << " found in StoreGate!");
      return StatusCode::FAILURE;
  }
  int numLeptons = 0;

  unsigned int nParticlesElectrons = xTruthParticleContainerElectron->size();
  for (unsigned int iPart=0; iPart<nParticlesElectrons; ++iPart) {
    const xAOD::TruthParticle* part = (*xTruthParticleContainerElectron)[iPart];

    if (part->status()==1 && part->absPdgId()==11) //electron 
        if(  part->pt()>= m_Ptmin && part->abseta() <= m_EtaRange )
        {
          numLeptons += 1;
          if (numLeptons >= m_NLeptons)
          {
            setFilterPassed(true);
            return StatusCode::SUCCESS;
          }
        }       
  }

  unsigned int nParticlesMuons = xTruthParticleContainerMuon->size();
  for (unsigned int iPart=0; iPart<nParticlesMuons; ++iPart) {
    const xAOD::TruthParticle* part = (*xTruthParticleContainerMuon)[iPart];

    if (part->status()==1 && part->absPdgId()==13) //Muon 
        if(  part->pt()>= m_Ptmin && part->abseta() <= m_EtaRange )
        {
          numLeptons += 1;
          if (numLeptons >= m_NLeptons)
          {
            setFilterPassed(true);
            return StatusCode::SUCCESS;
          }
        }      
  }
  
  setFilterPassed(false);
  return StatusCode::SUCCESS;

}
