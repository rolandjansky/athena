/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfoReaderAlg.cxx 682548 2015-07-13 13:57:15Z krasznaa $

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"

// Local include(s):
#include "EventInfoReaderAlg.h"

namespace {

   /// Helper output operator
   template< class T1, class T2 >
   std::ostream& operator<< ( std::ostream& out,
                              const std::pair< T1, T2 >& p ) {

      out << "{" << p.first << ", " << p.second << "}";
      return out;
   }

   /// Helper output operator
   template< class T >
   std::ostream& operator<< ( std::ostream& out,
                              const std::vector< T >& vec ) {

      out << "[";
      for( size_t i = 0; i < vec.size(); ++i ) {
         out << vec[ i ];
         if( i < ( vec.size() - 1 ) ) {
            out << ", ";
         }
      }
      out << "]";

      return out;
   }

} // private namespace

namespace xAODReader {

   EventInfoReaderAlg::EventInfoReaderAlg( const std::string& name,
                                           ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ) {

      declareProperty( "SGKey", m_key = "EventInfo" );
   }

   StatusCode EventInfoReaderAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode EventInfoReaderAlg::execute() {

      // Get the EventInfo object:
      const xAOD::EventInfo* ei = nullptr;
      ATH_CHECK( evtStore()->retrieve( ei, m_key ) );

      // Basic info:
      ATH_MSG_DEBUG( "runNumber = " << ei->runNumber() );
      ATH_MSG_DEBUG( "eventNumber = " << ei->eventNumber() );
      ATH_MSG_DEBUG( "lumiBlock = " << ei->lumiBlock() );
      ATH_MSG_DEBUG( "timeStamp = " << ei->timeStamp() );
      ATH_MSG_DEBUG( "timeStampNSOffset = " << ei->timeStampNSOffset() );
      ATH_MSG_DEBUG( "bcid = " << ei->bcid() );
      ATH_MSG_DEBUG( "detectorMask = " << ei->detectorMask() );

      // Event type info:
      ATH_MSG_DEBUG( "detDescrTags = " << ei->detDescrTags() );
      ATH_MSG_DEBUG( "eventTypeBitmask ="
                     << ( ei->eventType( xAOD::EventInfo::IS_SIMULATION ) ?
                          " IS_SIMULATION" : "" )
                     << ( ei->eventType( xAOD::EventInfo::IS_TESTBEAM ) ?
                          " IS_TESTBEAM" : "" )
                     << ( ei->eventType( xAOD::EventInfo::IS_CALIBRATION ) ?
                          " IS_CALIBRATION" : "" ) );
      if( ei->eventType( xAOD::EventInfo::IS_SIMULATION ) ) {
         ATH_MSG_DEBUG( "mcChannelNumber = " << ei->mcChannelNumber() );
         ATH_MSG_DEBUG( "mcEventNumber = " << ei->mcEventNumber() );
         ATH_MSG_DEBUG( "mcEventWeights = " << ei->mcEventWeights() );
      }

      // Check if it has sub-events:
      try {
         const auto& subEvents = ei->subEvents();
         int counter = 0;
         for( const xAOD::EventInfo::SubEvent& se : subEvents ) {
            ATH_MSG_DEBUG( "SubEvent #" << counter++ );
            ATH_MSG_DEBUG( "   time = " << se.time() );
            ATH_MSG_DEBUG( "   type = " << se.typeName() );
            const xAOD::EventInfo* sei = se.ptr();
            if( sei ) {
               ATH_MSG_DEBUG( "   runNumber = " << sei->runNumber() );
               ATH_MSG_DEBUG( "   eventNumber = " << sei->eventNumber() );
               ATH_MSG_DEBUG( "   bcid = " << sei->bcid() );
            }
         }
      } catch( const std::exception& ) {
         // Nope, it doesn't...
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODReader
