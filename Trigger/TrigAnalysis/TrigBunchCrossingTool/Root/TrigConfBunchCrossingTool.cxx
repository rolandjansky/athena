/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// STL include(s):
#include <algorithm>
#include <vector>

// Gaudi include(s):
#ifndef XAOD_STANDALONE
#   include "GaudiKernel/IIncidentSvc.h"
#endif // not XAOD_STANDALONE

// Trigger config include(s):
#include "TrigConfL1Data/BunchGroup.h"
#include "TrigConfL1Data/BunchGroupSet.h"

// Local include(s):
#include "TrigBunchCrossingTool/TrigConfBunchCrossingTool.h"

namespace {

   template< typename T >
   MsgStream& operator<< ( MsgStream& out, const std::vector< T >& vec ) {

      out << "[";
      for( size_t i = 0; i < vec.size(); ++i ) {
         out << vec[ i ];
         if( i < vec.size() - 1 ) {
            out << ", ";
         }
      }
      out << "]";

      // Return the stream:
      return out;
   }

} // private namespace

namespace Trig {

   TrigConfBunchCrossingTool::
   TrigConfBunchCrossingTool( const std::string& name )
      : BunchCrossingToolBase( name ), m_bgId( -1 ),
#ifndef XAOD_STANDALONE
        m_configSvc( "TrigConf::xAODConfigSvc/xAODConfigSvc", name ),
#endif // not XAOD_STANDALONE
        m_configTool( "TrigConf::xAODConfigTool" ) {

      // Declare the properties of the tool:
#ifndef XAOD_STANDALONE
      declareProperty( "ConfigSvc", m_configSvc );
#endif // not XAOD_STANDALONE
      declareProperty( "ConfigTool", m_configTool=nullptr );
   }

