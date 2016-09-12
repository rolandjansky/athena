/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfoCnvAlg.cxx 760935 2016-07-11 08:30:56Z krasznaa $

// System include(s):
#include <memory>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "EventInfo/EventInfo.h"
#include "EventInfo/PileUpEventInfo.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"
#include "xAODEventInfo/EventInfoContainer.h"
#include "xAODEventInfo/EventInfoAuxContainer.h"

// Local include(s):
#include "EventInfoCnvAlg.h"

namespace xAODMaker {

   EventInfoCnvAlg::EventInfoCnvAlg( const std::string& name,
                                     ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::EventInfoCnvTool/EventInfoCnvTool", this ) {

      declareProperty( "AODKey", m_aodKey = "" );
      declareProperty( "xAODKey", m_xaodKey = "EventInfo" );
      declareProperty( "CnvTool", m_cnvTool );
      declareProperty( "ForceOverwrite", m_overwrite = false );
   }

   StatusCode EventInfoCnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( "       AOD Key: " << m_aodKey );
      ATH_MSG_DEBUG( "      xAOD Key: " << m_xaodKey );
      ATH_MSG_DEBUG( "ForceOverwrite: " << m_overwrite );

      // Retrieve the converter tool:
      CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode EventInfoCnvAlg::execute() {

      // Check if anything needs to be done:
      if( evtStore()->contains< xAOD::EventInfo >( m_xaodKey ) &&
          ( ! m_overwrite ) ) { 
         ATH_MSG_DEBUG( "xAOD::EventInfo with key \"" << m_xaodKey
                        << "\" is already in StoreGate" );
         return StatusCode::SUCCESS;
      }

      // Retrieve the AOD object:
      const EventInfo* aod = 0;
      if( m_aodKey == "" ) {
         CHECK( evtStore()->retrieve( aod ) );
      } else {
         CHECK( evtStore()->retrieve( aod, m_aodKey ) );
      }

      // Create the xAOD object(s):
      std::unique_ptr< xAOD::EventAuxInfo > aux( new xAOD::EventAuxInfo() );
      std::unique_ptr< xAOD::EventInfo > xaod( new xAOD::EventInfo() );
      xaod->setStore( aux.get() );

      // Do the translation:
      CHECK( m_cnvTool->convert( aod, xaod.get() ) );

      // Record the xAOD object(s):
      if( m_overwrite ) {
         CHECK( evtStore()->overwrite( std::move( aux ), m_xaodKey + "Aux.",
                                       false, false, true ) );
         CHECK( evtStore()->overwrite( std::move( xaod ), m_xaodKey,
                                       false, false, true ) );
      } else {
         CHECK( evtStore()->record( std::move( aux ), m_xaodKey + "Aux." ) );
         CHECK( evtStore()->record( std::move( xaod ), m_xaodKey ) );
      }

      // Check if this is a PileUpEventInfo object:
      const PileUpEventInfo* paod =
         dynamic_cast< const PileUpEventInfo* >( aod );
      // If not, then we're done already:
      if( ! paod ) {
         return StatusCode::SUCCESS;
      }

      // Create an EventInfoContainer for the pileup events:
      std::unique_ptr< xAOD::EventInfoContainer >
         puei( new xAOD::EventInfoContainer() );
      std::unique_ptr< xAOD::EventInfoAuxContainer >
         puaux( new xAOD::EventInfoAuxContainer() );
      puei->setStore( puaux.get() );

      // Create xAOD::EventInfo objects for each pileup EventInfo object:
      PileUpEventInfo::SubEvent::const_iterator pu_itr = paod->beginSubEvt();
      PileUpEventInfo::SubEvent::const_iterator pu_end = paod->endSubEvt();
      for( ; pu_itr != pu_end; ++pu_itr ) {
         // Create a new xAOD object:
         xAOD::EventInfo* ei = new xAOD::EventInfo();
         puei->push_back( ei );
         // Fill it with information:
         CHECK( m_cnvTool->convert( pu_itr->pSubEvt, ei, true ) );
      }

      // Record the xAOD object(s):
      if( m_overwrite ) {
         CHECK( evtStore()->overwrite( std::move( puaux ),
                                       "PileUp" + m_xaodKey + "Aux.",
                                       false, false, true ) );
         CHECK( evtStore()->overwrite( std::move( puei ),
                                       "PileUp" + m_xaodKey,
                                       false, false, true ) );
      } else {
         CHECK( evtStore()->record( std::move( puaux ),
                                    "PileUp" + m_xaodKey + "Aux." ) );
         CHECK( evtStore()->record( std::move( puei ), "PileUp" + m_xaodKey ) );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode EventInfoCnvAlg::beginRun() {

#ifndef ATHENAHIVE
      // Let the user know what's happening:
      ATH_MSG_DEBUG( "Preparing xAOD::EventInfo object in beginRun()" );

      // Run the conversion using the execute function:
      CHECK( execute() );
#endif

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
