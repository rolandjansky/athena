/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecisionCnvAlg.cxx 626691 2014-11-06 16:27:15Z krasznaa $

// EDM include(s):
#include "TrigDecisionEvent/TrigDecision.h"
#include "xAODTrigger/TrigDecision.h"
#include "xAODTrigger/TrigDecisionAuxInfo.h"

// Local include(s):
#include "TrigDecisionCnvAlg.h"

namespace xAODMaker {

   TrigDecisionCnvAlg::TrigDecisionCnvAlg( const std::string& name,
                                           ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::TrigDecisionCnvTool/TrigDecisionCnvTool",
                   this ) {

      declareProperty( "AODKey", m_aodKey = "TrigDecision" );
      declareProperty( "xAODKey", m_xaodKey = "TrigDecision" );
      declareProperty( "CnvTool", m_cnvTool );
   }

   StatusCode TrigDecisionCnvAlg::initialize() {

       // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( " AOD Key: " << m_aodKey );
      ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );

      // Retrieve the converter tool:
      ATH_CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TrigDecisionCnvAlg::execute() {

      // Retrieve the AOD object:
      const TrigDec::TrigDecision* aod = 0;
      ATH_CHECK( evtStore()->retrieve( aod, m_aodKey ) );

      // Create the xAOD object and its auxiliary store:
      xAOD::TrigDecisionAuxInfo* aux = new xAOD::TrigDecisionAuxInfo();
      xAOD::TrigDecision* xaod = new xAOD::TrigDecision();
      xaod->setStore( aux );

      // Fill the xAOD object:
      ATH_CHECK( m_cnvTool->convert( aod, xaod ) );

      // Record the xAOD objects:
      if( evtStore()->contains< xAOD::TrigDecisionAuxInfo >( m_xaodKey +
                                                             "Aux." ) ) {
         ATH_CHECK( evtStore()->overwrite( aux, m_xaodKey + "Aux." ) );
      } else {
         ATH_CHECK( evtStore()->record( aux, m_xaodKey + "Aux." ) );
      }
      if( evtStore()->contains< xAOD::TrigDecision >( m_xaodKey ) ) {
         ATH_CHECK( evtStore()->overwrite( xaod, m_xaodKey ) );
      } else {
         ATH_CHECK( evtStore()->record( xaod, m_xaodKey ) );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
