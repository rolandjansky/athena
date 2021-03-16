/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

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

// Boost includes
#define BOOST_BIND_GLOBAL_PLACEHOLDERS // Needed to silence Boost pragma message
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace TrigConf {

   xAODConfigSvc::xAODConfigSvc( const std::string& name, ISvcLocator* svcLoc )
      : AthService( name, svcLoc ),
        m_stopOnFailure( true ), m_isInFailure( false ),
        m_tmcAux( nullptr ), m_tmc( nullptr ), m_menu(),
        m_ctpConfig(), m_chainList(), m_sequenceList(), m_bgSet(),
        m_metaStore( "InputMetaDataStore", name ),
        m_triggerMenuContainerAvailable( false ),
        m_menuJSONContainerAvailable( false ) {

   }

   StatusCode xAODConfigSvc::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );

      // Register read handle key
      CHECK( m_eventKey.initialize() );

      // Set up the callbacks for the service:
      ServiceHandle< IIncidentSvc > incSvc( "IncidentSvc", name() );
      CHECK( incSvc.retrieve() );
      incSvc->addListener( this, IncidentType::BeginEvent, 0,
                           m_stopOnFailure );

      if (m_useInFileMetadata) {
        // Retrieve the needed service(s):
        CHECK( m_metaStore.retrieve() );

        // Set up the callbacks for the service:
        ServiceHandle< IIncidentSvc > incSvc( "IncidentSvc", name() );
        incSvc->addListener( this, IncidentType::BeginInputFile, 0,
                             m_stopOnFailure );

        // Internal holder for R2 payloads
        m_tmcAux = std::make_unique<xAOD::TriggerMenuAuxContainer>();
        m_tmc    = std::make_unique<xAOD::TriggerMenuContainer>();
        m_tmc->setStore( m_tmcAux.get() );

        // Internal holders for R3 payloads
        m_hltJsonAux = std::make_unique<xAOD::TriggerMenuJsonAuxContainer>();
        m_hltJson    = std::make_unique<xAOD::TriggerMenuJsonContainer>();
        m_hltJson->setStore( m_hltJsonAux.get() );

        m_l1JsonAux = std::make_unique<xAOD::TriggerMenuJsonAuxContainer>();
        m_l1Json    = std::make_unique<xAOD::TriggerMenuJsonContainer>();
        m_l1Json->setStore( m_l1JsonAux.get() );

        m_hltpsJsonAux = std::make_unique<xAOD::TriggerMenuJsonAuxContainer>();
        m_hltpsJson    = std::make_unique<xAOD::TriggerMenuJsonContainer>();
        m_hltpsJson->setStore( m_hltpsJsonAux.get() );

        m_l1psJsonAux = std::make_unique<xAOD::TriggerMenuJsonAuxContainer>();
        m_l1psJson    = std::make_unique<xAOD::TriggerMenuJsonContainer>();
        m_l1psJson->setStore( m_l1psJsonAux.get() );

        m_bgJsonAux = std::make_unique<xAOD::TriggerMenuJsonAuxContainer>();
        m_bgJson    = std::make_unique<xAOD::TriggerMenuJsonContainer>();
        m_bgJson->setStore( m_bgJsonAux.get() );
      }

      ATH_CHECK( m_HLTPrescaleSetKey.initialize( !m_useInFileMetadata ) );
      ATH_CHECK( m_L1PrescaleSetKey.initialize( !m_useInFileMetadata ) );

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

      // Long-term-support interface for (including Run 2 AODs)
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

      // Long-term-support interface (including Run 2 AODs)
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


   const HLTChainList& xAODConfigSvc::chains() const {

      // Long-term-support interface (including Run 2 AODs)
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

   const HLTSequenceList& xAODConfigSvc::sequences() const {

      // Long-term-support interface (including Run 2 AODs)
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
      if (m_menuJSONContainerAvailable or !m_useInFileMetadata) {

         // Run3: From in-file JSON metadata or JSON from conditions store
         return m_currentHlt.get()->smk();

      } else {

         // Legacy support for R2 AODs
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
   }

   uint32_t xAODConfigSvc::lvl1PrescaleKey() const {
      if (m_menuJSONContainerAvailable or !m_useInFileMetadata) {
         
         // Run3: From in-file JSON metadata or JSON from conditions store
         return m_currentL1ps.get()->psk();

      } else {

         // Legacy support for R2 AODs
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
   }

   uint32_t xAODConfigSvc::hltPrescaleKey() const {
      if (m_menuJSONContainerAvailable or !m_useInFileMetadata) {

         // Run3: From in-file JSON metadata or JSON from conditions store
         return m_currentHltps.get()->psk();

      } else {

         // Legacy support for R2 AODs
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
   }

   const HLTMenu& xAODConfigSvc::hltMenu(const EventContext& ctx) const {
      if (m_useInFileMetadata and !m_menuJSONContainerAvailable) {
         REPORT_MESSAGE( MSG::FATAL ) << "Run 3 hltMenu JSON not loaded." << endmsg;
         throw GaudiException( "Service not initialised correctly",
                               "TrigConf::xAODConfigSvc::hltMenu",
                               StatusCode::FAILURE );
      }
      // Run3: From in-file JSON metadata or JSON from detector store
      return *(m_currentHlt.get(ctx));
   }

   const L1Menu& xAODConfigSvc::l1Menu(const EventContext& ctx) const {
      if (m_useInFileMetadata and !m_menuJSONContainerAvailable) {
         REPORT_MESSAGE( MSG::FATAL ) << "Run 3 l1Menu JSON not loaded." << endmsg;
         throw GaudiException( "Service not initialised correctly",
                               "TrigConf::xAODConfigSvc::l1Menu",
                               StatusCode::FAILURE );
      }
      // Run3: From in-file JSON metadata or JSON from detector store
      return *(m_currentL1.get(ctx));
   }

   const HLTPrescalesSet& xAODConfigSvc::hltPrescalesSet(const EventContext& ctx) const {
      if (m_useInFileMetadata and !m_menuJSONContainerAvailable) {
         REPORT_MESSAGE( MSG::FATAL ) << "Run 3 hltPrescalesSet JSON not loaded." << endmsg;
         throw GaudiException( "Service not initialised correctly",
                               "TrigConf::xAODConfigSvc::hltPrescalesSet",
                               StatusCode::FAILURE );
      }
      // Run3: From in-file JSON metadata or JSON from conditions store
      return *(m_currentHltps.get(ctx));
   }

   const L1PrescalesSet& xAODConfigSvc::l1PrescalesSet(const EventContext& ctx) const {
      if (m_useInFileMetadata and !m_menuJSONContainerAvailable) {
         REPORT_MESSAGE( MSG::FATAL ) << "Run 3 l1PrescalesSet JSON not loaded." << endmsg;
         throw GaudiException( "Service not initialised correctly",
                               "TrigConf::xAODConfigSvc::l1PrescalesSet",
                               StatusCode::FAILURE );
      }
      // Run3: From in-file JSON metadata or JSON from conditions store
      return *(m_currentL1ps.get(ctx));
   }

   const L1BunchGroupSet& xAODConfigSvc::l1BunchGroupSet(const EventContext& ctx) const {
      if (m_useInFileMetadata and !m_menuJSONContainerAvailable) {
         REPORT_MESSAGE( MSG::FATAL ) << "Run 3 l1BunchGroupSet JSON not loaded." << endmsg;
         throw GaudiException( "Service not initialised correctly",
                               "TrigConf::xAODConfigSvc::l1BunchGroupSet",
                               StatusCode::FAILURE );
      }
      // Run3: From in-file JSON metadata or JSON from conditions store
      return *(m_currentBg.get(ctx));
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

      // Only one thread at a time is allowed to process a new file being opened
      // and this should only happen when it is not being iterated over
      // as part of the BeginEvent incident.
      std::unique_lock lockUnique(m_sharedMutex);

      m_triggerMenuContainerAvailable = true;
      m_menuJSONContainerAvailable = true;

      // Read the R2 metadata object...
      const xAOD::TriggerMenuContainer* input_tmc = nullptr;
      if( !m_metaStore->contains<xAOD::TriggerMenuContainer>(m_metaName)
          or m_metaStore->retrieve( input_tmc, m_metaName ).isFailure() )
      {
         m_triggerMenuContainerAvailable = false;
      }

      // Read the R3 metadata object...
      const xAOD::TriggerMenuJsonContainer* input_hlt = nullptr;
      const xAOD::TriggerMenuJsonContainer* input_l1 = nullptr;
      const xAOD::TriggerMenuJsonContainer* input_hltps = nullptr;
      const xAOD::TriggerMenuJsonContainer* input_l1ps = nullptr;
      // const xAOD::TriggerMenuJsonContainer* input_bg = nullptr;
      if( !m_metaStore->contains<xAOD::TriggerMenuJsonContainer>(m_metaNameJSON_hlt)
          or m_metaStore->retrieve( input_hlt, m_metaNameJSON_hlt ).isFailure() )
      {
         m_menuJSONContainerAvailable = false;
      }
      if( !m_metaStore->contains<xAOD::TriggerMenuJsonContainer>(m_metaNameJSON_l1)
          or m_metaStore->retrieve( input_l1, m_metaNameJSON_l1 ).isFailure() )
      {
         m_menuJSONContainerAvailable = false;
      }
      if( !m_metaStore->contains<xAOD::TriggerMenuJsonContainer>(m_metaNameJSON_hltps)
          or m_metaStore->retrieve( input_hltps, m_metaNameJSON_hltps ).isFailure() )
      {
         m_menuJSONContainerAvailable = false;
      }
      if( !m_metaStore->contains<xAOD::TriggerMenuJsonContainer>(m_metaNameJSON_l1ps)
          or m_metaStore->retrieve( input_l1ps, m_metaNameJSON_l1ps ).isFailure() )
      {
         m_menuJSONContainerAvailable = false;
      }
      // if( !m_metaStore->contains<xAOD::TriggerMenuJsonContainer>(m_metaNameJSON_bg)
      //     or m_metaStore->retrieve( input_bg, m_metaNameJSON_bg ).isFailure() )
      // {
      //    m_menuJSONContainerAvailable = false;
      // }

      if (!m_triggerMenuContainerAvailable && !m_menuJSONContainerAvailable) {
         // Update the internal flag:
         m_isInFailure = true;
         // Decide what to do:
         if( m_stopOnFailure ) {
            REPORT_MESSAGE( MSG::FATAL ) << "Couldn't retrieve xAOD::TriggerMenuContainer or xAOD::TriggerMenuJsonContainer(s)" << endmsg;
            return StatusCode::FAILURE;
         } else {
            REPORT_MESSAGE( MSG::WARNING ) << "Couldn't retrieve xAOD::TriggerMenuContainer or xAOD::TriggerMenuJsonContainer(s)" << endmsg;
            return StatusCode::SUCCESS;
         }
      }

      // From file #2 and onwards, check for mixed-messages from the input files.
      // Note from the check just above, we know that we have at least one type of data available on this input file
      //
      // We do this as we currently have just two "available" flags, for the two data formats. Not two per slot
      if (m_tmc->size() > 0 && !m_triggerMenuContainerAvailable && m_menuJSONContainerAvailable) {
         m_isInFailure = true;
         m_triggerMenuContainerAvailable = false;
         m_menuJSONContainerAvailable = false;
         if( m_stopOnFailure ) {
            REPORT_MESSAGE( MSG::FATAL )
               << "In this input file we found xAOD::TriggerMenuJsonContainer(s), but no xAOD::TriggerMenuContainer. "
               << "This is inconsistent with previous input files." << endmsg;
               return StatusCode::FAILURE;
         } else {
            REPORT_MESSAGE( MSG::WARNING )
               << "In this input file we found xAOD::TriggerMenuJsonContainer(s), but no xAOD::TriggerMenuContainer. "
               << "This is inconsistent with previous input files." << endmsg;
            return StatusCode::SUCCESS;
         }
      }
      if (m_hltJson->size() > 0 && !m_menuJSONContainerAvailable && m_triggerMenuContainerAvailable) {
         m_isInFailure = true;
         m_triggerMenuContainerAvailable = false;
         m_menuJSONContainerAvailable = false;
         if( m_stopOnFailure ) {
            REPORT_MESSAGE( MSG::FATAL )
               << "In this input file we found xAOD::TriggerMenuContainer, but no xAOD::TriggerMenuJsonContainer. "
               << "This is inconsistent with previous input files." << endmsg;
               return StatusCode::FAILURE;
         } else {
            REPORT_MESSAGE( MSG::WARNING )
               << "In this input file we found xAOD::TriggerMenuContainer, but no xAOD::TriggerMenuJsonContainer. "
               << "This is inconsistent with previous input files." << endmsg;
            return StatusCode::SUCCESS;
         }
      }

      // Let the user know what happened:
      REPORT_MESSAGE( MSG::DEBUG ) << "Loaded trigger configuration metadata container(s)"  << endmsg;

      // If available, give preference to the R3 format
      if (m_menuJSONContainerAvailable) {

         copyMetadataToPersonalStore(input_hlt, m_hltJson.get());
         copyMetadataToPersonalStore(input_l1, m_l1Json.get());
         copyMetadataToPersonalStore(input_hltps, m_hltpsJson.get());
         copyMetadataToPersonalStore(input_l1ps, m_l1psJson.get());
         // copyMetadataToPersonalStore(input_bg, m_bgJson.get());

         return StatusCode::SUCCESS;

      } else if (m_triggerMenuContainerAvailable) { // Otherwise load the Run 2 format

         // A little sanity check:
         if( ! input_tmc->size() ) {
            REPORT_MESSAGE( MSG::WARNING ) << "No trigger configurations are available on the input" << endmsg;
            return StatusCode::SUCCESS;
         }

         // Copy in new menus
         for ( const xAOD::TriggerMenu* inputMenu : *input_tmc ) {
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

         return StatusCode::SUCCESS;

      } // m_menuJSONContainerAvailable or m_triggerMenuContainerAvailable

      //Should never get here (due to check above)
      ATH_MSG_ERROR( "Both m_menuJSONContainerAvailable and m_triggerMenuContainerAvailable are false" );
      return StatusCode::FAILURE;
   }

   void xAODConfigSvc::copyMetadataToPersonalStore(const xAOD::TriggerMenuJsonContainer* input, xAOD::TriggerMenuJsonContainer* existing) {
      for ( const xAOD::TriggerMenuJson* inputTriggerMenuJson : *input ) {
         bool alreadyHave = false;
         for( const xAOD::TriggerMenuJson* existingTriggerMenuJson : *existing ) {
            if (inputTriggerMenuJson->key() == existingTriggerMenuJson->key()) {
               alreadyHave = true;
               break;
            }
         }
         if (alreadyHave) {
            continue;
         }
         // Copy this menu JSON into the service's internal cache of all menus
         xAOD::TriggerMenuJson* newTriggerMenuJson = new xAOD::TriggerMenuJson();
         existing->push_back( newTriggerMenuJson ); // Note: 'newTriggerMenuJson' is now memory managed by 'existing'
         *newTriggerMenuJson = *inputTriggerMenuJson; // Perform xAOD copy
         REPORT_MESSAGE( MSG::DEBUG ) << "Imported new configuration: Name = " << newTriggerMenuJson->name() << " Key = " << newTriggerMenuJson->key() << endmsg;
      }
   }

   StatusCode xAODConfigSvc::prepareEvent() {

      // Can the incident service provide this to us?
      const EventContext context = Gaudi::Hive::currentContext();

      if (!m_useInFileMetadata) { // Run 3 RAWtoESD, RAWtoALL decoding mode
         return prepareEventRun3Athena(context);
      }

      // Otherwise we're dealing with in-file data
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

      if (m_menuJSONContainerAvailable) { // Run 3 AOD decoding mode
         return prepareEventxAODTriggerMenuJson(keys.cptr(), context);
      } else if (m_triggerMenuContainerAvailable) {  // Run 2 AOD decoding mode
         return prepareEventxAODTriggerMenu(keys.cptr(), context);
      }
      ATH_MSG_ERROR( "Both m_menuJSONContainerAvailable and m_triggerMenuContainerAvailable are false" );
      return StatusCode::FAILURE;
   }


   StatusCode xAODConfigSvc::prepareEventxAODTriggerMenu(const xAOD::TrigConfKeys* keys, const EventContext& context) {
      const xAOD::TriggerMenu* loadedMenuInSlot = m_menu.get(context)->m_ptr;

      // Check if we have the correct menu already:
      if( loadedMenuInSlot != nullptr && xAODKeysMatch( keys, loadedMenuInSlot ) ) {
         REPORT_MESSAGE( MSG::DEBUG )
           << "Configuration matches the loaded one, nothing to do." << endmsg;
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
         if( ! xAODKeysMatch( keys, *menu_itr ) ) continue;
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


   StatusCode xAODConfigSvc::prepareEventRun3Athena(const EventContext& context) {
      HLTMenu&         currentHlt   = *(m_currentHlt.get(context));
      L1Menu&          currentL1    = *(m_currentL1.get(context));
      HLTPrescalesSet& currentHltps = *(m_currentHltps.get(context));
      L1PrescalesSet&  currentL1ps  = *(m_currentL1ps.get(context));
      L1BunchGroupSet& currentBg    = *(m_currentBg.get(context));

      // There is only the beginEvent incident when reading the data from the Detector and Conditions stores.
      // So only one thread may be active at one time on these (slot-specific) objects. Hence we do not need to lock like
      // we do when we also have to deal with the possibility of concurrent beginFile incidents.

      bool validConfig = true;

      // In the beginEvent incident _before_ the first event, we cannot read the prescales (conditions) but we can still
      // read the menu (detector store).
      const bool firstEvent = (!currentHlt.isInitialized() and !currentHltps.isInitialized());

      SG::ReadHandle<HLTMenu> hltMenuHandle(m_hltMenuName);  // No context - Detector Store
      if( !hltMenuHandle.isValid() ) {
          REPORT_MESSAGE( MSG::WARNING )
             << "Unable to load " << m_hltMenuName << " from Detector Store." << endmsg;
      } else {
         if (!currentHlt.isInitialized() or currentHlt.smk() != hltMenuHandle->smk()) {
            validConfig = false;
            currentHlt.clear();
            currentHlt.setData( hltMenuHandle->data() );
            currentHlt.setSMK( hltMenuHandle->smk() );
         }
      }

      SG::ReadHandle<L1Menu> l1MenuHandle(m_l1MenuName);  // No context - Detector Store
      if( !l1MenuHandle.isValid() ) {
          REPORT_MESSAGE( MSG::WARNING )
             << "Unable to load " << m_l1MenuName << " from Detector Store." << endmsg;
      } else {
         if (!currentL1.isInitialized() or currentL1.smk() != l1MenuHandle->smk()) {
            validConfig = false;
            currentL1.clear();
            currentL1.setData( l1MenuHandle->data() );
            currentL1.setSMK( l1MenuHandle->smk() );
         }
      }

      if (firstEvent) {

          REPORT_MESSAGE( MSG::WARNING )
             << "L1 and HLT prescales will not be available via the TrigConf::xAODConfigSvc in the first "
             << "event when running with UseInFileMetadata=False" << endmsg;

      } else {

         SG::ReadCondHandle<L1PrescalesSet> l1psRCH(m_L1PrescaleSetKey, context);
         if( !l1psRCH.isValid() ) {
             REPORT_MESSAGE( MSG::WARNING )
                << "Unable to load " << m_L1PrescaleSetKey.key() << " from Conditions Store." << endmsg;
         } else {
            if (!currentL1ps.isInitialized() or currentL1ps.psk() != l1psRCH->psk()) {
               validConfig = false;
               currentL1ps.clear();
               currentL1ps.setData( l1psRCH->data() );
               currentL1ps.setPSK( l1psRCH->psk() );
            }
         }
   
         SG::ReadCondHandle<HLTPrescalesSet> hltpsRCH(m_HLTPrescaleSetKey, context);
         if( !hltpsRCH.isValid() ) {
             REPORT_MESSAGE( MSG::WARNING )
                << "Unable to load " << m_HLTPrescaleSetKey.key() << " from Conditions Store." << endmsg;
         } else {
            if (!currentHltps.isInitialized() or currentHltps.psk() != hltpsRCH->psk()) {
               validConfig = false;
               currentHltps.clear();
               currentHltps.setData( hltpsRCH->data() );
               currentHltps.setPSK( hltpsRCH->psk() );
            }        
         }
   
         // if (BunchGroup TODO) {
         //    validConfig = false;
         // }
      }

      if (validConfig) {
         REPORT_MESSAGE( MSG::DEBUG )
           << "Configuration matches the loaded one, nothing to do." << endmsg;
         return StatusCode::SUCCESS;
      }

      CTPConfig& ctpConfig = *(m_ctpConfig.get(context));
      HLTChainList& chainList = *(m_chainList.get(context));
      HLTSequenceList& sequenceList = *(m_sequenceList.get(context));
      BunchGroupSet& bgSet = *(m_bgSet.get(context));

      // Copy data into legacy long-term-support interfaces
      CHECK( prepareTriggerMenu( currentHlt,
                                 currentL1,
                                 currentHltps,
                                 currentL1ps,
                                 currentBg,
                                 ctpConfig,
                                 chainList,
                                 sequenceList,
                                 bgSet,
                                 msg() ) );

      REPORT_MESSAGE( MSG::INFO ) << "Loaded Trigger configuration from Conditions Store and Detector Store:"
          << "  SMK = " << (currentHlt.isInitialized() ? std::to_string(currentHlt.smk()) : std::string("UNKNOWN"))
          << ", L1PSK = " << (currentL1ps.isInitialized() ? std::to_string(currentL1ps.psk()) : std::string("UNKNOWN"))
          << ", HLTPSK = " << (currentHltps.isInitialized() ? std::to_string(currentHltps.psk()) : std::string("UNKNOWN")) << endmsg;

      REPORT_MESSAGE( MSG::DEBUG ) << "ctpConfig.menu().size() = " << ctpConfig.menu().size()
         << " chainList.size() = " << chainList.size()
         << " sequenceList.size() = " << sequenceList.size()
         << " bgSet.bunchGroups().size() = " << bgSet.bunchGroups().size() << endmsg;

      return StatusCode::SUCCESS;
   }



   StatusCode xAODConfigSvc::prepareEventxAODTriggerMenuJson(const xAOD::TrigConfKeys* keys, const EventContext& context) {
      const xAOD::TriggerMenuJson* loadedHltJson = m_currentHltJson.get(context)->m_ptr;
      const xAOD::TriggerMenuJson* loadedL1Json = m_currentL1Json.get(context)->m_ptr;
      const xAOD::TriggerMenuJson* loadedHltpsJson = m_currentHltpsJson.get(context)->m_ptr;
      const xAOD::TriggerMenuJson* loadedL1psJson = m_currentL1psJson.get(context)->m_ptr;
      //const xAOD::TriggerMenuJson* loadedBgJson = m_currentBgJson.get(context)->m_ptr;

      bool validConfig = true;
      if (loadedHltJson == nullptr || loadedHltJson->key() != keys->smk()) {
         validConfig = false;
      }
      if (loadedL1Json == nullptr || loadedL1Json->key() != keys->smk()) {
         validConfig = false;
      }
      if (loadedHltpsJson == nullptr || loadedHltpsJson->key() != keys->hltpsk()) {
         validConfig = false;
      }
      if (loadedL1psJson == nullptr || loadedL1psJson->key() != keys->l1psk()) {
         validConfig = false;
      }
      // if (loadedBgJson == nullptr || loadedBgJson->key() != TODO) {
      //    validConfig = false;
      // }

      if (validConfig) {
         REPORT_MESSAGE( MSG::DEBUG )
           << "Configuration matches the loaded one, nothing to do." << endmsg;
         return StatusCode::SUCCESS;
      }

      // If not, let's look for the correct configuration:
      // Open a shared lock. OK for multiple events to search at the same time,
      // but prevent the extension of m_hltJson et. al. from a BeginInputFile incident.
      std::shared_lock lockShared(m_sharedMutex);

      TriggerMenuJsonPtrWrapper& currentHltJson   = *(m_currentHltJson.get(context));
      TriggerMenuJsonPtrWrapper& currentL1Json    = *(m_currentL1Json.get(context));
      TriggerMenuJsonPtrWrapper& currentHltpsJson = *(m_currentHltpsJson.get(context));
      TriggerMenuJsonPtrWrapper& currentL1psJson  = *(m_currentL1psJson.get(context));
      //TriggerMenuJsonPtrWrapper& currentBgJson    = *(m_currentBgJson.get(context));

      TrigConf::HLTMenu&         currentHlt   = *(m_currentHlt.get(context));
      TrigConf::L1Menu&          currentL1    = *(m_currentL1.get(context));
      TrigConf::HLTPrescalesSet& currentHltps = *(m_currentHltps.get(context));
      TrigConf::L1PrescalesSet&  currentL1ps  = *(m_currentL1ps.get(context));
      TrigConf::L1BunchGroupSet& currentBg    = *(m_currentBg.get(context));

      ATH_CHECK( loadPtree("HLT Menu",      m_hltJson.get(),   keys->smk(),    currentHltJson,   currentHlt) );
      ATH_CHECK( loadPtree("L1 Menu",       m_l1Json.get(),    keys->smk(),    currentL1Json,    currentL1) );
      ATH_CHECK( loadPtree("HLT Prescales", m_hltpsJson.get(), keys->hltpsk(), currentHltpsJson, currentHltps) );
      ATH_CHECK( loadPtree("L1 Prescales",  m_l1psJson.get(),  keys->l1psk(),  currentL1psJson,  currentL1ps) );
      // ATH_CHECK( loadPtree("Bunchgroups",   m_bgJson.get(),    TODO,           currentBgGJson,    currentBg) );

      // Loading the payload doesn't additionally let the object know about its own key. We can load this in now too.
      // The current*Json objects were updated by loadPtree to point to the entry with the correct key.
      // We don't set this in loadPtree as the poperties are on the derived objects, not the base DataStructure.
      currentHlt.setSMK( currentHltJson.m_ptr->key() );
      currentL1.setSMK( currentL1Json.m_ptr->key() );
      currentHltps.setPSK( currentHltpsJson.m_ptr->key() );
      currentL1ps.setPSK( currentL1psJson.m_ptr->key() );
      //currentBg 

      CTPConfig& ctpConfig = *(m_ctpConfig.get(context));
      HLTChainList& chainList = *(m_chainList.get(context));
      HLTSequenceList& sequenceList = *(m_sequenceList.get(context));
      BunchGroupSet& bgSet = *(m_bgSet.get(context));

      // Copy data into legacy long-term-support interfaces
      CHECK( prepareTriggerMenu( currentHlt,
                                 currentL1,
                                 currentHltps,
                                 currentL1ps,
                                 currentBg,
                                 ctpConfig,
                                 chainList,
                                 sequenceList,
                                 bgSet,
                                 msg() ) );

      REPORT_MESSAGE( MSG::INFO ) << "Loaded xAOD::TriggerMenuJson configuration:"
          << "  SMK = " << keys->smk()
          << ", L1PSK = " << keys->l1psk()
          << ", HLTPSK = " << keys->hltpsk() << endmsg;

      REPORT_MESSAGE( MSG::DEBUG ) << "ctpConfig.menu().size() = " << ctpConfig.menu().size()
         << " chainList.size() = " << chainList.size()
         << " sequenceList.size() = " << sequenceList.size()
         << " bgSet.bunchGroups().size() = " << bgSet.bunchGroups().size() << endmsg;

      return StatusCode::SUCCESS;

   }

   StatusCode xAODConfigSvc::loadPtree(const std::string& humanName,
                                       const xAOD::TriggerMenuJsonContainer* metaContainer,
                                       const uint32_t keyToCheck,
                                       TriggerMenuJsonPtrWrapper& cacheOfLoadedMenuPtr,
                                       DataStructure& dataStructure) {
      xAOD::TriggerMenuJsonContainer::const_iterator menu_itr = metaContainer->begin();
      xAOD::TriggerMenuJsonContainer::const_iterator menu_end = metaContainer->end();
      const xAOD::TriggerMenuJson* ptrToLocatedMenu = nullptr;
      for( ; menu_itr != menu_end; ++menu_itr ) {
         // Check if this is the menu we're looking for:
         if( keyToCheck != (*menu_itr)->key() ) continue;
         // Remember its pointer:
         ptrToLocatedMenu = *menu_itr;
         cacheOfLoadedMenuPtr.m_ptr = ptrToLocatedMenu;
         std::stringstream rawData;
         rawData << ptrToLocatedMenu->payload();
         dataStructure.clear();
         try {
            boost::property_tree::ptree pt;
            boost::property_tree::read_json(rawData, pt);
            dataStructure.setData(std::move(pt));
         } catch (const boost::property_tree::json_parser_error& e) {
            REPORT_MESSAGE( MSG::FATAL ) << "Unable to decode a JSON trigger menu metadata payload for " << humanName << " with key " << keyToCheck << endmsg;
            REPORT_MESSAGE( MSG::FATAL ) << e.what();
            return StatusCode::FAILURE;
         }
      }

      if (ptrToLocatedMenu == nullptr) {
         REPORT_MESSAGE( MSG::FATAL )
            << "Couldn't find configuration for current event"
            << ", Requested key=" << keyToCheck
            << ", Requested menu=" << humanName
            << endmsg;
         return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
   }

} // namespace TrigConf
