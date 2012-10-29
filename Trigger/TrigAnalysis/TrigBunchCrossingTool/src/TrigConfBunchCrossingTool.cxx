/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigConfBunchCrossingTool.cxx 511865 2012-07-31 08:44:12Z krasznaa $

// STL include(s):
#include <algorithm>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Trigger config include(s):
#include "TrigConfL1Data/BunchGroup.h"
#include "TrigConfL1Data/BunchGroupSet.h"

// Local include(s):
#include "TrigConfBunchCrossingTool.h"
#include "TrigBunchCrossingTool/MsgWriter.h"

namespace Trig {

   /// Name of the trigger configuration incidents
   /**
    * Unfortunately the name of the trigger configuration incidents is not
    * published the same way as for the event incidents. So the easiest is just
    * to create a variable locally.
    */
   static const std::string TRIGCONF_INCIDENT_NAME = "TrigConf";

   TrigConfBunchCrossingTool::TrigConfBunchCrossingTool( const std::string& type,
                                                         const std::string& name,
                                                         const IInterface* parent )
      : AthAlgTool( type, name, parent ), m_bgId( -1 ),
        m_configSvc( "TrigConf::TrigConfigSvc/TrigConfigSvc", name ),
        m_incidentSvc( "IncidentSvc", name ) {

      // Declare the interfaces provided by the tool:
      declareInterface< IBunchCrossingTool >( this );
      declareInterface< IBunchCrossingConfProvider >( this );
      declareInterface< IIncidentListener >( this );

      // Declare the properties of the tool:
      declareProperty( "MaxBunchSpacing", m_maxBunchSpacing = 500,
                       "Maximum bunch spacing in the trains in nanoseconds" );
      declareProperty( "FilledBunchNames", m_filledBunchNames,
                       "Possible names for the filled bunch groups (DEPRECATED)" );

      declareProperty( "FrontLength", m_frontLength = 300,
                       "Length of the front part of a train in nanoseconds" );
      declareProperty( "TailLength", m_tailLength = 300,
                       "Length of the tail part of a train in nanoseconds" );

      declareProperty( "ConfigSvc", m_configSvc );
   }

   StatusCode TrigConfBunchCrossingTool::initialize() {

      // Reset he bunch group ID, so at the next update we definitely
      // load a new configuration:
      m_bgId = -1;

      // Configure the underlying logger:
      MsgWriter::instance()->setMinType( msg().level() );
      MsgWriter::instance()->setSource( name() );

      // Report about the initialization:
      ATH_MSG_INFO( "Initializing TrigConfBunchCrossingTool - package version: "
                    << PACKAGE_VERSION );
      ATH_MSG_INFO( "  Maximal bunch spacing: " << m_maxBunchSpacing << " ns" );
      ATH_MSG_INFO( "  Length of train front: " << m_frontLength << " ns" );
      ATH_MSG_INFO( "  Length of train tail : " << m_tailLength << " ns" );
      ATH_MSG_DEBUG( "  Filled bunch names   : " << m_filledBunchNames );
      ATH_MSG_DEBUG( "  LVL1 config service  : " << m_configSvc );

      // Configure the base class:
      setMaxBunchSpacing( m_maxBunchSpacing );
      setFrontLength( m_frontLength );
      setTailLength( m_tailLength );

      // Retrieve the incident service, and register the tool for the trigger
      // configuration incidents:
      CHECK( m_incidentSvc.retrieve() );
      m_incidentSvc->addListener( this, TRIGCONF_INCIDENT_NAME );

      // Retrieve the trigger configuration service:
      CHECK( m_configSvc.retrieve() );

      return StatusCode::SUCCESS;
   }

   StatusCode TrigConfBunchCrossingTool::finalize() {

      ATH_MSG_INFO( "Finalizing TrigConfBunchCrossingTool - package version: "
                    << PACKAGE_VERSION );

      return StatusCode::SUCCESS;
   }

