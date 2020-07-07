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
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for ( auto pitr: *genEvt){
    if (pitr->status() != 1 || std::abs(pitr->pdg_id()) != 13)  continue;
    if (pitr->momentum().perp() < m_Ptmin || std::abs(pitr->momentum().pseudoRapidity()) > m_EtaRange) continue;
    return StatusCode::SUCCESS;
    }
  }
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
