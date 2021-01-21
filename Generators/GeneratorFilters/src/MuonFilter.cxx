/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/MuonFilter.h"

MuonFilter::MuonFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("Ptcut",m_Ptmin = 10000.);
  declareProperty("Etacut",m_EtaRange = 10.0);
}


StatusCode MuonFilter::filterEvent() {
  McEventCollection::const_iterator itr;
  for (itr = events_const()->begin(); itr!=events_const()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr ){
      if ((*pitr)->status() == 1 && abs((*pitr)->pdg_id()) == 13) {
        if ((*pitr)->momentum().perp() >= m_Ptmin && fabs((*pitr)->momentum().pseudoRapidity()) <= m_EtaRange) {
          return StatusCode::SUCCESS;
        }
      }
    }
  }
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