   /**
    * The function checks if it received the expected type of incident, and then
    * instructs the tool to check if its configuration needs to be changed.
    */
   void TrigConfBunchCrossingTool::handle( const Incident& inc ) {

      if( inc.type() == TRIGCONF_INCIDENT_NAME ) {
         if( loadConfig().isFailure() ) {
            REPORT_MESSAGE( MSG::ERROR )
               << "Failed to load the new BunchGroup configuration!";
            REPORT_MESSAGE( MSG::ERROR )
               << "Results will be unreliable!";
            return;
         }
         return;
      }

      REPORT_MESSAGE( MSG::WARNING ) << "Received a not requested incident";
      return;
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

      // Check if the needed info is even there:
      if( ! m_configSvc->bunchGroupSet() ) {
         REPORT_ERROR( StatusCode::FAILURE )
            << "Trigger configuration service doesn't provide "
            << "BunchGroupSet information";
         REPORT_ERROR( StatusCode::FAILURE )
            << "Could you be reading MC files with this tool?";
         return StatusCode::FAILURE;
      }

      // Check if we already have the correct configuration:
      if( m_configSvc->bunchGroupSet()->id() == m_bgId ) {
         return StatusCode::SUCCESS;
      }

      // Report to the user what we're doing:
      ATH_MSG_INFO( "Updating tool configuration to BGKey #"
                    << m_configSvc->bunchGroupSet()->id() );

      // Print the configuration for debugging purposes, and remember which one
      // we're processing:
      m_bgId = m_configSvc->bunchGroupSet()->id();
      printBunchGroups();

      //
      // Select the collision bunch group:
      //
      const std::vector< TrigConf::BunchGroup >& bgs =
         m_configSvc->bunchGroupSet()->bunchGroups();
      std::vector< TrigConf::BunchGroup >::const_iterator filled_bg = bgs.begin();
      ++filled_bg;
      ATH_MSG_INFO( "Taking the second bunch group as the colliding bunch group" );

      //
      // Now interpret the information:
      //
      if( ! loadSingleBunches( filled_bg->bunches() ) ) {
         REPORT_ERROR( StatusCode::FAILURE ) << "Failed to interpret single bunches";
         return StatusCode::FAILURE;
      }
      if( ! loadBunchTrains( filled_bg->bunches() ) ) {
         REPORT_ERROR( StatusCode::FAILURE ) << "Failed to interpret bunch trains";
         return StatusCode::FAILURE;
      }

      //
      // Select the unpaired bunches. The overlaps are taken care of by the
      // base tool.
      //
      std::vector< int > unpaired;
      // These are the "unpaired isolated" bunches:
      if( bgs.size() > 4 ) unpaired.insert( unpaired.end(), bgs[ 4 ].bunches().begin(),
                                            bgs[ 4 ].bunches().end() );
      // These are the "unpaired non-isolated" bunches:
      if( bgs.size() > 5 ) unpaired.insert( unpaired.end(), bgs[ 5 ].bunches().begin(),
                                            bgs[ 5 ].bunches().end() );
      // These are the general unpaired bunches:
      /*
        In 2012 bunch group 7 no longer means unpaired bunches, but is a special
        group used for triggering on bunches excluding the first 3 bunches of trains.

      if( bgs.size() > 7 ) unpaired.insert( unpaired.end(), bgs[ 7 ].bunches().begin(),
                                            bgs[ 7 ].bunches().end() );
      */

      //
      // Now interpret the information:
      //
      if( ! loadUnpairedBunches( unpaired, unpaired ) ) {
         REPORT_ERROR( StatusCode::FAILURE ) << "Failed to interpret unpaired bunches";
         return StatusCode::FAILURE;
      }

      // Print the configuration to give some feedback to the user:
      printConfig();

      // Let everybody know that the configuration of the tool has changed:
      m_incidentSvc->fireIncident( Incident( "BunchConfiguration update",
                                             BUNCH_CONFIG_INCIDENT_NAME ) );

      return StatusCode::SUCCESS;
   }

   /**
    * This function is only here for debugging. It prints practically the full
    * configuration information that the tool has to work with.
    */
   void TrigConfBunchCrossingTool::printBunchGroups() const {

      REPORT_MESSAGE( MSG::VERBOSE ) << "Printing BunchGroup configuration:";
      const std::vector< TrigConf::BunchGroup >& bgs =
         m_configSvc->bunchGroupSet()->bunchGroups();
      std::vector< TrigConf::BunchGroup >::const_iterator itr = bgs.begin();
      std::vector< TrigConf::BunchGroup >::const_iterator end = bgs.end();
      for( ; itr != end; ++itr ) {
         REPORT_MESSAGE( MSG::VERBOSE ) << "  BG \"" << itr->name() << "\": "
                                        << itr->bunches();
      }

      return;
   }

}