   StatusCode TrigConfBunchCrossingTool::initialize() {

      // Reset he bunch group ID, so at the next update we definitely
      // load a new configuration:
      m_bgId = -1;

      // Report about the initialization:
      ATH_MSG_INFO( "Initializing TrigConfBunchCrossingTool" );
      ATH_MSG_INFO( "  Maximal bunch spacing: " << m_maxBunchSpacing << " ns" );
      ATH_MSG_INFO( "  Length of train front: " << m_frontLength << " ns" );
      ATH_MSG_INFO( "  Length of train tail : " << m_tailLength << " ns" );
#ifndef XAOD_STANDALONE
      ATH_MSG_DEBUG( "  LVL1 config service  : " << m_configSvc );
#endif // XAOD_STANDALONE
      ATH_MSG_DEBUG( "  Trig config tool     : " << m_configTool );

      // Retrieve the trigger configuration service/tool:
#ifndef XAOD_STANDALONE
      if (!m_configTool.empty()) {
        ATH_MSG_DEBUG( "  xAODConfigTool is set - will read from xAOD metadata" );
        ATH_CHECK( m_configTool.retrieve() );
      } else {
        ATH_MSG_DEBUG( "  xAODConfigTool is not set - will read from xAODConfigSvc" );
        ATH_CHECK( m_configSvc.retrieve() );
      }
#else
      ATH_CHECK( m_configTool.retrieve() );
#endif // not XAOD_STANDALONE

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TrigConfBunchCrossingTool::beginEvent() {

      // Simply call the function doing the heavy lifting:
      ATH_CHECK( loadConfig() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This specialization of the function just returns the BGKey of the currently
    * loaded configuration. This is the simplest implementation for this function...
    */
   IBunchCrossingConfProvider::configid_type
   TrigConfBunchCrossingTool::configID() const {

      return static_cast< configid_type >( m_bgId );
   }

   /**
    * This function will always return an empty vector, because this tool doesn't
    * provide this sort of information. While saving the default 1.0 values for
    * all the filled bunches would have the same result right now, the tools
    * may handle the absence of the intensity information in different ways
    * in the future.
    *
    * @returns An empty vector
    */
   std::vector< float >
   TrigConfBunchCrossingTool::configuredIntensitiesBeam1() const {

      return std::vector< float >();
   }

   /**
    * This function will always return an empty vector, because this tool doesn't
    * provide this sort of information. While saving the default 1.0 values for
    * all the filled bunches would have the same result right now, the tools
    * may handle the absence of the intensity information in different ways
    * in the future.
    *
    * @returns An empty vector
    */
   std::vector< float >
   TrigConfBunchCrossingTool::configuredIntensitiesBeam2() const {

      return std::vector< float >();
   }

   /**
    * This function will always return an empty vector, because this tool doesn't
    * provide this sort of information. While saving the default 1.0 values for
    * all the unpaired bunches would have the same result right now, the tools
    * may handle the absence of the intensity information in different ways
    * in the future.
    *
    * @returns An empty vector
    */
   std::vector< float >
   TrigConfBunchCrossingTool::configuredUnpairedIntensitiesBeam1() const {

      return std::vector< float >();
   }

   /**
    * This function will always return an empty vector, because this tool doesn't
    * provide this sort of information. While saving the default 1.0 values for
    * all the unpaired bunches would have the same result right now, the tools
    * may handle the absence of the intensity information in different ways
    * in the future.
    *
    * @returns An empty vector
    */
   std::vector< float >
   TrigConfBunchCrossingTool::configuredUnpairedIntensitiesBeam2() const {

      return std::vector< float >();
   }

   /**
    * This function makes sure that the latest configuration from the TriggerDB is
    * loaded. It has to be called using some mechanism, which is the incident
    * mechanism at the moment.
    *
    * The function checks if the currently loaded configuration is the same as the
    * most recent configuration available from the trigger configuration service.
    * If it's not, then it loads the current configuration from the trigger config
    * service.
    *
    * @returns <code>StatusCode::SUCCESS</code> if the operation wa successful, or
    *          <code>StatusCode::FAILURE</code> if it wasn't.
    */
   StatusCode TrigConfBunchCrossingTool::loadConfig() {

      // Decide where to take the configuration from:
      TrigConf::IILVL1ConfigSvc* configSvc = 0;
#ifndef XAOD_STANDALONE
      if (m_configTool.isSet()) {
        configSvc = m_configTool.operator->();
      } else {
        configSvc = m_configSvc.operator->();
      }
#else
      configSvc = m_configTool.operator->();
#endif // not XAOD_STANDALONE

      // Check if the needed info is even there:
      if( ! configSvc->bunchGroupSet() ) {
         ATH_MSG_FATAL( "Trigger configuration service doesn't provide "
                        "BunchGroupSet information" );
         return StatusCode::FAILURE;
      }

      // Check if we already have the correct configuration:
      if( configSvc->bunchGroupSet()->id() == m_bgId ) {
         return StatusCode::SUCCESS;
      }

      // Report to the user what we're doing:
      ATH_MSG_INFO( "Updating tool configuration to BGKey #"
                    << configSvc->bunchGroupSet()->id() );

      // Print the configuration for debugging purposes, and remember which one
      // we're processing:
      m_bgId = configSvc->bunchGroupSet()->id();
      printBunchGroups( configSvc );

      //
      // Select the collision bunch group:
      //
      const std::vector< TrigConf::BunchGroup >& bgs =
         configSvc->bunchGroupSet()->bunchGroups();
      std::vector< TrigConf::BunchGroup >::const_iterator filled_bg =
         bgs.begin();
      ++filled_bg;
      ATH_MSG_INFO( "Taking the second bunch group as the colliding bunch "
                    "group" );

      //
      // Now interpret the information:
      //
      ATH_CHECK( loadSingleBunches( filled_bg->bunches() ) );
      ATH_CHECK( loadBunchTrains( filled_bg->bunches() ) );

      //
      // Select the unpaired bunches. The overlaps are taken care of by the
      // base tool.
      //
      std::vector< int > unpaired;
      // These are the "unpaired isolated" bunches:
      if( bgs.size() > 4 ) {
         unpaired.insert( unpaired.end(), bgs[ 4 ].bunches().begin(),
                          bgs[ 4 ].bunches().end() );
      }
      // These are the "unpaired non-isolated" bunches:
      if( bgs.size() > 5 ) {
         unpaired.insert( unpaired.end(), bgs[ 5 ].bunches().begin(),
                          bgs[ 5 ].bunches().end() );
      }

      //
      // Now interpret the information:
      //
      ATH_CHECK( loadUnpairedBunches( unpaired, unpaired ) );

      // Print the configuration to give some feedback to the user:
      printConfig();

#ifndef XAOD_STANDALONE
      // Let everybody know that the configuration of the tool has changed:
      ServiceHandle< IIncidentSvc > incidentSvc( "IncidentSvc", name() );
      ATH_CHECK( incidentSvc.retrieve() );
      incidentSvc->fireIncident( Incident( "BunchConfiguration update",
                                           BUNCH_CONFIG_INCIDENT_NAME ) );
#endif // not XAOD_STANDALONE

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This function is only here for debugging. It prints practically the full
    * configuration information that the tool has to work with.
    */
   void TrigConfBunchCrossingTool::
   printBunchGroups( const TrigConf::IILVL1ConfigSvc* svc ) const {

      // Skip the rest if nothing will be printed anyway:
      if( ! msgLvl( MSG::VERBOSE ) ) return;

      // Print the information:
      ATH_MSG_VERBOSE( "Printing BunchGroup configuration:" );
      for( const auto& bg : svc->bunchGroupSet()->bunchGroups() ) {
         ATH_MSG_VERBOSE( "  BG \"" << bg.name() << "\": " << bg.bunches() );
      }

      // Return gracefully:
      return;
   }

} // namespace Trig
