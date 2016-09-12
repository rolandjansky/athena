/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfoMCWeightFixAlg.cxx 772715 2016-09-12 11:37:15Z krasznaa $

// EDM include(s):
#include "xAODTruth/TruthEventContainer.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"

// Local include(s):
#include "EventInfoMCWeightFixAlg.h"

EventInfoMCWeightFixAlg::EventInfoMCWeightFixAlg( const std::string& name,
                                                  ISvcLocator* pSvcLocator )
  : AthAlgorithm( name, pSvcLocator ) {

   declareProperty( "TruthEventsKey", m_truthEventsKey = "TruthEvents",
                    "SG key for the xAOD::TruthEventContainer to be used" );
}

StatusCode EventInfoMCWeightFixAlg::execute() {  

   // If no fix is applied, the sequence should stop:
   setFilterPassed( false );

   // Retrieve the object in question:
   const EventInfo* evtInfo = 0;
   ATH_CHECK( evtStore()->retrieve( evtInfo ) );

   // Check that it's an MC sample:
   if( ! evtInfo->event_type()->test( EventType::IS_SIMULATION ) ) {
      return StatusCode::SUCCESS;
   }

   // Retrieve the container helping in the fix:
   const xAOD::TruthEventContainer* truthEvents = 0;
   ATH_CHECK( evtStore()->retrieve( truthEvents , m_truthEventsKey ) );

   // Check if the first event in TruthEvents has same vector of weights as
   // EventInfo (just check size):
   if( ( truthEvents->size() == 0 ) ||
       ( truthEvents->at( 0 )->weights().size() ==
         evtInfo->event_type()->n_mc_event_weights() ) ) {
      return StatusCode::SUCCESS;
   }

   // Get the weights of the signal event:
   auto& weights = truthEvents->at( 0 )->weights();

   // Got here, so there must be a mismatch...
   ATH_MSG_VERBOSE( "Fixing weights: TruthEvent[0].weights().size() = "
                    << weights.size() << " vs EventInfo n_mc_event_weights() = "
                    <<  evtInfo->event_type()->n_mc_event_weights() );

   // Fix the values in an old style loop:
   for( unsigned int i = 0; i < weights.size(); ++i ) {
      evtInfo->event_type()->set_mc_event_weight( weights[ i ] , i );
   }

   // Let the sequence know, that it should continue:
   setFilterPassed( true );

   // Return gracefully:
   return StatusCode::SUCCESS;
}
