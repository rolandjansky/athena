/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecisionCnvAlg.cxx 688265 2015-08-08 16:31:45Z stelzer $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigDecisionEvent/TrigDecision.h"
#include "xAODTrigger/TrigDecision.h"
#include "xAODTrigger/TrigDecisionAuxInfo.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"

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
      CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TrigDecisionCnvAlg::execute() {

      // Retrieve the AOD object:
      const TrigDec::TrigDecision* aod = 0;
      CHECK( evtStore()->retrieve( aod, m_aodKey ) );

      // trigger Info
      const EventInfo * evtInfo ( nullptr );
      CHECK( evtStore()->retrieve( evtInfo ) );
      const TriggerInfo * trigInfo = evtInfo ? evtInfo->trigger_info() : nullptr ;

      // Create the xAOD object and its auxiliary store:
      xAOD::TrigDecisionAuxInfo* aux = new xAOD::TrigDecisionAuxInfo();
      xAOD::TrigDecision* xaod = new xAOD::TrigDecision();
      xaod->setStore( aux );

      // Fill the xAOD object:
      CHECK( m_cnvTool->convert( aod, xaod, trigInfo ) );

      // Record the xAOD objects:
      CHECK( evtStore()->record( aux, m_xaodKey + "Aux." ) );
      CHECK( evtStore()->record( xaod, m_xaodKey ) );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
