/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EventInfoMgt includes
#include "EventInfoMCWeightFixAlg.h"

#include "xAODTruth/TruthEventContainer.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"

EventInfoMCWeightFixAlg::EventInfoMCWeightFixAlg( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm( name, pSvcLocator ){

}


EventInfoMCWeightFixAlg::~EventInfoMCWeightFixAlg() {}


StatusCode EventInfoMCWeightFixAlg::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode EventInfoMCWeightFixAlg::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode EventInfoMCWeightFixAlg::execute() {  
  setFilterPassed(false); //will set to true if we actually fixed EventInfo

  const EventInfo* evtInfo = 0;
  CHECK( evtStore()->retrieve( evtInfo ) );
  //check we are MC
  if( !evtInfo->event_type()->test(EventType::IS_SIMULATION) ) return StatusCode::SUCCESS;

  const xAOD::TruthEventContainer* truthEvents = 0;
  CHECK( evtStore()->retrieve( truthEvents , "TruthEvents" ) );

  //check if the first event in TruthEvents has same vector of weights as EventInfo (just check size)
  if( truthEvents->size()==0 || truthEvents->at(0)->weights().size() == evtInfo->event_type()->n_mc_event_weights() ) return StatusCode::SUCCESS;

  auto& weights = truthEvents->at(0)->weights();

  //got here, so there must be a mismatch
  ATH_MSG_VERBOSE("Fixing weights: TruthEvent[0].weights().size() = " << weights.size() << " vs EventInfo n_mc_event_weights() = " <<  evtInfo->event_type()->n_mc_event_weights() );

  for(unsigned int i=0;i<weights.size();i++) {
    evtInfo->event_type()->set_mc_event_weight( weights[i] , i );
  }

  setFilterPassed(true);


  return StatusCode::SUCCESS;
}


