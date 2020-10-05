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
  for (itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (auto part: *genEvt) {
      if ( part->status() != 1) continue;
      if ( std::abs(part->pdg_id()) != 11) continue;
	  if ( (part->momentum().perp() >= m_Ptmin) && std::abs(part->momentum().pseudoRapidity()) <= m_EtaRange) {
	    numElectrons++;
	  }
	}
  }
  ATH_MSG_INFO("Found " << numElectrons << "Electrons");
  setFilterPassed(numElectrons >= m_NElectrons);
  return StatusCode::SUCCESS;
}
