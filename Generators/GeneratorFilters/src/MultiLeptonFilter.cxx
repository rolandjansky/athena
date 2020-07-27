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
    for (auto part: *genEvt) {
      if ( part->status() != 1) continue;
	  if ( abs(part->pdg_id()) == 11 || abs(part->pdg_id()) == 13 ) {
	    if (part->momentum().perp() >= m_Ptmin && std::abs(part->momentum().pseudoRapidity()) <= m_EtaRange) {
	      numLeptons += 1;
	    }
	  }
	}
  }
  ATH_MSG_INFO("Found " << numLeptons << "Leptons");
  setFilterPassed(numLeptons >= m_NLeptons);
  return StatusCode::SUCCESS;
}
