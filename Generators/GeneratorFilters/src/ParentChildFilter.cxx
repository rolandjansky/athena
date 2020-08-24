/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/ParentChildFilter.h"

ParentChildFilter::ParentChildFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("PDGParent",m_PDGParent);
  declareProperty("PtMinParent",m_PtMinParent = 0.);
  declareProperty("PtMaxParent",m_PtMaxParent = 1e9);
  declareProperty("MassMinParent",m_MassMinParent = -1e9);
  declareProperty("MassMaxParent",m_MassMaxParent = 1e9);
  declareProperty("EtaRangeParent",m_EtaRangeParent = 10.0);
  declareProperty("PDGChild",m_PDGChild);
  declareProperty("PtMinChild",m_PtMinChild = 0.);
  declareProperty("EtaRangeChild",m_EtaRangeChild = 10.0);
}


StatusCode ParentChildFilter::filterInitialize() {
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


StatusCode ParentChildFilter::filterEvent() {
  for (McEventCollection::const_iterator itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (auto  pitr: *genEvt) {
      int okPDGParent = 0;
      for (int i = 0; i < int(m_PDGParent.size()); i++) if (std::abs(pitr->pdg_id()) == m_PDGParent[i]) okPDGParent=1;
      if ( (m_PDGParent[0] == 0) || (okPDGParent
           && pitr->momentum().perp() >= m_PtMinParent
           && (pitr->momentum().perp() < m_PtMaxParent)
           && (pitr->momentum().m() >= m_MassMinParent)
           && (pitr->momentum().m() < m_MassMaxParent)
           && std::abs(pitr->momentum().eta()) < m_EtaRangeParent)) {
        // Check if has end_vertex (skips initial protons)
        if (!pitr->end_vertex()) continue;
        // Child
        for (auto thisChild: *(pitr->end_vertex())) {
          int okPDGChild = 0;
          for (int i=0; i < int(m_PDGChild.size()); i++) if (std::abs(thisChild->pdg_id()) == m_PDGChild[i]) okPDGChild=1;
          if ( thisChild->pdg_id() != pitr->pdg_id()
               && (m_PDGChild[0] == 0 || okPDGChild)
               && thisChild->momentum().perp() > m_PtMinChild
               && std::abs(thisChild->momentum().eta()) < m_EtaRangeChild ) {
            return StatusCode::SUCCESS;
          }
        }
      }
    }
  }
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
