/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/LeadingPhotonFilter.h"

LeadingPhotonFilter::LeadingPhotonFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("PtMin", m_Ptmin = 10000.);
  declareProperty("PtMax", m_Ptmax = 100000000.);
  declareProperty("EtaCut", m_EtaRange = 2.50);
}


StatusCode LeadingPhotonFilter::filterEvent() {
  const HepMC::GenEvent* genEvt = (*(events()->begin()));
  double ptmax = 0;
  for (auto p: *genEvt) {
    if (p->pdg_id() != 22 || p->status() != 1) continue;
    if (std::abs(p->momentum().pseudoRapidity()) > m_EtaRange) continue;
    ptmax = std::max(p->momentum().perp(), ptmax);
  }
  setFilterPassed(ptmax >= m_Ptmin && ptmax < m_Ptmax);
  return StatusCode::SUCCESS;
}
