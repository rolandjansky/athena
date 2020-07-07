/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerMenuMetaDataTool.cxx 683395 2015-07-16 11:11:56Z krasznaa $

// System include(s):
#include <stdexcept>

// Local include(s):
#include "xAODTriggerCnv/TriggerMenuMetaDataTool.h"

namespace xAODMaker {

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
      declareProperty("InputKeyJSON_L1", m_inputKeyJSON_L1 = "TriggerMenuJson_L1");
      declareProperty("OutputKeyJSON_L1", m_outputKeyJSON_L1 = "TriggerMenuJson_L1");
      declareProperty("InputKeyJSON_HLTPS", m_inputKeyJSON_HLTPS = "TriggerMenuJson_HLTPS");
      declareProperty("OutputKeyJSON_HLTPS", m_outputKeyJSON_HLTPS = "TriggerMenuJson_HLTPS");
      declareProperty("InputKeyJSON_L1PS", m_inputKeyJSON_L1PS = "TriggerMenuJson_L1PS");
      declareProperty("OutputKeyJSON_L1PS", m_outputKeyJSON_L1PS = "TriggerMenuJson_L1PS");
   }

   StatusCode TriggerMenuMetaDataTool::initialize() {

      // Greet the user:
      ATH_MSG_DEBUG( "Initialising TriggerMenuMetaDataTool" );




      // Reset the internal variable(s):
      m_menu.reset();
      m_menuAux.reset();
      m_menuJSON_hlt.reset();
      m_menuJSON_hltAux.reset();
      m_menuJSON_l1.reset();
      m_menuJSON_l1Aux.reset();
      m_menuJSON_hltps.reset();
      m_menuJSON_hltpsAux.reset();
      m_menuJSON_l1ps.reset();
      m_menuJSON_l1psAux.reset();
      // m_menuJSON_bg.reset(); // TODO
      // m_menuJSON_bgAux.reset();

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
      const xAOD::TriggerMenuContainer* input = 0;
      ATH_CHECK( inputMetaStore()->retrieve( input, m_inputKey ) );

      // Create an output container if it doesn't exist yet:
      if( ( ! m_menu.get() ) && ( ! m_menuAux.get() ) ) {
         ATH_MSG_DEBUG( "Creating output container" );
         m_menu.reset( new xAOD::TriggerMenuContainer() );
         m_menuAux.reset( new xAOD::TriggerMenuAuxContainer() );
         m_menu->setStore( m_menuAux.get() );
      }

      // Loop over the configurations of the input file:
      for( const xAOD::TriggerMenu* menu : *input ) {

         // Check if this configuration is already in the output container:
         bool exists = false;
         for( const xAOD::TriggerMenu* existing : *m_menu ) {
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

         // If it's a new configuration, put it into the output container:
         ATH_MSG_VERBOSE( "Copying configuration with SMK: "
                          << menu->smk() << ", L1PSK: " << menu->l1psk()
                          << ", HLTPSK: " << menu->hltpsk() );
         xAOD::TriggerMenu* out = new xAOD::TriggerMenu();
         m_menu->push_back( out );
         *out = *menu;
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TriggerMenuMetaDataTool::checkxAODTriggerMenuJson() {
      ATH_CHECK( checkCopyJSON(m_inputKeyJSON_HLT, m_menuJSON_hlt, m_menuJSON_hltAux) );
      ATH_CHECK( checkCopyJSON(m_inputKeyJSON_L1, m_menuJSON_l1, m_menuJSON_l1Aux) );
      ATH_CHECK( checkCopyJSON(m_inputKeyJSON_HLTPS, m_menuJSON_hltps, m_menuJSON_hltpsAux) );
      ATH_CHECK( checkCopyJSON(m_inputKeyJSON_L1PS, m_menuJSON_l1ps, m_menuJSON_l1psAux) );
      // ATH_CHECK( checkCopyJSON(m_inputKeyJSON_BG, m_menuJSON_bg, m_menuJSON_bgAux) );
      return StatusCode::SUCCESS;
   }

   StatusCode TriggerMenuMetaDataTool::checkCopyJSON(const std::string& inputMetaSGKey,
      std::unique_ptr< xAOD::TriggerMenuJsonContainer >& outContainer,
      std::unique_ptr< xAOD::TriggerMenuJsonAuxContainer >& outAuxContainer) {

      const xAOD::TriggerMenuJsonContainer* input = 0;
      if (inputMetaStore()->retrieve( input, inputMetaSGKey ).isSuccess() ) {

         // Create an output container if it doesn't exist yet:
         if( ( ! outContainer.get() ) && ( ! outAuxContainer.get() ) ) {
            ATH_MSG_DEBUG( "Creating output container" );
            outContainer.reset( new xAOD::TriggerMenuJsonContainer() );
            outAuxContainer.reset( new xAOD::TriggerMenuJsonAuxContainer() );
            outContainer->setStore( outAuxContainer.get() );
         }

         // Loop over the configurations of the input file:
         for( const xAOD::TriggerMenuJson* menuJson : *input ) {
            // Check if this configuration is already in the output container:
            bool exists = false;
            for( const xAOD::TriggerMenuJson* existing : *outContainer ) {
               if (existing->key() == menuJson->key()) {
                  exists = true;
                  break;
               }
            }
            if( exists ) {
               continue;
            }

            // If it's a new configuration, put it into the output container:
            ATH_MSG_VERBOSE( "Copying " << inputMetaSGKey << " configuration with Key: " << menuJson->key() );
            xAOD::TriggerMenuJson* out = new xAOD::TriggerMenuJson();
            outContainer->push_back( out );
            *out = *menuJson;
         }
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
      ATH_CHECK( endxAODTriggerMenu() );
      ATH_CHECK( endxAODTriggerMenuJson() );
      return StatusCode::SUCCESS;
   }

   StatusCode TriggerMenuMetaDataTool::endxAODTriggerMenu() {
      // The output may already have trigger configuration metadata in it.
      // For instance from TrigConf::xAODMenuWriter. In this case let that
      // object take precedence.
      if( outputMetaStore()->contains< xAOD::TriggerMenuContainer >(
                                                               m_outputKey ) ) {
         ATH_MSG_DEBUG( "xAOD::TriggerMenuContainer already in the output" );
         return StatusCode::SUCCESS;
      }

      // Record the trigger configuration metadata, if any was found in the
      // processed input files.
      if( m_menu.get() && m_menuAux.get() ) {
         ATH_MSG_DEBUG( "Recording trigger configuration metadata" );
         ATH_CHECK( outputMetaStore()->record( m_menu.release(),
                                               m_outputKey ) );
         ATH_CHECK( outputMetaStore()->record( m_menuAux.release(),
                                               m_outputKey + "Aux." ) );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TriggerMenuMetaDataTool::endxAODTriggerMenuJson() {
      ATH_CHECK( checkExportJSON(m_outputKeyJSON_HLT, m_menuJSON_hlt, m_menuJSON_hltAux) );
      ATH_CHECK( checkExportJSON(m_outputKeyJSON_L1, m_menuJSON_l1, m_menuJSON_l1Aux) );
      ATH_CHECK( checkExportJSON(m_outputKeyJSON_HLTPS, m_menuJSON_hltps, m_menuJSON_hltpsAux) );
      ATH_CHECK( checkExportJSON(m_outputKeyJSON_L1PS, m_menuJSON_l1ps, m_menuJSON_l1psAux) );
      // ATH_CHECK( checkExportJSON(m_outputKeyJSON_BG, m_menuJSON_bg, m_menuJSON_bgAux) );
      return StatusCode::SUCCESS;
   }

   StatusCode TriggerMenuMetaDataTool::checkExportJSON(const std::string& outputMetaSGKey,
      std::unique_ptr< xAOD::TriggerMenuJsonContainer >& outContainer,
      std::unique_ptr< xAOD::TriggerMenuJsonAuxContainer >& outAuxContainer) {

      if( outputMetaStore()->contains< xAOD::TriggerMenuJsonContainer >( outputMetaSGKey ) ) {
         ATH_MSG_DEBUG( "xAOD::TriggerMenuJsonContainer already in the output" );
         return StatusCode::SUCCESS;
      }

      // Record the trigger configuration metadata, if any was found in the
      // processed input files.
      if( outContainer.get() && outAuxContainer.get() ) {
         ATH_MSG_DEBUG( "Recording " << outputMetaSGKey << " trigger configuration metadata" );
         ATH_CHECK( outputMetaStore()->record( outContainer.release(),
                                               outputMetaSGKey ) );
         ATH_CHECK( outputMetaStore()->record( outAuxContainer.release(),
                                               outputMetaSGKey + "Aux." ) );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
