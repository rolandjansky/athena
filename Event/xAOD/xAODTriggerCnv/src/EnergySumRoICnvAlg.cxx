/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EnergySumRoICnvAlg.cxx 575028 2013-12-11 14:30:06Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODTrigger/EnergySumRoIAuxInfo.h"

// Local include(s):
#include "EnergySumRoICnvAlg.h"

namespace xAODMaker {

   EnergySumRoICnvAlg::EnergySumRoICnvAlg( const std::string& name,
                                           ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::EnergySumRoICnvTool/EnergySumRoICnvTool",
                   this ) {

      declareProperty( "AODKey", m_aodKey = "LVL1_ROI" );
      declareProperty( "xAODKey", m_xaodKey = "LVL1EnergySumRoI" );
      declareProperty( "CnvTool", m_cnvTool );
   }

   StatusCode EnergySumRoICnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( " AOD Key: " << m_aodKey );
      ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );

      // Retrieve the converter tool:
      CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode EnergySumRoICnvAlg::execute() {

      // Retrieve the AOD container:
      const LVL1_ROI* aod = nullptr;
      CHECK( evtStore()->retrieve( aod, m_aodKey ) );

      // Create the xAOD container and its auxiliary store:
      xAOD::EnergySumRoIAuxInfo* aux = new xAOD::EnergySumRoIAuxInfo();
      xAOD::EnergySumRoI* xaod = new xAOD::EnergySumRoI();
      xaod->setStore( aux );

      // Fill the xAOD container:
      CHECK( m_cnvTool->convert( aod, xaod ) );

      // Record the xAOD containers:
      CHECK( evtStore()->record( aux, m_xaodKey + "Aux." ) );
      CHECK( evtStore()->record( xaod, m_xaodKey ) );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
