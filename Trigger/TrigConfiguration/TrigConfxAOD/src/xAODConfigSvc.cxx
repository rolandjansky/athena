/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODConfigSvc.cxx 583271 2014-02-14 12:44:39Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/GaudiException.h"
#include "AthenaKernel/errorcheck.h"

// Trigger include(s):
#include "TrigConfL1Data/BunchGroup.h"
#include "TrigConfHLTData/HLTSequenceList.h"

// xAOD include(s):
#include "xAODTrigger/TrigConfKeys.h"

// Local include(s):
#include "xAODConfigSvc.h"

namespace {

   /// Helper function for finding configurations with given keys
   bool keysMatch( const xAOD::TrigConfKeys* keys,
                   const xAOD::TriggerMenu* menu ) {

      return ( ( keys->smk() == menu->smk() ) &&
               ( keys->l1psk() == menu->l1psk() ) &&
               ( keys->hltpsk() == menu->hltpsk() ) );
   }

} // private namespace

namespace TrigConf {

   xAODConfigSvc::xAODConfigSvc( const std::string& name, ISvcLocator* svcLoc )
      : AthService( name, svcLoc ),
        m_stopOnFailure( true ), m_isInFailure( false ),
        m_tmc( 0 ), m_menu( 0 ),
        m_ctpConfig(), m_chainList(),
        m_eventStore( "StoreGateSvc", name ),
        m_metaStore( "InputMetaDataStore", name ) {

      declareProperty( "StopOnFailure", m_stopOnFailure = true );

      declareProperty( "EventObjectName", m_eventName = "TrigConfKeys" );
      declareProperty( "MetaObjectName", m_metaName = "TriggerMenu" );

      declareProperty( "MetaDataStore", m_metaStore );
   }

