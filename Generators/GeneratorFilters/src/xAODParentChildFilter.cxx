/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/xAODParentChildFilter.h"

xAODParentChildFilter::xAODParentChildFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
 
}


StatusCode xAODParentChildFilter::filterInitialize() {
  if (m_PDGParent.size() == 0) ATH_MSG_ERROR("PDGParent[] not set ");
  if (m_PDGChild.size() == 0) ATH_MSG_ERROR("PDGChild[] not set ");
  for (int i=0; i < int(m_PDGParent.size()); i++) ATH_MSG_INFO("PDGParent["<<i<<"] = " << m_PDGParent[i]);
  ATH_MSG_INFO("PtMinParent = " << m_PtMinParent);
  ATH_MSG_INFO("PtMaxParent = " << m_PtMaxParent);
  ATH_MSG_INFO("MassMinParent = " << m_MassMinParent);
  ATH_MSG_INFO("MassMaxParent = " << m_MassMaxParent);
  ATH_MSG_INFO("EtaRangeParent = " << m_EtaRangeParent);
  for (int i=0; i < int(m_PDGChild.size()); i++) ATH_MSG_INFO("PDGChild["<<i<<"] = " << m_PDGChild[i]);
  ATH_MSG_INFO("PtMinChild = " << m_PtMinChild);
  ATH_MSG_INFO("EtaRangeChild = " << m_EtaRangeChild);
  return StatusCode::SUCCESS;
}


StatusCode xAODParentChildFilter::filterEvent() {
    
   // Retrieve full TruthEventContainer container
   const xAOD::TruthEventContainer *xTruthEventContainer=NULL;
    if (evtStore()->retrieve(xTruthEventContainer, "TruthEvents").isFailure())
    {
        ATH_MSG_ERROR("No TruthEvent collection with name " << "TruthEvents" << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
     
    for ( xAOD::TruthEventContainer::const_iterator itr = xTruthEventContainer->begin(); itr != xTruthEventContainer->end(); ++itr) {
       unsigned int nPart = (*itr)->nTruthParticles();
       for (unsigned int iPart = 0; iPart < nPart; ++iPart) {
            const xAOD::TruthParticle* pitr =  (*itr)->truthParticle(iPart);
  
    int okPDGParent = 0;
      for (int i = 0; i < int(m_PDGParent.size()); i++) if (std::abs(pitr->pdgId()) == m_PDGParent[i]) okPDGParent=1;
      if ( (m_PDGParent[0] == 0) || (okPDGParent
           && pitr->pt() >= m_PtMinParent
           && (pitr->pt() < m_PtMaxParent)
           && (pitr->m() >= m_MassMinParent)
           && (pitr->m() < m_MassMaxParent)
           && std::abs(pitr->eta()) < m_EtaRangeParent)) {
       
       // Check if has end_vertex (skips initial protons)
        if (!pitr->decayVtx()) continue;
    
        const xAOD::TruthVertex* decayVertex = pitr->decayVtx();
        int num_outgoing_particles = decayVertex->nOutgoingParticles();

        for (int iOutPart = 0; iOutPart< num_outgoing_particles; iOutPart++) {
    		const xAOD::TruthParticle* thisChild =  decayVertex->outgoingParticle(iOutPart);
        	int okPDGChild = 0;
          for (int i=0; i < int(m_PDGChild.size()); i++) if (std::abs(thisChild->pdgId()) == m_PDGChild[i]) okPDGChild=1;
          if ( thisChild->pdgId() != pitr->pdgId()
               && (m_PDGChild[0] == 0 || okPDGChild)
               && thisChild->pt() > m_PtMinChild
               && std::abs(thisChild->eta()) < m_EtaRangeChild ) {
            return StatusCode::SUCCESS;
          }
        }
      }
    }
  }
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
