/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$ //

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigMuonEvent/IsoMuonFeatureContainer.h"
#include "TrigMuonEvent/IsoMuonFeature.h"
#include "xAODTrigMuon/L2IsoMuonContainer.h"
#include "xAODTrigMuon/L2IsoMuonAuxContainer.h"

// Local include(s):
#include "IsoMuonFeatureCnvAlg.h"


namespace xAODMaker {

    IsoMuonFeatureCnvAlg::IsoMuonFeatureCnvAlg( const std::string& name,
                                       ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc ),
     m_cnvTool( "xAODMaker::IsoMuonFeatureContainerCnvTool/IsoMuonFeatureContainerCnvTool", this ) {
        
        declareProperty( "AODKey", m_aodKey = "IsoMuonFeature" );
        declareProperty( "xAODKey", m_xaodKey = "L2IsoMuon" );
        declareProperty( "CnvTool", m_cnvTool );
    }

    
    StatusCode IsoMuonFeatureCnvAlg::initialize() {
        
        // Greet the user:
        ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
        ATH_MSG_INFO( " AOD Key: " << m_aodKey );
        ATH_MSG_INFO( "xAOD Key: " << m_xaodKey );
        
        // Retrieve the converter tool:
        CHECK( m_cnvTool.retrieve() );
        
        // Return gracefully:
        return StatusCode::SUCCESS;
    }

    StatusCode IsoMuonFeatureCnvAlg::execute() {
        
        // Retrieve the AOD container:
        const IsoMuonFeatureContainer* aod = nullptr;
        if (evtStore()->retrieve( aod, m_aodKey ).isFailure()) {
            ATH_MSG_WARNING("Requesting in Alg: IsoMuonFeatureCnvAlg to convert AOD Key: " << m_aodKey <<
                            " into xAOD " << m_xaodKey << ". Input container not found or incorrect type");
            return StatusCode::SUCCESS;
        }
        
        xAOD::L2IsoMuonContainer* xaod = new xAOD::L2IsoMuonContainer();
        xAOD::L2IsoMuonAuxContainer* aux = new xAOD::L2IsoMuonAuxContainer();
        xaod->setStore( aux );
        
        CHECK( m_cnvTool->convert(aod, xaod) );

        
        // Record the xAOD containers:
        CHECK( evtStore()->record( aux, m_xaodKey + "Aux" ) );
        CHECK( evtStore()->record( xaod, m_xaodKey ) );
        
        // Return gracefully:
        return StatusCode::SUCCESS;
    }
    
} // namespace xAODMaker
