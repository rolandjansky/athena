/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "EventTrigInfoFillerTool.h"

using xAOD::EventInfo;

namespace D3PD {

   EventTrigInfoFillerTool::EventTrigInfoFillerTool( const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent )
      : BlockFillerTool< EventInfo >( type, name, parent ) {

      declareProperty( "Streams", m_configStreams,
                       "Streams for which the decision should be stored" );

      EventTrigInfoFillerTool::book().ignore();  // Avoid coverity warnings.
   }

   StatusCode EventTrigInfoFillerTool::book() {

      //
      // Create boolean variables with the names of the streams in question:
      //
      m_streams.resize( m_configStreams.size(), 0 );
      for( size_t i = 0; i < m_configStreams.size(); ++i ) {
         CHECK( addVariable( "streamDecision_" + m_configStreams[ i ],
                             m_streams[ i ],
                             "Did this event end up in the '" + m_configStreams[ i ] +
                             "' stream?" ) );
      }

      CHECK( addVariable( "l1id", m_l1id, "Extended LVL1 ID" ) );

      return StatusCode::SUCCESS;
   }

   StatusCode EventTrigInfoFillerTool::fill( const EventInfo& ei ) {

      // Access the needed information:
      const auto& stags = ei.streamTags();

      // Set the flags according to the stream decisions:
      for( size_t i = 0; i < m_configStreams.size(); ++i ) {
         if( streamPassed( stags, m_configStreams[ i ] ) ) {
            *( m_streams[ i ] ) = true;
         } else {
            *( m_streams[ i ] ) = false;
         }
      }

      *m_l1id = ei.extendedLevel1ID();

      return StatusCode::SUCCESS;
   }

   /**
    * This is a very simple algorithm for deciding whether the specified stream appears
    * in the list of streams that the current event was written to.
    *
    * @param stags The list of streams that the event was written to
    * @param sname Name of the stream that we're interested in
    * @returns <code>true</code> if the event ended up in the stream in question,
    *          <code>false</code> otherwise
    */
   bool
   EventTrigInfoFillerTool::streamPassed( const std::vector< EventInfo::StreamTag >& stags,
                                          const std::string& sname ) {

     //std::vector< TriggerInfo::StreamTag >::const_iterator itr = stags.begin();
     //std::vector< TriggerInfo::StreamTag >::const_iterator end = stags.end();
     //for( ; itr != end; ++itr ) {
     for (const auto& itr : stags) {
         if( itr.name() == sname ) return true;
      }

      return false;
   }

} // namespace D3PD
