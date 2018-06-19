/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

      declareProperty( "InputKey", m_inputKey = "TriggerMenu" );
      declareProperty( "OutputKey", m_outputKey = "TriggerMenu" );

#ifdef ASGTOOL_ATHENA
      declareInterface< ::IMetaDataTool >( this );
#endif // ASGTOOL_ATHENA
   }

   StatusCode TriggerMenuMetaDataTool::initialize() {

      // Greet the user:
      ATH_MSG_DEBUG( "Initialising TriggerMenuMetaDataTool" );
      ATH_MSG_DEBUG( "  InputKey  = " << m_inputKey );
      ATH_MSG_DEBUG( "  OutputKey = " << m_outputKey );

      // Reset the internal variable(s):
      m_menu.reset(); m_menuAux.reset();
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

} // namespace xAODMaker
