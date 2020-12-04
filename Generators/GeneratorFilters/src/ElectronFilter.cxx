/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/ElectronFilter.h"


ElectronFilter::ElectronFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("Ptcut", m_Ptmin = 10000.);
  declareProperty("Etacut", m_EtaRange = 10.0);
}


StatusCode ElectronFilter::filterEvent() {
  for (McEventCollection::const_iterator itr = events_const()->begin(); itr != events_const()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = *itr;
    for (HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin(); pitr!=genEvt->particles_end(); ++pitr ) {
      if ((*pitr)->status() == 1 && abs((*pitr)->pdg_id()) == 11) { // electron
        if ((*pitr)->momentum().perp() >= m_Ptmin && fabs((*pitr)->momentum().pseudoRapidity()) <= m_EtaRange) {
          return StatusCode::SUCCESS;
        }
      }
    }
  }
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
