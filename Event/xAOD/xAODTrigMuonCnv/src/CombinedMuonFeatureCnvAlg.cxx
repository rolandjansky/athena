/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$ //

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuonAuxContainer.h"

// Local include(s):
#include "CombinedMuonFeatureCnvAlg.h"


namespace xAODMaker {

    CombinedMuonFeatureCnvAlg::CombinedMuonFeatureCnvAlg( const std::string& name,
                                       ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc ),
     m_cnvTool( "xAODMaker::CombinedMuonFeatureContainerCnvTool/CombinedMuonFeatureContainerCnvTool", this ) {
        
        declareProperty( "AODKey", m_aodKey = "CombinedMuonFeature" );
        declareProperty( "xAODKey", m_xaodKey = "L2CombinedMuon" );
        declareProperty( "CnvTool", m_cnvTool );
    }

    
    StatusCode CombinedMuonFeatureCnvAlg::initialize() {
        
        // Greet the user:
        ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
        ATH_MSG_INFO( " AOD Key: " << m_aodKey );
        ATH_MSG_INFO( "xAOD Key: " << m_xaodKey );
        
        // Retrieve the converter tool:
        CHECK( m_cnvTool.retrieve() );
        
        // Return gracefully:
        return StatusCode::SUCCESS;
    }

    StatusCode CombinedMuonFeatureCnvAlg::execute() {
        
        // Retrieve the AOD container:
        const CombinedMuonFeatureContainer* aod = nullptr;
        if (evtStore()->retrieve( aod, m_aodKey ).isFailure()) {
            ATH_MSG_WARNING("Requesting in Alg: CombinedMuonFeatureCnvAlg to convert AOD Key: " << m_aodKey <<
                            " into xAOD " << m_xaodKey << ". Input container not found or incorrect type");
            return StatusCode::SUCCESS;
        }
        
        xAOD::L2CombinedMuonContainer* xaod = new xAOD::L2CombinedMuonContainer();
        xAOD::L2CombinedMuonAuxContainer* aux = new xAOD::L2CombinedMuonAuxContainer();
        xaod->setStore( aux );
        
        CHECK( m_cnvTool->convert(aod, xaod) );

        
        // Record the xAOD containers:
        CHECK( evtStore()->record( aux, m_xaodKey + "Aux" ) );
        CHECK( evtStore()->record( xaod, m_xaodKey ) );
        
        // Return gracefully:
        return StatusCode::SUCCESS;
    }
    
} // namespace xAODMaker
