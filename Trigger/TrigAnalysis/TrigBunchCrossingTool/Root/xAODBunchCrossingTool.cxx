/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODBunchCrossingTool.cxx 620838 2014-10-09 11:38:00Z krasznaa $

// EDM include(s):
#include "xAODTrigger/BunchConfContainer.h"
#include "xAODTrigger/BunchConfKey.h"

// Local include(s):
#include "TrigBunchCrossingTool/xAODBunchCrossingTool.h"

namespace Trig {

   xAODBunchCrossingTool::xAODBunchCrossingTool( const std::string& name )
   : BunchCrossingToolBase( name ),
     m_cache(), m_currentID( -1 ) {

      declareProperty( "MetaDataKey", m_metaKey = "BunchConfiguration" );
      declareProperty( "EventDataKey", m_eventKey = "BunchConfKey" );
   }

   StatusCode xAODBunchCrossingTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising..." );
      ATH_MSG_DEBUG( "MetaDataKey = " << m_metaKey );
      ATH_MSG_DEBUG( "EventDataKey = " << m_eventKey );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode xAODBunchCrossingTool::beginInputFile() {

      // Get the configuration object from the file:
      const xAOD::BunchConfContainer* metaCont = 0;
      ATH_CHECK( inputMetaStore()->retrieve( metaCont, m_metaKey ) );

      // Copy the information from the container into the tool's internal
      // cache:
      xAOD::BunchConfContainer::const_iterator itr = metaCont->begin();
      xAOD::BunchConfContainer::const_iterator end = metaCont->end();
      for( ; itr != end; ++itr ) {
         // Create/access the configuration object:
         BunchConfig& conf = m_cache[ ( *itr )->configID() ];
         // Fill it with information:
         conf.filledBCIDs = ( *itr )->collidingBCIDs();
         if( ( *itr )->intensitiesBeam1Available() ) {
            conf.filledIntBeam1 = ( *itr )->intensitiesBeam1();
         } else {
            conf.filledIntBeam1.clear();
         }
         if( ( *itr )->intensitiesBeam2Available() ) {
            conf.filledIntBeam2 = ( *itr )->intensitiesBeam2();
         } else {
            conf.filledIntBeam2.clear();
         }
         if( ( *itr )->unpairedBCIDsBeam1Available() ) {
            conf.unpairedBCIDsBeam1 = ( *itr )->unpairedBCIDsBeam1();
         } else {
            conf.unpairedBCIDsBeam1.clear();
         }
         if( ( *itr )->unpairedBCIDsBeam2Available() ) {
            conf.unpairedBCIDsBeam2 = ( *itr )->unpairedBCIDsBeam2();
         } else {
            conf.unpairedBCIDsBeam2.clear();
         }
         if( ( *itr )->unpairedIntensitiesBeam1Available() ) {
            conf.unpairedIntBeam1 = ( *itr )->unpairedIntensisitesBeam1();
         } else {
            conf.unpairedIntBeam1.clear();
         }
         if( ( *itr )->unpairedIntensitiesBeam2Available() ) {
            conf.unpairedIntBeam2 = ( *itr )->unpairedIntensisitesBeam2();
         } else {
            conf.unpairedIntBeam2.clear();
         }
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode xAODBunchCrossingTool::beginEvent() {

      // Get the event object:
      const xAOD::BunchConfKey* key = 0;
      ATH_CHECK( evtStore()->retrieve( key, m_eventKey ) );

      // If this is the currently loaded ID, we're done already:
      if( key->id() == m_currentID ) {
         return StatusCode::SUCCESS;
      }

      // If not, check if we know the configuration for this ID:
      Cache_t::const_iterator config_itr = m_cache.find( key->id() );
      if( config_itr == m_cache.end() ) {
         // Whoops, we don't know this configuration...
         ATH_MSG_ERROR( "No configuration available for ID "
                        << key->id() );
         return StatusCode::FAILURE;
      }

      // Load the new configuration:
      ATH_CHECK( loadSingleBunches( config_itr->second.filledBCIDs,
                                    config_itr->second.filledIntBeam1,
                                    config_itr->second.filledIntBeam2 ) );
      ATH_CHECK( loadBunchTrains( config_itr->second.filledBCIDs,
                                  config_itr->second.filledIntBeam1,
                                  config_itr->second.filledIntBeam2 ) );
      ATH_CHECK( loadUnpairedBunches( config_itr->second.unpairedBCIDsBeam1,
                                      config_itr->second.unpairedBCIDsBeam2,
                                      config_itr->second.unpairedIntBeam1,
                                      config_itr->second.unpairedIntBeam2 ) );

      // Remember that this is the new configuration:
      m_currentID = key->id();

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace Trig
