/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMJetHitsCnvAlg.cxx 576052 2013-12-18 09:13:50Z morrisj $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigT1CaloEvent/CMMJetHitsCollection.h"
#include "xAODTrigL1Calo/CMMJetHitsContainer.h"
#include "xAODTrigL1Calo/CMMJetHitsAuxContainer.h"

// Local include(s):
#include "CMMJetHitsCnvAlg.h"

namespace xAODMaker {

   CMMJetHitsCnvAlg::CMMJetHitsCnvAlg( const std::string& name,
                                       ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::CMMJetHitsCnvTool/CMMJetHitsCnvTool", this ) {

      declareProperty( "ESDKey" , m_esdKey  = "CMMJetHits" );
      declareProperty( "xAODKey", m_xaodKey = "CMMJetHits" );
      declareProperty( "CnvTool", m_cnvTool );
   }

   StatusCode CMMJetHitsCnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( " ESD Key: " << m_esdKey );
      ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );

      // Retrieve the converter tool:
      CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode CMMJetHitsCnvAlg::execute() {
      
      // Retrieve the ESD container:
      const CMMJetHitsCollection* esd = nullptr;
      CHECK( evtStore()->retrieve( esd, m_esdKey ) );
      
      // Create the xAOD container and its auxiliary store:
      xAOD::CMMJetHitsAuxContainer* aux = new xAOD::CMMJetHitsAuxContainer();
      xAOD::CMMJetHitsContainer* xaod = new xAOD::CMMJetHitsContainer();
      xaod->setStore( aux );
      
      // Fill the xAOD container:
      CHECK( m_cnvTool->convert( esd, xaod ) );     
      
      // Record the xAOD containers:
      CHECK( evtStore()->record( aux, m_xaodKey + "Aux." ) );
      CHECK( evtStore()->record( xaod, m_xaodKey ) );

      // Return gracefully:
      return StatusCode::SUCCESS;     
   }

} // namespace xAODMaker
