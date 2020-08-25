/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Allows the user to search for particles with specified decay positions
// It will pass if there is an particle with c*t in the specified range
#include <fstream>

#include "GeneratorFilters/DecayPositionFilter.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataSvc.h"

#include "StoreGate/StoreGateSvc.h"

#include "HepPDT/TableBuilder.hh"

DecayPositionFilter::DecayPositionFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("RCut",m_RCut = 400.);
  declareProperty("PDG",m_PDGID = 3000001);
  declareProperty("MinPass",m_MinPass=2);
}


StatusCode DecayPositionFilter::filterInitialize() {
  ATH_MSG_INFO("RCut=" << m_RCut);
  ATH_MSG_INFO("PDG=" << m_PDGID);
  return StatusCode::SUCCESS;
}

StatusCode DecayPositionFilter::filterEvent() {
  int nPass = 0;
  for (McEventCollection::const_iterator itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (auto pitr: *genEvt) {
      if (std::abs(pitr->pdg_id()) != m_PDGID)  continue;
	    if ( !std::abs( pitr->momentum().pseudoRapidity()) ) continue;// AV:WHAT IS THIS?   
            // Count only particles not decaying to themselves
        bool notSelfDecay = true;
        if (!pitr->end_vertex()) continue;
        for ( auto child:  *(pitr->end_vertex())) {
                if ( child->pdg_id() == pitr->pdg_id() && HepMC::barcode(child)!=HepMC::barcode(pitr) && HepMC::barcode(child) < 100000) {
                  notSelfDecay = false;
                  break;
                }
         }
         if(notSelfDecay){
	     auto vtx = pitr->end_vertex();
		 double x = vtx->position().x();
		 double y = vtx->position().y();
		 double Rdecay = std::sqrt(x*x + y*y);
		 if(Rdecay < m_RCut ) nPass++;	
		 }
     }
    }
    setFilterPassed(nPass == m_MinPass);

  return StatusCode::SUCCESS;
}
