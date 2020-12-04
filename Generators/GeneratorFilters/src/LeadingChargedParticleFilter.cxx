/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "GeneratorFilters/LeadingChargedParticleFilter.h"
#include "TruthUtils/HepMCHelpers.h"

LeadingChargedParticleFilter::LeadingChargedParticleFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("PtMin", m_Ptmin = 10000.);
  declareProperty("PtMax", m_Ptmax = 100000000.);
  declareProperty("EtaCut", m_EtaRange = 2.50);
}


StatusCode LeadingChargedParticleFilter::filterEvent() {
  const HepMC::GenEvent* genEvt = (*(events_const()->begin()));
  double ptmax = 0;
  for (HepMC::GenEvent::particle_const_iterator p = genEvt->particles_begin(); p != genEvt->particles_end(); ++p) {
    if ((*p)->status()!=1 || fabs(MC::PID::charge((*p)->pdg_id()))<0.5) continue;
    if (fabs((*p)->momentum().pseudoRapidity()) > m_EtaRange) continue;
    ptmax = std::max((*p)->momentum().perp(), ptmax);
  }
  setFilterPassed(ptmax >= m_Ptmin && ptmax < m_Ptmax);
  return StatusCode::SUCCESS;
}
