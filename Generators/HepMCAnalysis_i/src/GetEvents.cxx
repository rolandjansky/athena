/**
 *  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 *  Author: Andrii Verbytsky andrii.verbytskyi@mpp.mpg.de
 * 
 *  This file implements helper functions that prepare the ATLAS MC events 
 *  to be used with the HepMCAnalysis package.
 */
#include <vector>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "Gaudi/Property.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "GeneratorObjects/McEventCollection.h"

//class HepMCAnalysisGenEvent;
using HepMCAnalysisGenEvent=HepMC::GenEvent;
HepMCAnalysisGenEvent* PrepareHepMCAnalysisGenEvent(const HepMC::GenEvent* cevent);

StatusCode   GetRunEventNumber(AthAlgorithm* a,int& runNumber,int& evtNumber,  const std::string im)
{
  // load event info
  const EventInfo* mcInfoptr;
  if ( a->evtStore()->retrieve( mcInfoptr, im ).isFailure() ) {
    //ATH_MSG_ERROR( "Could not retrieve EventInfo" );
    return StatusCode::FAILURE;
  
  } else{
    runNumber = mcInfoptr->event_ID()->run_number();
    evtNumber = mcInfoptr->event_ID()->event_number();
    //ATH_MSG_DEBUG( "run: " << runNumber << " event: " << evtNumber );
    //ATH_MSG_DEBUG("Event info loaded");
  }
  return StatusCode::SUCCESS;
}


StatusCode  GetEvents(AthAlgorithm* a,std::vector<HepMCAnalysisGenEvent*>& evts, const std::string km)
{
  // load HepMC info
  const McEventCollection* mcCollptr;
  if ( a->evtStore()->retrieve( mcCollptr, km ).isFailure() ) {
    //ATH_MSG_WARNING( "Could not retrieve McEventCollection" );  
  } else { 
    //ATH_MSG_DEBUG("HepMC info loaded");

    // loop over all events in McEventCollection
    for ( McEventCollection::const_iterator itr = mcCollptr->begin(); itr != mcCollptr->end(); ++itr ) {
	evts.push_back(PrepareHepMCAnalysisGenEvent(*itr));
}
}
  return StatusCode::SUCCESS;
}