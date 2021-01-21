/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/MultiElectronFilter.h"


MultiElectronFilter::MultiElectronFilter(const std::string& name, ISvcLocator* pSvcLocator)
: GenFilter(name,pSvcLocator)
{
  declareProperty("Ptcut",m_Ptmin = 10000.);
  declareProperty("Etacut",m_EtaRange = 10.0);
  declareProperty("NElectrons",m_NElectrons = 2);
}


StatusCode MultiElectronFilter::filterEvent() {
  McEventCollection::const_iterator itr;
  int numElectrons = 0;
  for (itr = events_const()->begin(); itr != events_const()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {
      if ( (*pitr)->status() != 1) continue;
      if ( abs((*pitr)->pdg_id()) != 11) continue;
	  if ( ((*pitr)->momentum().perp() >= m_Ptmin) && fabs((*pitr)->momentum().pseudoRapidity()) <= m_EtaRange) {
	    numElectrons++;
	  }
	}
  }
  ATH_MSG_INFO("Found " << numElectrons << "Electrons");
  setFilterPassed(numElectrons >= m_NElectrons);
  return StatusCode::SUCCESS;
}
