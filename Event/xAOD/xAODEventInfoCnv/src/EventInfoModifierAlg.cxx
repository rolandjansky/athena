//
// Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "EventInfoModifierAlg.h"

// EDM include(s).
#include "xAODEventInfo/EventInfo.h"

namespace xAODReader {

   EventInfoModifierAlg::EventInfoModifierAlg( const std::string& name,
                                               ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ) {

      // Declare the algorithm's properties.
      declareProperty( "EventInfoKey", m_key = "EventInfo",
                       "Event store key for the xAOD::EventInfo object to "
                       "modify" );
   }

   StatusCode EventInfoModifierAlg::execute() {

      // Retrieve a non-const pointer to the event info.
      xAOD::EventInfo* ei = nullptr;
      ATH_CHECK( evtStore()->retrieve( ei, m_key ) );
      ATH_MSG_VERBOSE( "Retrieved a non-const xAOD::EventInfo object "
                       "with key \"" << m_key << "\"" );

      // Now try to modify the object.
      ei->setRunNumber( ei->runNumber() + 1 );
      ei->setEventNumber( ei->eventNumber() - 1 );
      if( ei->isAvailable< uint32_t >( "mvEventNumber" ) ) {
         ei->setMCEventNumber( ei->mcEventNumber() + 2 );
      }
      ATH_MSG_VERBOSE( "Modified some static and dynamic properties of \""
                       << m_key << "\"" );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

} // namespace xAODReader
