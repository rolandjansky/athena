/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODConfigSvc.cxx 674242 2015-06-11 08:22:06Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/GaudiException.h"
#include "AthenaKernel/errorcheck.h"

// Trigger include(s):
#include "TrigConfL1Data/BunchGroup.h"
#include "TrigConfHLTData/HLTSequenceList.h"

// Local include(s):
#include "xAODConfigSvc.h"
#include "TrigConfxAOD/tools/prepareTriggerMenu.h"
#include "TrigConfxAOD/tools/xAODKeysMatch.h"

namespace TrigConf {

   xAODConfigSvc::xAODConfigSvc( const std::string& name, ISvcLocator* svcLoc )
      : AthService( name, svcLoc ),
        m_stopOnFailure( true ), m_isInFailure( false ),
        m_tmcAux( nullptr ), m_tmc( nullptr ), m_menu(),
        m_ctpConfig(), m_chainList(), m_sequenceList(), m_bgSet(),
        m_metaStore( "InputMetaDataStore", name ) {

   }

   StatusCode xAODConfigSvc::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );

      // Retrieve the needed service(s):
      CHECK( m_metaStore.retrieve() );

      // Register read handle key
      CHECK( m_eventKey.initialize() );

      // Set up the callbacks for the service:
      ServiceHandle< IIncidentSvc > incSvc( "IncidentSvc", name() );
      CHECK( incSvc.retrieve() );
      incSvc->addListener( this, IncidentType::BeginEvent, 0,
                           m_stopOnFailure );
      incSvc->addListener( this, IncidentType::BeginInputFile, 0,
                           m_stopOnFailure );

      m_tmcAux = std::make_unique<xAOD::TriggerMenuAuxContainer>();
      m_tmc    = std::make_unique<xAOD::TriggerMenuContainer>();
      m_tmc->setStore( m_tmcAux.get() );

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

      // Return the slot-specific pointer:
      return m_ctpConfig.get();
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

      // Return the slot-specific pointer:
      return m_bgSet.get();
   }

   uint32_t xAODConfigSvc::lvl1PrescaleKey() const {

      // Check that we know the configuration already:
      if( ! m_menu.get()->m_ptr ) {
         REPORT_MESSAGE( MSG::ERROR )
            << "Trigger menu not yet known. Configuration key not returned.";
         throw GaudiException( "Service not initialised correctly",
                               "TrigConf::xAODConfigSvc::lvl1PrescaleKey",
                               StatusCode::FAILURE );
         return 0;
      }

      // Return the key from the slot-specific metadata object:
      return m_menu.get()->m_ptr->l1psk();
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

      // Return the slot-specific pointer:
      return m_chainList.get();
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

      // Return the slot-specifc object:
      return *(m_chainList.get());
   }

   const HLTSequenceList* xAODConfigSvc::sequenceList() const {

      // Check if the object is well prepared:
      if( m_isInFailure ) {
         REPORT_MESSAGE( MSG::FATAL )
            << "Trigger menu not loaded";
         throw GaudiException( "Service not initialised correctly",
                               "TrigConf::xAODConfigSvc::chainList",
                               StatusCode::FAILURE );
      }

      // Return the slot-specific pointer:
      return m_sequenceList.get();
   }

   const HLTSequenceList& xAODConfigSvc::sequences() const {

      // Check if the object is well prepared:
      if( m_isInFailure ) {
         REPORT_MESSAGE( MSG::FATAL )
            << "Trigger menu not loaded";
         throw GaudiException( "Service not initialised correctly",
                               "TrigConf::xAODConfigSvc::chains",
                               StatusCode::FAILURE );
      }

      // Return the slot-specific object:
      return *(m_sequenceList.get());
   }

   uint32_t xAODConfigSvc::masterKey() const {

      // Check that we know the configuration already:
      if( ! m_menu.get()->m_ptr ) {
         REPORT_MESSAGE( MSG::FATAL )
            << "Trigger menu not yet known. Configuration key not returned.";
         throw GaudiException( "Service not initialised correctly",
                               "TrigConf::xAODConfigSvc::masterKey",
                               StatusCode::FAILURE );
         return 0;
      }

      // Return the key from the slot-specific metadata object:
      return m_menu.get()->m_ptr->smk();
   }

   uint32_t xAODConfigSvc::hltPrescaleKey() const {

      // Check that we know the configuration already:
      if( ! m_menu.get()->m_ptr ) {
         REPORT_MESSAGE( MSG::FATAL )
            << "Trigger menu not yet known. Configuration key not returned.";
         throw GaudiException( "Service not initialised correctly",
                               "TrigConf::xAODConfigSvc::hltPrescaleKey",
                               StatusCode::FAILURE );
         return 0;
      }

      // Return the key from the slot-specific metadata object:
      return m_menu.get()->m_ptr->hltpsk();
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
      REPORT_MESSAGE( MSG::DEBUG ) << "Callback received with incident: "
                     << inc.type() << endmsg;

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
         REPORT_MESSAGE( MSG::WARNING ) << "Unknown incident type received: "
                          << inc.type() << endmsg;
         return;
      }

      // Return gracefully:
      return;
   }

   StatusCode xAODConfigSvc::readMetadata() {

      // Read the metadata object...
      const xAOD::TriggerMenuContainer* input = nullptr;
      if( m_metaStore->retrieve( input, m_metaName ).isFailure() ) {
         // Update the internal flag:
         m_isInFailure = true;
         // Decide what to do:
         if( m_stopOnFailure ) {
            REPORT_MESSAGE( MSG::FATAL )
               << "Couldn't retrieve xAOD::TriggerMenuContainer";
            return StatusCode::FAILURE;
         } else {
            REPORT_MESSAGE( MSG::INFO ) << "Couldn't retrieve xAOD::TriggerMenuContainer" << endmsg;
            return StatusCode::SUCCESS;
         }
      }

      // Let the user know what happened:
      REPORT_MESSAGE( MSG::DEBUG ) << "Loaded trigger configuration metadata container"  << endmsg;

      // A little sanity check:
      if( ! input->size() ) {
         REPORT_MESSAGE( MSG::WARNING ) << "No trigger configurations are available on the input" << endmsg;
         return StatusCode::SUCCESS;
      }

      // Only one thread at a time is allowed to extend m_tmc, 
      // and this should only happen when it is not being iterated over
      // as part of the BeginEvent indicent.
      std::unique_lock lockUnique(m_sharedMutex);

      // Copy in new menus
      for ( const xAOD::TriggerMenu* inputMenu : *input ) {
         bool alreadyHave = false;
         for( const xAOD::TriggerMenu* existingMenu : *m_tmc ) {
            if (xAODKeysMatch(inputMenu, existingMenu)) {
               alreadyHave = true;
               break;
            }
         }
         if (alreadyHave) {
            continue;
         }
         // Copy this menu into the service's internal cache of all menus
         xAOD::TriggerMenu* newMenu = new xAOD::TriggerMenu();
         m_tmc->push_back( newMenu ); // Note: 'newMenu' is now memory managed by m_tmc
         *newMenu = *inputMenu;
         REPORT_MESSAGE( MSG::DEBUG ) << "Imported new configuration: SMK = " << newMenu->smk()
            << ", L1PSK = " << newMenu->l1psk()
            << ", HLTPSK = " << newMenu->hltpsk() << endmsg;
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode xAODConfigSvc::prepareEvent() {

      // Can the incident service provide this to us?
      const EventContext context = Gaudi::Hive::currentContext();

      // Read the current event's trigger keys:
      SG::ReadHandle<xAOD::TrigConfKeys> keys(m_eventKey, context);
      if( !keys.isValid() ) {
         // Update the internal flag:
         m_isInFailure = true;
         // Decide what to do:
         if( m_stopOnFailure ) {
            REPORT_MESSAGE( MSG::FATAL )
               << "Coudln't retrieve xAOD::TrigConfKeys";
            return StatusCode::FAILURE;
         } else {
            REPORT_MESSAGE( MSG::DEBUG ) << "Coudln't retrieve xAOD::TrigConfKeys" << endmsg;
            return StatusCode::SUCCESS;
         }
      }

      const xAOD::TriggerMenu* loadedMenuInSlot = m_menu.get(context)->m_ptr;

      // Check if we have the correct menu already:
      if( loadedMenuInSlot != nullptr && xAODKeysMatch( keys.cptr(), loadedMenuInSlot ) ) {
         return StatusCode::SUCCESS;
      }

      // If not, let's look for the correct configuration:
      // Open a shared lock. OK for multiple events to search at the same time,
      // but prevent the extension of m_tmc from a BeginInputFile incident.  
      std::shared_lock lockShared(m_sharedMutex);

      xAOD::TriggerMenuContainer::const_iterator menu_itr = m_tmc->begin();
      xAOD::TriggerMenuContainer::const_iterator menu_end = m_tmc->end();
      for( ; menu_itr != menu_end; ++menu_itr ) {
         // Check if this is the menu we're looking for:
         if( ! xAODKeysMatch( keys.cptr(), *menu_itr ) ) continue;
         // Remember it's pointer:
         loadedMenuInSlot = *menu_itr;
         m_menu.get(context)->m_ptr = loadedMenuInSlot;
         // Cache the menu's configuration.
         CTPConfig& ctpConfig = *(m_ctpConfig.get(context));
         HLTChainList& chainList = *(m_chainList.get(context));
         HLTSequenceList& sequenceList = *(m_sequenceList.get(context));
         BunchGroupSet& bgSet = *(m_bgSet.get(context));
         CHECK( prepareTriggerMenu( loadedMenuInSlot, ctpConfig, 
                                    chainList, sequenceList,
                                    bgSet, msg() ) );
         REPORT_MESSAGE( MSG::DEBUG ) << "ctpConfig.menu().size() = " << ctpConfig.menu().size()
            << " chainList.size() = " << chainList.size()
            << " sequenceList.size() = " << sequenceList.size()
            << " bgSet.bunchGroups().size() = " << bgSet.bunchGroups().size() << endmsg;
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

} // namespace TrigConf
