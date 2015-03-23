/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerMenuMetaDataTool.cxx 656102 2015-03-23 12:32:37Z krasznaa $

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TriggerMenuMetaDataTool.h"

namespace xAODMaker {

   /// Name of the incident sent around for metadata writing
   static const char* METADATASTOP = "MetaDataStop";

   TriggerMenuMetaDataTool::TriggerMenuMetaDataTool( const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent )
      : AthAlgTool( type, name, parent ),
        m_inputMetaStore( "StoreGateSvc/InputMetaDataStore", name ),
        m_outputMetaStore( "StoreGateSvc/MetaDataStore", name ),
        m_menu(), m_menuAux() {

      declareProperty( "InputMetaStore", m_inputMetaStore );
      declareProperty( "OutputMetaStore", m_outputMetaStore );

      declareProperty( "InputKey", m_inputKey = "TriggerMenu" );
      declareProperty( "OutputKey", m_outputKey = "TriggerMenu" );

      declareInterface< ::IMetaDataTool >( this );
      declareInterface< ::IIncidentListener >( this );
   }

   StatusCode TriggerMenuMetaDataTool::initialize() {

      // Greet the user:
      ATH_MSG_DEBUG( "Initialising - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( "  InputMetaStore  = " << m_inputMetaStore );
      ATH_MSG_DEBUG( "  OutputMetaStore = " << m_outputMetaStore );
      ATH_MSG_DEBUG( "  InputKey  = " << m_inputKey );
      ATH_MSG_DEBUG( "  OutputKey = " << m_outputKey );

      // Connect to the metadata stores:
      ATH_CHECK( m_inputMetaStore.retrieve() );
      ATH_CHECK( m_outputMetaStore.retrieve() );

      // Make the tool listen to certain incidents:
      ServiceHandle< ::IIncidentSvc > incSvc( "IncidentSvc", name() );
      ATH_CHECK( incSvc.retrieve() );
      incSvc->addListener( this, IncidentType::BeginInputFile );
      incSvc->addListener( this, METADATASTOP, 70 );

      // Retrun gracefully:
      return StatusCode::SUCCESS;
   }

   void TriggerMenuMetaDataTool::handle( const Incident& inc ) {

      // Some debugging information:
      ATH_MSG_DEBUG( "Received incident: " << inc.type() );

      if( inc.type() == IncidentType::BeginInputFile ) {
         if( collectMetaData().isFailure() ) {
            REPORT_ERROR( MSG::FATAL )
               << "Failed to collect metadata from the input file";
            throw std::runtime_error( "Failed to collect trigger configuration "
                                      "metadata from the input" );
         }
      } else if( inc.type() == METADATASTOP ) {
         if( writeMetaData().isFailure() ) {
            REPORT_ERROR( MSG::FATAL )
               << "Failed to write metadata to the output store";
            throw std::runtime_error( "Failed write xAOD::TriggerMenuContainer "
                                      "to the output" );
         }
      } else {
         ATH_MSG_WARNING( "Unknown incident type (" << inc.type() << ") "
                          << "received" );
      }

      return;
   }

   StatusCode TriggerMenuMetaDataTool::collectMetaData() {

      // If the input file doesn't have any trigger configuration metadata,
      // then finish right away:
      if( ! m_inputMetaStore->contains< xAOD::TriggerMenuContainer >(
                                                                m_inputKey ) ) {
         return StatusCode::SUCCESS;
      }

      // Retrieve the input container:
      const xAOD::TriggerMenuContainer* input = 0;
      ATH_CHECK( m_inputMetaStore->retrieve( input, m_inputKey ) );

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

   StatusCode TriggerMenuMetaDataTool::writeMetaData() {

      // The output may already have trigger configuration metadata in it.
      // For instance from TrigConf::xAODMenuWriter. In this case let that
      // object take precedence.
      if( m_outputMetaStore->contains< xAOD::TriggerMenuContainer >(
                                                               m_outputKey ) ) {
         ATH_MSG_DEBUG( "xAOD::TriggerMenuContainer already in the output" );
         return StatusCode::SUCCESS;
      }

      // Record the trigger configuration metadata, if any was found in the
      // processed input files.
      if( m_menu.get() && m_menuAux.get() ) {
         ATH_MSG_DEBUG( "Recording trigger configuration metadata" );
         ATH_CHECK( m_outputMetaStore->record( std::move( m_menu ),
                                               m_outputKey ) );
         ATH_CHECK( m_outputMetaStore->record( std::move( m_menuAux ),
                                               m_outputKey + "Aux." ) );
      }

      // Retrun gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
