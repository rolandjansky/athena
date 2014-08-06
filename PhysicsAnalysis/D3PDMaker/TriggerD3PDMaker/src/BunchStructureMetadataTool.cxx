/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchStructureMetadataTool.cxx 511909 2012-07-31 11:50:19Z krasznaa $

// Athena/Gaudi include(s):
#include "AthenaKernel/errorcheck.h"

// D3PDMaker include(s):
#include "D3PDMakerInterfaces/ID3PD.h"

// Local include(s):
#include "BunchStructureMetadataTool.h"

namespace D3PD {

   static const std::string BUNCH_CONFIG_INCIDENT_NAME = "BunchConfig";

   BunchStructureMetadataTool::BunchStructureMetadataTool( const std::string& type,
                                                           const std::string& name,
                                                           const IInterface* parent )
      : AthAlgTool( type, name, parent ),
        m_configDir( "" ),
        m_incidentSvc( "IncidentSvc", name ),
        m_d3pdSvc( "D3PD::RootD3PDSvc/BCD3PDSvc", name ),
        m_bcConfProv( "Trig::TrigConfBunchCrossingTool/BunchCrossingTool" ),
        m_configTree( 0 ), m_confId( 0 ), m_filledBunches( 0 ),
        m_bunchIntensitiesBeam1( 0 ), m_bunchIntensitiesBeam2( 0 ),
        m_unpairedBunchesBeam1( 0 ), m_unpairedBunchesBeam2( 0 ),
        m_unpairedBunchIntensitiesBeam1( 0 ), m_unpairedBunchIntensitiesBeam2( 0 ) {

      // Declare the interfaces provided by the tool:
      declareInterface< IMetadataTool >( this );
      declareInterface< IIncidentListener >( this );

      // Declare the properties of the tool:
      declareProperty( "ConfigDir", m_configDir = "/D3PD/d3pdMeta" );

      declareProperty( "IncidentSvc", m_incidentSvc );
      declareProperty( "D3PDSvc", m_d3pdSvc );
      declareProperty( "BCConfProvider", m_bcConfProv );
   }

   StatusCode BunchStructureMetadataTool::initialize() {

      ATH_MSG_INFO( "Initializing. Package version: " << PACKAGE_VERSION );

      // Retrieve the needed services:
      CHECK( m_incidentSvc.retrieve() );
      CHECK( m_d3pdSvc.retrieve() );
      // ...and tools:
      CHECK( m_bcConfProv.retrieve() );

      // Set up the tool to listen to incidents about the change of the bunch
      // structure:
      m_incidentSvc->addListener( this, BUNCH_CONFIG_INCIDENT_NAME );

      // Create the configuration objects through the D3PD service:
      CHECK( m_d3pdSvc->make( m_configDir + "/BunchConfTree", m_configTree ) );

      // Add the variables to the configuration "tree":
      CHECK( m_configTree->addVariable( "ConfigID",         m_confId,
                                        "ID of this bunch configuration" ) );
      CHECK( m_configTree->addVariable( "FilledBunches",    m_filledBunches,
                                        "BCIDs of the filled bunch crossings" ) );
      CHECK( m_configTree->addVariable( "FilledBunchIntBeam1", m_bunchIntensitiesBeam1,
                                        "Beam1 intensities of the filled bunch crossings" ) );
      CHECK( m_configTree->addVariable( "FilledBunchIntBeam2", m_bunchIntensitiesBeam2,
                                        "Beam1 intensities of the filled bunch crossings" ) );
      CHECK( m_configTree->addVariable( "UnpairedBeam1",  m_unpairedBunchesBeam1,
                                        "BCIDs of the beam 1 unpaired bunch crossings" ) );
      CHECK( m_configTree->addVariable( "UnpairedBeam2",  m_unpairedBunchesBeam2,
                                        "BCIDs of the beam 2 unpaired bunch crossings" ) );
      CHECK( m_configTree->addVariable( "UnpairedIntBeam1", m_unpairedBunchIntensitiesBeam1,
                                        "Intensities of the beam 1 unpaired bunch crossings" ) );
      CHECK( m_configTree->addVariable( "UnpairedIntBeam2", m_unpairedBunchIntensitiesBeam2,
                                        "Intensities of the beam 2 unpaired bunch crossings" ) );

      // Clear the cache, just to be sure:
      m_configKeyCache.clear();

      return StatusCode::SUCCESS;
   }

   StatusCode BunchStructureMetadataTool::writeMetadata( ID3PD* ) {

      ATH_MSG_INFO( "Bunch configuration available in the D3PD in directory: \""
                    << m_configDir << "\"" );

      return StatusCode::SUCCESS;
   }

   void BunchStructureMetadataTool::handle( const Incident& inc ) {

      // Check if we received the expected incident:
      if( inc.type() != BUNCH_CONFIG_INCIDENT_NAME ) {
         REPORT_MESSAGE( MSG::WARNING ) << "Received incident not recognised";
         return;
      }

      // Check if we already have this configuration:
      if( m_configKeyCache.find( m_bcConfProv->configID() ) !=
          m_configKeyCache.end() ) {
         ATH_MSG_DEBUG( "Configuration with ID " << m_bcConfProv->configID()
                        << " already stored" );
         return;
      }

      // Let the user know what we're doing:
      ATH_MSG_INFO( "Storing bunch configuration with ID: "
                    << m_bcConfProv->configID() );

      // Fill the variables:
      *m_confId           = m_bcConfProv->configID();
      *m_filledBunches    = m_bcConfProv->configuredBCIDs();
      *m_bunchIntensitiesBeam1 = m_bcConfProv->configuredIntensitiesBeam1();
      *m_bunchIntensitiesBeam2 = m_bcConfProv->configuredIntensitiesBeam2();
      *m_unpairedBunchesBeam1  = m_bcConfProv->configuredUnpairedBCIDsBeam1();
      *m_unpairedBunchesBeam2  = m_bcConfProv->configuredUnpairedBCIDsBeam2();
      *m_unpairedBunchIntensitiesBeam1 = m_bcConfProv->configuredUnpairedIntensitiesBeam1();
      *m_unpairedBunchIntensitiesBeam2 = m_bcConfProv->configuredUnpairedIntensitiesBeam2();

      // Fill the configuration into the output:
      if( m_configTree->capture().isFailure() ) {
         REPORT_MESSAGE( MSG::ERROR ) << "Couldn't save the configuration for "
                                      << "ID: " << m_bcConfProv->configID();
         return;
      }

      // Update the book-keeping variable:
      m_configKeyCache.insert( m_bcConfProv->configID() );

      return;
   }

} // namespace D3PD
