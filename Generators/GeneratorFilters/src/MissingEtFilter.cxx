/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/MissingEtFilter.h"
#include "TruthUtils/HepMCHelpers.h"


MissingEtFilter::MissingEtFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("METCut",m_METmin = 10000.);
}


StatusCode MissingEtFilter::filterEvent() {
  double sumx(0), sumy(0);
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {
      if (!MC::isGenStable(*pitr)) continue;

      // Treat all non-interacting particles identically
      // We want Missing Transverse Momentum, not "Missing Transverse Energy"
      if (MC::isNonInteracting(*pitr)) {
	ATH_MSG_VERBOSE("Found noninteracting particle: ID = " << (*pitr)->pdg_id() << " PX = " << (*pitr)->momentum().px() << " PY = "<< (*pitr)->momentum().py());
	sumx += (*pitr)->momentum().px();
        sumy += (*pitr)->momentum().py();
      }
    }
  }

  // Now see what the total missing Et is and compare to minimum
  double met = sqrt(sumx*sumx + sumy*sumy);
  ATH_MSG_DEBUG("Totals for event: EX = " << sumx << ", EY = "<< sumy << ", ET = " << met);
  setFilterPassed(met >= m_METmin);
  return StatusCode::SUCCESS;
}
