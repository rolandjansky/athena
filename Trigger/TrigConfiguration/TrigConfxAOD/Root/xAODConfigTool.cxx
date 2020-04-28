/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODConfigTool.cxx 673576 2015-06-09 08:44:29Z krasznaa $

// System include(s):
#include <stdexcept>

// Trigger include(s):
#include "TrigConfL1Data/BunchGroup.h"
#include "TrigConfHLTData/HLTSequenceList.h"

// xAOD include(s):
#include "xAODTrigger/TrigConfKeys.h"

// Local include(s):
#include "TrigConfxAOD/xAODConfigTool.h"
#include "TrigConfxAOD/tools/prepareTriggerMenu.h"
#include "TrigConfxAOD/tools/xAODKeysMatch.h"

namespace TrigConf {

   struct xAODConfigTool::Impl
   {
      /// The "translated" LVL1 configuration object
      CTPConfig m_ctpConfig;
      /// The "translated" HLT configuration object
      HLTChainList m_chainList;
      /// The "translated" HLT configuration object
      HLTSequenceList m_sequenceList;
      /// The "translated" bunch group set object
      BunchGroupSet m_bgSet;
   };

   xAODConfigTool::xAODConfigTool( const std::string& name )
      : asg::AsgMetadataTool( name ),
        m_tmc( 0 ), m_menu( 0 ),
        m_impl (std::make_unique<Impl>()) {

      declareProperty( "EventObjectName", m_eventName = "TrigConfKeys" );
      declareProperty( "MetaObjectName", m_metaName = "TriggerMenu" );
   }

   xAODConfigTool::~xAODConfigTool()
   {
   }

   StatusCode xAODConfigTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising..." );
      ATH_MSG_DEBUG( "EventObjectName = " << m_eventName );
      ATH_MSG_DEBUG( "MetaObjectName  = " << m_metaName );

      // Reset the pointers:
      m_tmc = 0; m_menu = 0;

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   const CTPConfig* xAODConfigTool::ctpConfig() const {

      // Check if the object is well prepared:
      if( ! m_menu ) {
         ATH_MSG_FATAL( "Trigger menu not loaded" );
         throw std::runtime_error( "Tool not initialised correctly" );
      }

      // Return the pointer:
      return &m_impl->m_ctpConfig;
   }

   const BunchGroupSet* xAODConfigTool::bunchGroupSet() const {

      // Check if the object is well prepared:
      if( ! m_menu ) {
         ATH_MSG_FATAL( "Trigger menu not loaded" );
         throw std::runtime_error( "Tool not initialised correctly" );
      }

      // Return the pointer:
      return &m_impl->m_bgSet;
   }

   uint32_t xAODConfigTool::lvl1PrescaleKey() const {

      // Check if the object is well prepared:
      if( ! m_menu ) {
         ATH_MSG_FATAL( "Trigger menu not loaded" );
         throw std::runtime_error( "Tool not initialised correctly" );
      }

      // Return the key from the metadata object:
      return m_menu->l1psk();
   }

   const HLTChainList* xAODConfigTool::chainList() const {

      // Check if the object is well prepared:
      if( ! m_menu ) {
         ATH_MSG_FATAL( "Trigger menu not loaded" );
         throw std::runtime_error( "Tool not initialised correctly" );
      }

      // Return the pointer:
      return &m_impl->m_chainList;
   }

   const HLTChainList& xAODConfigTool::chains() const {

      // Check if the object is well prepared:
      if( ! m_menu ) {
         ATH_MSG_FATAL( "Trigger menu not loaded" );
         throw std::runtime_error( "Tool not initialised correctly" );
      }

      // Return the object:
      return m_impl->m_chainList;
   }

   const HLTSequenceList* xAODConfigTool::sequenceList() const {

      // Check if the object is well prepared:
      if( ! m_menu ) {
         ATH_MSG_FATAL( "Trigger menu not loaded" );
         throw std::runtime_error( "Tool not initialised correctly" );
      }

      // Return the pointer:
      return &m_impl->m_sequenceList;
   }

   const HLTSequenceList& xAODConfigTool::sequences() const {

      // Check if the object is well prepared:
      if( ! m_menu ) {
         ATH_MSG_FATAL( "Trigger menu not loaded" );
         throw std::runtime_error( "Tool not initialised correctly" );
      }

      // Return the object:
      return m_impl->m_sequenceList;
   }

   uint32_t xAODConfigTool::masterKey() const {

      // Check if the object is well prepared:
      if( ! m_menu ) {
         ATH_MSG_FATAL( "Trigger menu not loaded" );
         throw std::runtime_error( "Tool not initialised correctly" );
      }

      // Return the key from the metadata object:
      return m_menu->smk();
   }

   uint32_t xAODConfigTool::hltPrescaleKey() const {

      // Check if the object is well prepared:
      if( ! m_menu ) {
         ATH_MSG_FATAL( "Trigger menu not loaded" );
         throw std::runtime_error( "Tool not initialised correctly" );
      }

      // Return the key from the metadata object:
      return m_menu->hltpsk();
   }

   StatusCode xAODConfigTool::beginInputFile() {

      // Tell the user what's happening:
      ATH_MSG_DEBUG( "Loading the trigger menu from a new input file" );

      // Read the metadata object:
      m_tmc = 0;
      ATH_CHECK( inputMetaStore()->retrieve( m_tmc, m_metaName ) );

      // A little sanity check:
      if( ! m_tmc->size() ) {
         // This can happen when we encounter empty input files. In which
         // case we should not bail, but continue, and only bail if by the
         // start of an event, we still don't see any configurations.
         ATH_MSG_WARNING( "No trigger configurations are available on "
                          "the input" );
         return StatusCode::SUCCESS;
      }

      // Point the menu pointer to the first element by default:
      m_menu = m_tmc->at( 0 );
      // Cache the menu's configuration:
      ATH_CHECK( prepareTriggerMenu( m_menu, m_impl->m_ctpConfig,
                                     m_impl->m_chainList,
                                     m_impl->m_sequenceList,
                                     m_impl->m_bgSet, msg() ) );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode xAODConfigTool::beginEvent() {

      // It may be that the input file opening event is missed in standalone
      // mode at the very beginning of the application. (Depending on the
      // creation order of the objects.) So make sure that we have the
      // configuration objects at hand...
      if( ( ! m_tmc ) || ( ! m_menu ) ) {
         ATH_CHECK( beginInputFile() );
      }

      // Read the current event's trigger keys:
      const xAOD::TrigConfKeys* keys = 0;
      ATH_CHECK( evtStore()->retrieve( keys, m_eventName ) );

      // Check if we have the correct menu already:
      if( m_menu && xAODKeysMatch( keys, m_menu ) ) {
         return StatusCode::SUCCESS;
      }

      // If not, let's look for the correct configuration:
      xAOD::TriggerMenuContainer::const_iterator menu_itr = m_tmc->begin();
      xAOD::TriggerMenuContainer::const_iterator menu_end = m_tmc->end();
      for( ; menu_itr != menu_end; ++menu_itr ) {
         // Check if this is the menu we're looking for:
         if( ! xAODKeysMatch( keys, *menu_itr ) ) continue;
         // Remember it's pointer:
         m_menu = *menu_itr;
         // Cache the menu's configuration:
         ATH_CHECK( prepareTriggerMenu( m_menu, m_impl->m_ctpConfig,
                                        m_impl->m_chainList,
                                        m_impl->m_sequenceList,
                                        m_impl->m_bgSet, msg() ) );
         // We're done:
         return StatusCode::SUCCESS;
      }

      // Apparently we didn't find the correct menu!
      ATH_MSG_ERROR( "Couldn't find configuration for current event (SMK:"
                     << keys->smk() << ", L1PSK:" << keys->l1psk()
                     << ", HLTPSK:" << keys->hltpsk() << ")" );
      return StatusCode::FAILURE;
   }

} // namespace TrigConf
