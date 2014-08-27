/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/MultiLeptonFilter.h"


MultiLeptonFilter::MultiLeptonFilter(const std::string& name, ISvcLocator* pSvcLocator)
: GenFilter(name, pSvcLocator)
{
  declareProperty("Ptcut", m_Ptmin = 10000.);
  declareProperty("Etacut", m_EtaRange = 10.0);
  declareProperty("NLeptons", m_NLeptons = 4);
}


StatusCode MultiLeptonFilter::filterEvent() {
  McEventCollection::const_iterator itr;
  int numLeptons = 0;
  for (itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = *itr;
    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin();	pitr != genEvt->particles_end(); ++pitr) {
      if ( (*pitr)->status() != 1) continue;
	  if ( abs((*pitr)->pdg_id()) == 11 || abs((*pitr)->pdg_id()) == 13 ) {
	    if ((*pitr)->momentum().perp() >= m_Ptmin && fabs((*pitr)->momentum().pseudoRapidity()) <= m_EtaRange) {
	      numLeptons += 1;
	    }
	  }
	}
  }
  ATH_MSG_INFO("Found " << numLeptons << "Leptons");
  setFilterPassed(numLeptons >= m_NLeptons);
  return StatusCode::SUCCESS;
}
