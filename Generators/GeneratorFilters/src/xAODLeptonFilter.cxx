/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/xAODLeptonFilter.h"
#include <cmath>


xAODLeptonFilter::xAODLeptonFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator) 
{
 
}


StatusCode xAODLeptonFilter::filterEvent() {

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

    double leading_lepton_pt_e = 0;
    double leading_lepton_pt_mu = 0;
    double leading_lepton_pt = 0;
    

   // Loop over xTruthParticleContainerElectron 
    unsigned int nParticlesElectrons = xTruthParticleContainerElectron->size();
  for (unsigned int iPart=0; iPart<nParticlesElectrons; ++iPart) {
    const xAOD::TruthParticle* part = (*xTruthParticleContainerElectron)[iPart];




    if (part->status()==1 && part->absPdgId()==11){ //electron 
          const double pT = part->pt();
          const double eta = part->abseta();
          if (pT > leading_lepton_pt_e && std::abs(eta) <= m_EtaRange) {
	  leading_lepton_pt_e = pT;
        }       
     }
  }

  // Loop over xTruthParticleContainerMuon
  unsigned int nParticlesMuons = xTruthParticleContainerMuon->size();
  for (unsigned int iPart=0; iPart<nParticlesMuons; ++iPart) {
    const xAOD::TruthParticle* part = (*xTruthParticleContainerMuon)[iPart];

    if (part->status()==1 && part->absPdgId()==13){ //Muon 
        const double pT = part->pt();
          const double eta = part->abseta();
          if (pT > leading_lepton_pt_mu && std::abs(eta) <= m_EtaRange) {
          leading_lepton_pt_mu = pT;
        }
     }
   }
  
   if (leading_lepton_pt_e > leading_lepton_pt_mu){
     leading_lepton_pt = leading_lepton_pt_e;
   } else  {
     leading_lepton_pt = leading_lepton_pt_mu;
   }

     
  ATH_MSG_DEBUG ( "Leading lepton pt = " << leading_lepton_pt << "within |eta| <= " << m_EtaRange);

  if (leading_lepton_pt < m_Ptmin) {
    setFilterPassed(false);
    ATH_MSG_DEBUG( "Fail: no e or mu found "
		   << " with pT >= " << m_Ptmin);
  } else if (leading_lepton_pt >= m_Ptmax) {
    setFilterPassed(false);
    ATH_MSG_DEBUG ( "Fail: high pt lepton veto "
		    << " pT < " << m_Ptmax );
  } else {
    setFilterPassed(true);
    ATH_MSG_DEBUG ( "Within min and max pt cuts " << m_Ptmin << ", " 
		    << m_Ptmax );
  }
  return StatusCode::SUCCESS;
}
