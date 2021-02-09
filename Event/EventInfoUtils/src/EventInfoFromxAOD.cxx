
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfoUtils/EventInfoFromxAOD.h"


/**
 * Create EventID object from xAOD::EventInfo
 *
 * @param xaod The xAOD::EventInfo object
 * @returns <code>EventID</code>
 */
   
EventID eventIDFromxAOD( const xAOD::EventInfo* xaod )
{
   return EventID( xaod->runNumber(), 
                   xaod->eventNumber(),
                   xaod->timeStamp(),
                   xaod->timeStampNSOffset(),
                   xaod->lumiBlock(),
                   xaod->bcid() );
}

   
/**
 * Create EventType object from xAOD::EventInfo
 *
 * @param xaod The xAOD::EventInfo object
 * @returns <code>EventType</code>
 */
   
EventType eventTypeFromxAOD( const xAOD::EventInfo* xaod )
{
   EventType et;
   // Copy DetDescr tags
   std::string ddtags;
   for( const auto& ddtag: xaod->detDescrTags() ) {
      //MN  FIX: this is only my guess how it should look like
      ddtags += '#' + ddtag.first + '#' + ddtag.second;
   }
   et.set_user_type( ddtags );
      
   if( xaod->eventType(xAOD::EventInfo::IS_TESTBEAM) ) {
      et.add_type(EventType::IS_TESTBEAM);
   }
   if( xaod->eventType(xAOD::EventInfo::IS_CALIBRATION) ) {
      et.add_type(EventType::IS_CALIBRATION);
   }
   if( xaod->eventType(xAOD::EventInfo::IS_SIMULATION) ) {
      et.add_type(EventType::IS_SIMULATION);

      et.set_mc_channel_number( xaod->mcChannelNumber() );
      et.set_mc_event_number( xaod->mcEventNumber() );
      for( size_t i=0; i< xaod->mcEventWeights().size(); i++ ) {
         et.set_mc_event_weight( xaod->mcEventWeight(i), i, xaod->mcEventWeights().size() );
      }
   }

   return et;
}