   StatusCode xAODConfigSvc::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );

      // Retrieve the needed service(s):
      CHECK( m_eventStore.retrieve() );
      CHECK( m_metaStore.retrieve() );

      // Set up the callbacks for the service:
      ServiceHandle< IIncidentSvc > incSvc( "IncidentSvc", name() );
      CHECK( incSvc.retrieve() );
      incSvc->addListener( this, IncidentType::BeginEvent, 0,
                           m_stopOnFailure );
      incSvc->addListener( this, IncidentType::BeginInputFile, 0,
                           m_stopOnFailure );

      // Reset the internal flag:
      m_isInFailure = false;

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode xAODConfigSvc::finalize() {

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   const CTPConfig* xAODConfigSvc::ctpConfig() const {

      // Check if the object is well prepared:
      if( m_isInFailure ) {
         REPORT_MESSAGE( MSG::FATAL )
            << "Trigger menu not loaded";
         throw GaudiException( "Service not initialised correctly",
                               "TrigConf::xAODConfigSvc::ctpConfig",
                               StatusCode::FAILURE );
      }

      // Return the pointer:
      return &m_ctpConfig;
   }

   const BunchGroupSet* xAODConfigSvc::bunchGroupSet() const {

      // Check if the object is well prepared:
      if( m_isInFailure ) {
         REPORT_MESSAGE( MSG::FATAL )
            << "Trigger menu not loaded";
         throw GaudiException( "Service not initialised correctly",
                               "TrigConf::xAODConfigSvc::bunchGroupSet",
                               StatusCode::FAILURE );
      }

      // Return the pointer:
      return &m_bgSet;
   }

   uint32_t xAODConfigSvc::lvl1PrescaleKey() const {

      // Check that we know the configuration already:
      if( ! m_menu ) {
         REPORT_MESSAGE( MSG::ERROR )
            << "Trigger menu not yet known. Configuration key not returned.";
         throw GaudiException( "Service not initialised correctly",
                               "TrigConf::xAODConfigSvc::lvl1PrescaleKey",
                               StatusCode::FAILURE );
         return 0;
      }

      // Return the key from the metadata object:
      return m_menu->l1psk();
   }

   const HLTChainList* xAODConfigSvc::chainList() const {

      // Check if the object is well prepared:
      if( m_isInFailure ) {
         REPORT_MESSAGE( MSG::FATAL )
            << "Trigger menu not loaded";
         throw GaudiException( "Service not initialised correctly",
                               "TrigConf::xAODConfigSvc::chainList",
                               StatusCode::FAILURE );
      }

      // Return the pointer:
      return &m_chainList;
   }

   const HLTChainList& xAODConfigSvc::chains() const {

      // Check if the object is well prepared:
      if( m_isInFailure ) {
         REPORT_MESSAGE( MSG::FATAL )
            << "Trigger menu not loaded";
         throw GaudiException( "Service not initialised correctly",
                               "TrigConf::xAODConfigSvc::chains",
                               StatusCode::FAILURE );
      }

      // Return the object:
      return m_chainList;
   }

   const HLTSequenceList& xAODConfigSvc::sequences() const {

      // Let the user know that something bad is happening:
      ATH_MSG_WARNING( "HLT sequences are not available from xAOD input" );

      // Create a dummy object:
      static const HLTSequenceList dummy;
      // And return this one...
      return dummy;
   }

   uint32_t xAODConfigSvc::masterKey() const {

      // Check that we know the configuration already:
      if( ! m_menu ) {
         REPORT_MESSAGE( MSG::FATAL )
            << "Trigger menu not yet known. Configuration key not returned.";
         throw GaudiException( "Service not initialised correctly",
                               "TrigConf::xAODConfigSvc::masterKey",
                               StatusCode::FAILURE );
         return 0;
      }

      // Return the key from the metadata object:
      return m_menu->smk();
   }

   uint32_t xAODConfigSvc::hltPrescaleKey() const {

      // Check that we know the configuration already:
      if( ! m_menu ) {
         REPORT_MESSAGE( MSG::FATAL )
            << "Trigger menu not yet known. Configuration key not returned.";
         throw GaudiException( "Service not initialised correctly",
                               "TrigConf::xAODConfigSvc::hltPrescaleKey",
                               StatusCode::FAILURE );
         return 0;
      }

      // Return the key from the metadata object:
      return m_menu->hltpsk();
   }

   StatusCode xAODConfigSvc::queryInterface( const InterfaceID& riid,
                                             void** ppvIf ) {

      // Check if the user asked for the ITrigConfigSvc interface:
      if( riid == ITrigConfigSvc::interfaceID() ) {
         *ppvIf = static_cast< ITrigConfigSvc* >( this );
         addRef();
         return StatusCode::SUCCESS;
      }
      // Check if it's the ILVL1ConfigSvc interface we need:
      else if( riid == ILVL1ConfigSvc::interfaceID() ) {
         *ppvIf = static_cast< ILVL1ConfigSvc* >( this );
         addRef();
         return StatusCode::SUCCESS;
      }
      // Check if it's the IHLTConfigSvc interface we need:
      else if( riid == IHLTConfigSvc::interfaceID() ) {
         *ppvIf = static_cast< IHLTConfigSvc* >( this );
         addRef();
         return StatusCode::SUCCESS;
      }
      // Check if it's the IL1TopoConfigSvc interface we need:
      else if( riid == IL1TopoConfigSvc::interfaceID() ) {
         *ppvIf = static_cast< IL1TopoConfigSvc* >( this );
         addRef();
         return StatusCode::SUCCESS;
      }
      // Finally check if the user asked for the IIncidentListener interface:
      else if( riid == IIncidentListener::interfaceID() ) {
         *ppvIf = static_cast< IIncidentListener* >( this );
         addRef();
         return StatusCode::SUCCESS;
      }

      // Leave it to the base class to deal with the rest:
      return AthService::queryInterface( riid, ppvIf );
   }

   void xAODConfigSvc::handle( const Incident& inc ) {

      // Tell the user what we're doing:
      ATH_MSG_DEBUG( "Callback received with incident: "
                     << inc.type() );

      // If it's a file-beginning incident, let's read in the
      // metadata object:
      if( inc.type() == IncidentType::BeginInputFile ) {
         if( readMetadata().isFailure() && m_stopOnFailure ) {
            REPORT_MESSAGE( MSG::FATAL )
               << "Couldn't read in the input file's metadata";
            throw GaudiException( "Couldn't read in the input file's "
                                  "metadata",
                                  "TrigConf::xAODConfigSvc::handle",
                                  StatusCode::FAILURE );
            return;
         }
      }
      // If it's an event-beginning incident, let's make sure that the
      // correct menu is cached:
      else if( inc.type() == IncidentType::BeginEvent ) {
         if( prepareEvent().isFailure() && m_stopOnFailure ) {
            REPORT_MESSAGE( MSG::FATAL )
               << "Couldn't prepare the trigger configuration for the "
               << "current event";
            throw GaudiException( "Couldn't prepare the trigger configuration "
                                  "for the current event",
                                  "TrigConf::xAODConfigSvc::handle",
                                  StatusCode::FAILURE );
            return;
         }
      }
      // We got some strange incident...
      else {
         ATH_MSG_WARNING( "Unknown incident type received: "
                          << inc.type() );
         return;
      }

      // Return gracefully:
      return;
   }

   StatusCode xAODConfigSvc::readMetadata() {

      // Read the metadata object...
      m_tmc = 0; m_menu = 0;
      if( m_metaStore->retrieve( m_tmc, m_metaName ).isFailure() ) {
         // Update the internal flag:
         m_isInFailure = true;
         // Decide what to do:
         if( m_stopOnFailure ) {
            REPORT_MESSAGE( MSG::FATAL )
               << "Couldn't retrieve xAOD::TriggerMenuContainer";
            return StatusCode::FAILURE;
         } else {
            ATH_MSG_INFO( "Couldn't retrieve xAOD::TriggerMenuContainer" );
            return StatusCode::SUCCESS;
         }
      }

      // Let the user know what happened:
      ATH_MSG_INFO( "Loaded trigger configuration metadata container" );

      // Point the menu pointer to the first element by default:
      m_menu = m_tmc->at( 0 );
      // Cache the menu's configuration:
      CHECK( prepareMenu( m_menu ) );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode xAODConfigSvc::prepareEvent() {

      // Read the current event's trigger keys:
      const xAOD::TrigConfKeys* keys = 0;
      if( m_eventStore->retrieve( keys, m_eventName ).isFailure() ) {
         // Update the internal flag:
         m_isInFailure = true;
         // Decide what to do:
         if( m_stopOnFailure ) {
            REPORT_MESSAGE( MSG::FATAL )
               << "Coudln't retrieve xAOD::TrigConfKeys";
            return StatusCode::FAILURE;
         } else {
            ATH_MSG_DEBUG( "Coudln't retrieve xAOD::TrigConfKeys" );
            return StatusCode::SUCCESS;
         }
      }

      // Check if we have the correct menu already:
      if( keysMatch( keys, m_menu ) ) {
         return StatusCode::SUCCESS;
      }

      // If not, let's look for the correct configuration:
      xAOD::TriggerMenuContainer::const_iterator menu_itr = m_tmc->begin();
      xAOD::TriggerMenuContainer::const_iterator menu_end = m_tmc->end();
      for( ; menu_itr != menu_end; ++menu_itr ) {
         // Check if this is the menu we're looking for:
         if( ! keysMatch( keys, *menu_itr ) ) continue;
         // Remember it's pointer:
         m_menu = *menu_itr;
         // Cache the menu's configuration:
         CHECK( prepareMenu( m_menu ) );
         // We're done:
         return StatusCode::SUCCESS;
      }

      // Apparently we didn't find the correct menu!
      REPORT_MESSAGE( MSG::FATAL )
         << "Couldn't find configuration for current event (SMK:"
         << keys->smk() << ", L1PSK:" << keys->l1psk()
         << ", HLTPSK:" << keys->hltpsk() << ")";
      return StatusCode::FAILURE;
   }

   StatusCode xAODConfigSvc::prepareMenu( const xAOD::TriggerMenu* menu ) {

      // Clear the current LVL1 configuration:
      m_ctpConfig.menu().clear();
      m_ctpConfig.clearPrescaleSets();

      // Fill the LVL1 configuration:
      for( size_t i = 0; i < menu->itemCtpIds().size(); ++i ) {
         TriggerItem* item = new TriggerItem();
         item->setName( menu->itemNames()[ i ] );
         item->setCtpId( menu->itemCtpIds()[ i ] );
         m_ctpConfig.menu().addTriggerItem( item );
         if( menu->itemPrescalesAvailable() ) {
            m_ctpConfig.prescaleSet().setPrescale( menu->itemCtpIds()[ i ],
               static_cast< int >( menu->itemPrescales()[ i ] ) );
         }
      }

      // Clear the current HLT configuration:
      m_chainList.clear();

      // Fill the HLT configuration:
      for( size_t i = 0; i < menu->chainIds().size(); ++i ) {

         // Figure out which level this chain is from:
         std::string level = "";
         if( menu->chainNames()[ i ].find( "L2_" ) == 0 ) {
            level = "L2";
         } else if( menu->chainNames()[ i ].find( "EF_" ) == 0 ) {
            level = "EF";
         } else if( menu->chainNames()[ i ].find( "HLT_" ) == 0 ) {
            level = "HLT";
         } else {
            ATH_MSG_WARNING( "Couldn't figure out 'level' for chain: "
                             << menu->chainNames()[ i ] );
         }
         // An empty signature list for the chain:
         const std::vector< HLTSignature* > dummySignatures;
         // Create the chain object:
         HLTChain* chain = new HLTChain( menu->chainNames()[ i ],
                                         menu->chainIds()[ i ],
                                         1, // Chain version not important
                                         level,
                                         menu->chainParentNames()[ i ],
                                         -1, // Lower chain ID not important
                                         dummySignatures );
         if( menu->chainRerunPrescalesAvailable() ) {
            chain->set_rerun_prescale( menu->chainRerunPrescales()[ i ] );
         }
         if( menu->chainPassthroughPrescalesAvailable() ) {
            chain->set_pass_through( menu->chainPassthroughPrescales()[ i ] );
         }
         // Add it to the list of chains:
         if( ! m_chainList.addHLTChain( chain ) ) {
            REPORT_MESSAGE( MSG::FATAL )
               << "Couldn't add chain \"" << chain->name()
               << "\"";
            delete chain;
            return StatusCode::FAILURE;
         }
      }

      // Check if bunch-groups are available:
      if( menu->bunchGroupBunchesAvailable() ) {

         // Create a new BunchGroupSet object, since an existing one can't be
         // modified... :-/
         BunchGroupSet bgSet;

         // Fill it with info:
         for( size_t i = 0; i < menu->bunchGroupBunches().size(); ++i ) {

            // Create a BunchGroup object:
            BunchGroup bg;
            bg.setInternalNumber( i );
            std::vector< uint16_t >::const_iterator b_itr =
               menu->bunchGroupBunches()[ i ].begin();
            std::vector< uint16_t >::const_iterator b_end =
               menu->bunchGroupBunches()[ i ].end();
            for( ; b_itr != b_end; ++b_itr ) {
               bg.addBunch( *b_itr );
            }

            // Add it to the set:
            bgSet.addBunchGroup( bg );
         }

         // Replace the current bunch-group set with the new one:
         m_bgSet = bgSet;

      } else {
         ATH_MSG_WARNING( "Bunch-group information not available on the "
                          "input" );
      }

      // Let the user know what happened:
      ATH_MSG_INFO( "Loaded configuration:" );
      ATH_MSG_INFO( "  SMK = " << m_menu->smk()
                    << ", L1PSK = " << m_menu->l1psk()
                    << ", HLTPSK = " << m_menu->hltpsk() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace TrigConf
