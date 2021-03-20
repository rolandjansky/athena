/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchConfCnvAlg.cxx 583381 2014-02-14 15:58:20Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// xAOD include(s):
#include "xAODTrigger/BunchConf.h"
#include "xAODTrigger/BunchConfKey.h"
#include "xAODTrigger/BunchConfAuxContainer.h"

// Local include(s):
#include "BunchConfCnvAlg.h"

namespace xAODMaker {

   BunchConfCnvAlg::BunchConfCnvAlg( const std::string& name,
                                     ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_bcConf( "Trig::TrigConfBunchCrossingTool/BunchCrossingTool" ),
        m_metaStore( "MetaDataStore", name ),
        m_bcc( nullptr ) {

      declareProperty( "EventName", m_eventName = "BunchConfKey" );
      declareProperty( "MetaName", m_metaName = "BunchConfiguration" );

      declareProperty( "BCConfProvider", m_bcConf );
      declareProperty( "MetaDataStore", m_metaStore );
   }

   StatusCode BunchConfCnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( "  EventName = " << m_eventName );
      ATH_MSG_DEBUG( "  MetaName  = " << m_metaName );

      // Retrieve all the used component(s):
      CHECK( m_bcConf.retrieve() );
      CHECK( m_metaStore.retrieve() );

      // Clear the internal cache:
      m_convertedIDs.clear();

      // Create an empty bunch configuration container:
      xAOD::BunchConfAuxContainer* aux = new xAOD::BunchConfAuxContainer();
      m_bcc = new xAOD::BunchConfContainer();
      m_bcc->setStore( aux );

      // Record it into the metadata store:
      CHECK( m_metaStore->record( m_bcc, m_metaName ) );
      CHECK( m_metaStore->record( aux, m_metaName + "Aux." ) );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode BunchConfCnvAlg::execute() {

      // Create and record the configuration identifier object for the
      // event data:
      xAOD::BunchConfKey* key = new xAOD::BunchConfKey( m_bcConf->configID() );
      CHECK( evtStore()->record( key, m_eventName ) );

      // Check if this configuration was saved already:
      if( ! m_convertedIDs.insert( m_bcConf->configID() ).second ) {
         ATH_MSG_VERBOSE( "Configuration with key " << m_bcConf->configID()
                          << " already translated" );
         return StatusCode::SUCCESS;
      }

      // Let the user know what's happening:
      ATH_MSG_INFO( "Converting the configuration with key: "
                    << m_bcConf->configID() );

      // Create a new configuration object:
      xAOD::BunchConf* conf = new xAOD::BunchConf();
      m_bcc->push_back( conf );

      // Set its configuration ID:
      conf->setConfigID( m_bcConf->configID() );

      // Set the basic configuration on it:
      conf->setCollidingBCIDs( m_bcConf->configuredBCIDs() );
      conf->setIntensitiesBeam1( m_bcConf->configuredIntensitiesBeam1() );
      conf->setIntensitiesBeam2( m_bcConf->configuredIntensitiesBeam2() );

      // Set the detailed configuration on it:
      conf->setUnpairedBCIDsBeam1( m_bcConf->configuredUnpairedBCIDsBeam1() );
      conf->setUnpairedBCIDsBeam2( m_bcConf->configuredUnpairedBCIDsBeam2() );
      conf->setUnpairedIntensitiesBeam1(
               m_bcConf->configuredUnpairedIntensitiesBeam1() );
      conf->setUnpairedIntensitiesBeam2(
               m_bcConf->configuredUnpairedIntensitiesBeam2() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
