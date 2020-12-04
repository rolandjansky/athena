/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration 
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
  declareProperty("EtaMinParent",m_EtaMinParent = 0.);
  declareProperty("RapidityRangeParent",m_RapidityRangeParent = 10.0);
  declareProperty("RapidityMinParent",m_RapidityMinParent = 0.);
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
  ATH_MSG_INFO("EtaMinParent = " << m_EtaMinParent);
  ATH_MSG_INFO("RapidityRangeParent = " << m_RapidityRangeParent);
  ATH_MSG_INFO("RapidityMinParent = " << m_RapidityMinParent);
  for (int i=0; i < int(m_PDGChild.size()); i++) ATH_MSG_INFO("PDGChild["<<i<<"] = " << m_PDGChild[i]);
  ATH_MSG_INFO("PtMinChild = " << m_PtMinChild);
  ATH_MSG_INFO("EtaRangeChild = " << m_EtaRangeChild);
  return StatusCode::SUCCESS;
}


StatusCode ParentChildFilter::filterEvent() {
  for (McEventCollection::const_iterator itr = events_const()->begin(); itr != events_const()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {
      int okPDGParent = 0;
      for (int i = 0; i < int(m_PDGParent.size()); i++) if (abs((*pitr)->pdg_id()) == m_PDGParent[i]) okPDGParent=1;
      if ( (m_PDGParent[0] == 0) || (okPDGParent
	   && (*pitr)->momentum().perp() >= m_PtMinParent
           && ((*pitr)->momentum().perp() < m_PtMaxParent)
           && ((*pitr)->momentum().m() >= m_MassMinParent)
	   && ((*pitr)->momentum().m() < m_MassMaxParent)
	   && (fabs((*pitr)->momentum().eta()) > m_EtaMinParent)
	   && (fabs((*pitr)->momentum().eta()) < m_EtaRangeParent)
	   && (fabs(getRapidity(pitr)) > m_RapidityMinParent)
	   && (fabs(getRapidity(pitr)) < m_RapidityRangeParent))) {
        // Check if has end_vertex (skips initial protons)
        if (!(*pitr)->end_vertex()) continue;
        // Child
        HepMC::GenVertex::particle_iterator firstChild = (*pitr)->end_vertex()->particles_begin(HepMC::children);
        HepMC::GenVertex::particle_iterator endChild = (*pitr)->end_vertex()->particles_end(HepMC::children);
        HepMC::GenVertex::particle_iterator thisChild = firstChild;
        for (; thisChild != endChild; ++thisChild) {
          int okPDGChild = 0;
          for (int i=0; i < int(m_PDGChild.size()); i++) if (abs((*thisChild)->pdg_id()) == m_PDGChild[i]) okPDGChild=1;
          if ( (*thisChild)->pdg_id() != (*pitr)->pdg_id()
               && (m_PDGChild[0] == 0 || okPDGChild)
               && (*thisChild)->momentum().perp() > m_PtMinChild
               && fabs((*thisChild)->momentum().eta()) < m_EtaRangeChild ) {
            return StatusCode::SUCCESS;
          }
        }
      }
    }
  }
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
