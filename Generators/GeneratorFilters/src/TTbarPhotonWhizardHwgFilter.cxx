/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/TTbarPhotonWhizardHwgFilter.h"

TTbarPhotonWhizardHwgFilter::TTbarPhotonWhizardHwgFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("Ptcut",m_Ptmin = 10000.);
  declareProperty("Etacut",m_Etamax = 10.);
}


StatusCode TTbarPhotonWhizardHwgFilter::filterEvent() {
  for (McEventCollection::const_iterator itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
	for (HepMC::GenEvent::particle_const_iterator pit = genEvt->particles_begin();pit != genEvt->particles_end(); ++pit) {
      if ((*pit)->pdg_id() == 22 && (*pit)->status() == 124 &&
          (*pit)->momentum().perp() >= m_Ptmin && fabs((*pit)->momentum().pseudoRapidity()) < m_Etamax) {
        setFilterPassed(true);
        return StatusCode::SUCCESS;
      }
	}
  }
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
