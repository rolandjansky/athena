/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventDuplicateFinderAlg.cxx 663741 2015-04-29 11:52:21Z krasznaa $

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"

// Local include(s):
#include "EventDuplicateFinderAlg.h"

namespace xAODReader {

   EventDuplicateFinderAlg::EventDuplicateFinderAlg( const std::string& name,
                                                     ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_seenEvents() {

      declareProperty( "EventInfoKey", m_key = "EventInfo",
                       "StoreGate key of the xAOD::EventInfo object to be "
                       "used" );
   }

   StatusCode EventDuplicateFinderAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( "EventInfoKey = " << m_key );

      // Reset the internal variable:
      m_seenEvents.clear();

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode EventDuplicateFinderAlg::execute() {

      // Retrieve the EI object:
      const xAOD::EventInfo* ei = nullptr;
      ATH_CHECK( evtStore()->retrieve( ei, m_key ) );

      // Check if we saw this event already or not:
      if( ! m_seenEvents.insert(
                         std::make_pair( ei->runNumber(),
                                         ei->eventNumber() ) ).second ) {
         ATH_MSG_FATAL( "Seeing run #" << ei->runNumber() << ", event #"
                        << ei->eventNumber() << " for the second time" );
         return StatusCode::FAILURE;
      }

      // No, we just saw it now for the first time:
      ATH_MSG_VERBOSE( "Saw run #" << ei->runNumber() << ", event #"
                       << ei->eventNumber() << " for the first time" );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODReader
