/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerMenuMetaDataTool.cxx 683395 2015-07-16 11:11:56Z krasznaa $

// System include(s):
#include <memory>

#include <stdexcept>

// Hack to initialise decoration accessors for Trigger EDM in offline jobs
// starting from ESD or later, see ATR-22421
#if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS)
#include "TrigOutputHandling/TriggerEDMAuxAccessors.h"
#endif

// Local include(s):
#include "xAODTriggerCnv/TriggerMenuMetaDataTool.h"

namespace xAODMaker {

   std::mutex TriggerMenuMetaDataTool::s_mutex;

   TriggerMenuMetaDataTool::TriggerMenuMetaDataTool( const std::string& name )
      : asg::AsgMetadataTool( name ),
        m_menu(), m_menuAux(), m_beginFileIncidentSeen( false ) {



#ifndef XAOD_STANDALONE
      declareInterface< ::IMetaDataTool >( this );
#endif // XAOD_STANDALONE

      declareProperty("InputKey", m_inputKey = "TriggerMenu");
      declareProperty("OutputKey", m_outputKey = "TriggerMenu");
      declareProperty("InputKeyJSON_HLT", m_inputKeyJSON_HLT = "TriggerMenuJson_HLT");
      declareProperty("OutputKeyJSON_HLT", m_outputKeyJSON_HLT = "TriggerMenuJson_HLT");
      declareProperty("InputKeyJSON_HLTMonitoring", m_inputKeyJSON_HLTMonitoring = "TriggerMenuJson_HLTMonitoring");
      declareProperty("OutputKeyJSON_HLTMonitoring", m_outputKeyJSON_HLTMonitoring = "TriggerMenuJson_HLTMonitoring");
      declareProperty("InputKeyJSON_L1", m_inputKeyJSON_L1 = "TriggerMenuJson_L1");
      declareProperty("OutputKeyJSON_L1", m_outputKeyJSON_L1 = "TriggerMenuJson_L1");
      declareProperty("InputKeyJSON_HLTPS", m_inputKeyJSON_HLTPS = "TriggerMenuJson_HLTPS");
      declareProperty("OutputKeyJSON_HLTPS", m_outputKeyJSON_HLTPS = "TriggerMenuJson_HLTPS");
      declareProperty("InputKeyJSON_L1PS", m_inputKeyJSON_L1PS = "TriggerMenuJson_L1PS");
      declareProperty("OutputKeyJSON_L1PS", m_outputKeyJSON_L1PS = "TriggerMenuJson_L1PS");
      declareProperty("InputKeyJSON_BG", m_inputKeyJSON_BG = "TriggerMenuJson_BG");
      declareProperty("OutputKeyJSON_BG", m_outputKeyJSON_BG = "TriggerMenuJson_BG");
   }

   StatusCode TriggerMenuMetaDataTool::initialize() {

      // Greet the user:
      ATH_MSG_DEBUG( "Initialising TriggerMenuMetaDataTool" );


      // Reset the internal variable(s):
      m_menu.reset();
      m_menuAux.reset();
      m_menuJSON_hlt.reset();
      m_menuJSON_hltAux.reset();
      m_menuJSON_hltmonitoring.reset();
      m_menuJSON_hltmonitoringAux.reset();
      m_menuJSON_l1.reset();
      m_menuJSON_l1Aux.reset();
      m_menuJSON_hltps.reset();
      m_menuJSON_hltpsAux.reset();
      m_menuJSON_l1ps.reset();
      m_menuJSON_l1psAux.reset();
      m_menuJSON_bg.reset();
      m_menuJSON_bgAux.reset();

      m_beginFileIncidentSeen = false;

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TriggerMenuMetaDataTool::endInputFile() {
      // Return gracefully:
      return StatusCode::SUCCESS;
   }
   StatusCode TriggerMenuMetaDataTool::beginInputFile() {
      // Whatever happens, we've seen the first BeginInputFile incident now.
      m_beginFileIncidentSeen = true;
      ATH_CHECK( checkxAODTriggerMenu() );
      ATH_CHECK( checkxAODTriggerMenuJson() );
      return StatusCode::SUCCESS;
   }

   StatusCode TriggerMenuMetaDataTool::checkxAODTriggerMenu() {
      // If the input file doesn't have any trigger configuration metadata,
      // then finish right away:
      if( ! inputMetaStore()->contains< xAOD::TriggerMenuContainer >(
                                                                m_inputKey ) ) {
         return StatusCode::SUCCESS;
      }

      // Retrieve the input container:
      const xAOD::TriggerMenuContainer* input = nullptr;
      ATH_CHECK( inputMetaStore()->retrieve( input, m_inputKey ) );

      // Create an internal container if it doesn't exist yet:
      if( ( ! m_menu.get() ) && ( ! m_menuAux.get() ) ) {
         ATH_MSG_DEBUG( "Creating internal container" );
         m_menu = std::make_unique<xAOD::TriggerMenuContainer>( );
         m_menuAux = std::make_unique<xAOD::TriggerMenuAuxContainer>( );
         m_menu->setStore( m_menuAux.get() );
      }

      // Copy (and de-duplicate) from the input collection to the internal collection
      ATH_CHECK( doCopyxAODTriggerMenu(input, m_menu.get()) );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TriggerMenuMetaDataTool::doCopyxAODTriggerMenu(const xAOD::TriggerMenuContainer* copyFrom, xAOD::TriggerMenuContainer* copyTo) {
      // Loop over the configurations of the copyFrom collection:
      for( const xAOD::TriggerMenu* menu : *copyFrom ) {

         // Check if this configuration is already in the copyTo container:
         bool exists = false;
         for( const xAOD::TriggerMenu* existing : *copyTo ) {
            if( ( existing->smk() == menu->smk() ) &&
                ( existing->l1psk() == menu->l1psk() ) &&
                ( existing->hltpsk() == menu->hltpsk() ) ) {
               exists = true;
               break;
            }
         }
         if( exists ) {
            continue;
         }

         // If it's a new configuration, put it into the copyTo container:
         ATH_MSG_VERBOSE( "Copying configuration with SMK: "
                          << menu->smk() << ", L1PSK: " << menu->l1psk()
                          << ", HLTPSK: " << menu->hltpsk() );
         xAOD::TriggerMenu* out = new xAOD::TriggerMenu();
         copyTo->push_back( out );
         *out = *menu;
      }
      return StatusCode::SUCCESS;
   }

   StatusCode TriggerMenuMetaDataTool::checkxAODTriggerMenuJson() {
      ATH_CHECK( checkCopyJson(m_inputKeyJSON_HLT, m_menuJSON_hlt, m_menuJSON_hltAux) );
      ATH_CHECK( checkCopyJson(m_inputKeyJSON_HLTMonitoring, m_menuJSON_hltmonitoring, m_menuJSON_hltmonitoringAux) );
      ATH_CHECK( checkCopyJson(m_inputKeyJSON_L1, m_menuJSON_l1, m_menuJSON_l1Aux) );
      ATH_CHECK( checkCopyJson(m_inputKeyJSON_HLTPS, m_menuJSON_hltps, m_menuJSON_hltpsAux) );
      ATH_CHECK( checkCopyJson(m_inputKeyJSON_L1PS, m_menuJSON_l1ps, m_menuJSON_l1psAux) );
      ATH_CHECK( checkCopyJson(m_inputKeyJSON_BG, m_menuJSON_bg, m_menuJSON_bgAux) );
      return StatusCode::SUCCESS;
   }

   StatusCode TriggerMenuMetaDataTool::checkCopyJson(const std::string& inputMetaSGKey,
      std::unique_ptr< xAOD::TriggerMenuJsonContainer >& internalContainer,
      std::unique_ptr< xAOD::TriggerMenuJsonAuxContainer >& internalAuxContainer)
   {

      if( !inputMetaStore()->contains<xAOD::TriggerMenuJsonContainer>(inputMetaSGKey)) {
         return StatusCode::SUCCESS;
      }

      const xAOD::TriggerMenuJsonContainer* input = nullptr;
      if (inputMetaStore()->retrieve( input, inputMetaSGKey ).isSuccess() ) {

         // Create an internal container if it doesn't exist yet:
         if( ( ! internalContainer.get() ) && ( ! internalAuxContainer.get() ) ) {
            ATH_MSG_DEBUG( "Creating internal container" );
            internalContainer = std::make_unique<xAOD::TriggerMenuJsonContainer>( );
            internalAuxContainer = std::make_unique<xAOD::TriggerMenuJsonAuxContainer>( );
            internalContainer->setStore( internalAuxContainer.get() );
         }

         // Copy (and de-duplicate) from the input collection to the internal collection
         ATH_CHECK( doCopyxAODTriggerMenuJson(inputMetaSGKey, input, internalContainer.get()) );
      }
      return StatusCode::SUCCESS;
   }

   StatusCode TriggerMenuMetaDataTool::doCopyxAODTriggerMenuJson(const std::string& inputMetaSGKey,
      const xAOD::TriggerMenuJsonContainer* copyFrom,
      xAOD::TriggerMenuJsonContainer* copyTo)
   {
      // Loop over the configurations of the copyFrom input:
      for( const xAOD::TriggerMenuJson* menuJson : *copyFrom ) {
         // Check if this configuration is already in the copyTo container:
         bool exists = false;
         for( const xAOD::TriggerMenuJson* existing : *copyTo ) {
            if (existing->key() == menuJson->key()) {
               exists = true;
               break;
            }
         }
         if( exists ) {
            continue;
         }

         // If it's a new configuration, put it into the copyTo container:
         ATH_MSG_VERBOSE( "Copying " << inputMetaSGKey << " configuration with Key: " << menuJson->key() );
         xAOD::TriggerMenuJson* out = new xAOD::TriggerMenuJson();
         copyTo->push_back( out );
         *out = *menuJson;
      }
      return StatusCode::SUCCESS;
   }

   StatusCode TriggerMenuMetaDataTool::beginEvent() {

      // In case the BeginInputFile incident was missed in standalone mode, make
      // sure that the metadata from the first input file is collected at this
      // point at least.
      if( ! m_beginFileIncidentSeen ) {
         ATH_CHECK( beginInputFile() );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TriggerMenuMetaDataTool::metaDataStop() {
      // Note: Copying into a given collection in the output store should only be done
      // by a single thread at a time.
      std::scoped_lock lock(s_mutex);

      ATH_CHECK( endxAODTriggerMenu() );
      ATH_CHECK( endxAODTriggerMenuJson() );
      return StatusCode::SUCCESS;
   }

   StatusCode TriggerMenuMetaDataTool::endxAODTriggerMenu() {
      // The output may already have trigger configuration metadata in it.
      // For instance from TrigConf::xAODMenuWriter or other instances of the
      // TriggerMenuMetaDataTool in MP mode. Merge into the output

      // If we don't have an internal store then nothing to do
      if( ( ! m_menu.get() ) && ( ! m_menuAux.get() ) ) {
         ATH_MSG_DEBUG( "No internal xAOD::TriggerMenu store to save/merge to output. (Expected in Run 3)." );
         return StatusCode::SUCCESS;
      }

      if( not outputMetaStore()->contains<xAOD::TriggerMenuContainer>(m_outputKey) ) {
         // No output yet - hand over ownership of our internal store
         ATH_MSG_DEBUG( "Recording xAOD::TriggerMenu trigger configuration metadata container with " << m_menu->size() << " entries." );
         ATH_CHECK( outputMetaStore()->record( m_menu.release(),
                                               m_outputKey ) );
         ATH_CHECK( outputMetaStore()->record( m_menuAux.release(),
                                               m_outputKey + "Aux." ) );
      } else {
         // Merge into the existing output store
         ATH_MSG_DEBUG( "Merging into existing output xAOD::TriggerMenu configuration metadata container" );
         xAOD::TriggerMenuContainer* output = nullptr;
         ATH_CHECK( outputMetaStore()->retrieve( output, m_outputKey ) );
         // Copy (and de-duplicate) from the internal collection to the output collection
         ATH_CHECK( doCopyxAODTriggerMenu(m_menu.get(), output) );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TriggerMenuMetaDataTool::endxAODTriggerMenuJson() {
      ATH_CHECK( checkExportJson(m_outputKeyJSON_HLT, m_menuJSON_hlt, m_menuJSON_hltAux) );
      ATH_CHECK( checkExportJson(m_outputKeyJSON_HLTMonitoring, m_menuJSON_hltmonitoring, m_menuJSON_hltmonitoringAux) );
      ATH_CHECK( checkExportJson(m_outputKeyJSON_L1, m_menuJSON_l1, m_menuJSON_l1Aux) );
      ATH_CHECK( checkExportJson(m_outputKeyJSON_HLTPS, m_menuJSON_hltps, m_menuJSON_hltpsAux) );
      ATH_CHECK( checkExportJson(m_outputKeyJSON_L1PS, m_menuJSON_l1ps, m_menuJSON_l1psAux) );
      ATH_CHECK( checkExportJson(m_outputKeyJSON_BG, m_menuJSON_bg, m_menuJSON_bgAux) );
      return StatusCode::SUCCESS;
   }

   StatusCode TriggerMenuMetaDataTool::checkExportJson(const std::string& outputMetaSGKey,
      std::unique_ptr< xAOD::TriggerMenuJsonContainer >& internalContainer,
      std::unique_ptr< xAOD::TriggerMenuJsonAuxContainer >& internalAuxContainer)
   {

      // The output may already have trigger configuration metadata in it.
      // For instance from TrigConf::xAODMenuWriterMT or other instances of the
      // TriggerMenuMetaDataTool in MP mode. Merge into the output

      // If we don't have an internal store then nothing to do
      if( ( ! internalContainer.get() ) && ( ! internalAuxContainer.get() ) ) {
         ATH_MSG_DEBUG( "No internal xAOD::TriggerJsonMenu " << outputMetaSGKey 
            << " store to save/merge to output. (Expected for Run 2 MC, release 21 data, or the BG data as this is not added as of Dec 21)." );
         return StatusCode::SUCCESS;
      }

      if( not outputMetaStore()->contains< xAOD::TriggerMenuJsonContainer >( outputMetaSGKey ) ) {
         // No output yet - hand over ownership of our internal store
         ATH_MSG_DEBUG( "Recording " << outputMetaSGKey << " xAOD::TriggerMenuJson trigger configuration metadata container with " 
            << internalContainer->size() << " entries" );
         ATH_CHECK( outputMetaStore()->record( internalContainer.release(),
                                               outputMetaSGKey ) );
         ATH_CHECK( outputMetaStore()->record( internalAuxContainer.release(),
                                               outputMetaSGKey + "Aux." ) );
      } else {
         // Merge into the existing output store
         ATH_MSG_DEBUG( "Merging into existing " << outputMetaSGKey << " output xAOD::TriggerMenuJson configuration metadata container" );
         xAOD::TriggerMenuJsonContainer* output = nullptr;
         ATH_CHECK( outputMetaStore()->retrieve( output, outputMetaSGKey ) );
         // Copy (and de-duplicate) from the internal collection to the output collection
         ATH_CHECK( doCopyxAODTriggerMenuJson(outputMetaSGKey, internalContainer.get(), output) );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
