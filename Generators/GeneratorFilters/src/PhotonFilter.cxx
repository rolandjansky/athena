/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Will pass if there are the specified number of photons with pT and eta in the specified range
#include "GeneratorFilters/PhotonFilter.h"

PhotonFilter::PhotonFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("PtMin", m_Ptmin = 10000.);
  declareProperty("PtMax", m_Ptmax = 100000000.);
  declareProperty("EtaCut", m_EtaRange = 2.50);
  declareProperty("NPhotons", m_NPhotons = 2);

  // Backward compatibility aliases
  declareProperty("Ptcut", m_Ptmin = 10000.);
  declareProperty("Etacut", m_EtaRange = 2.50);
}


StatusCode PhotonFilter::filterEvent() {
  int NPhotons = 0;
  for (McEventCollection::const_iterator itr = events_const()->begin(); itr != events_const()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin();	pitr != genEvt->particles_end(); ++pitr) {
      if ((*pitr)->pdg_id() == 22 && (*pitr)->status()==1 &&
          (*pitr)->momentum().perp() >= m_Ptmin &&
          (*pitr)->momentum().perp() <  m_Ptmax &&
          fabs((*pitr)->momentum().pseudoRapidity()) <= m_EtaRange) NPhotons++;
    }
  }
  setFilterPassed(NPhotons >= m_NPhotons);
  return StatusCode::SUCCESS;
}
