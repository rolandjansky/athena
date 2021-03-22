/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$ //

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/MuonFeatureDetailsContainer.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigMuon/L2StandAloneMuonAuxContainer.h"

// Local include(s):
#include "MuonFeatureCnvAlg.h"


namespace xAODMaker {

  MuonFeatureCnvAlg::MuonFeatureCnvAlg( const std::string& name, ISvcLocator* svcLoc ) :
    AthAlgorithm( name, svcLoc ),
    m_converterTool( "MuonFeatureCnvTool/MuonFeatureCnvTool") 
  {      
    declareProperty( "AODMuonFeatureContainerName", m_mfName = "HLT" );
    declareProperty( "AODMuonFeatureDetailsContainerName", m_mfdName = "HLT" );
    declareProperty( "xAODL2StandAloneMuonContainerName", m_xaodName = "L2StandAloneMuon" );
    declareProperty( "ConverterTool", m_converterTool );
  }

  StatusCode MuonFeatureCnvAlg::initialize() {
    
    // Greet the user:
    ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
    ATH_MSG_INFO( "AOD MuonFeatureContainer Name: " << m_mfName );
    ATH_MSG_INFO( "AOD MuonFeatureDetailsContainer Name: " << m_mfdName );
    ATH_MSG_INFO( "xAOD L2StandAloneMuonContainer Name: " << m_xaodName );
    ATH_MSG_INFO( "ConverterTool = " << m_converterTool.typeAndName());
    
    // Retrieve the converter tool:
    ATH_CHECK( m_converterTool.retrieve() );
    
    // Return gracefully:
    return StatusCode::SUCCESS;
  }
  
  StatusCode MuonFeatureCnvAlg::execute() {
    
    // Retrieve the AOD container:
    // MuonFeature
    const MuonFeatureContainer* aod_mf = nullptr;
    if (evtStore()->retrieve( aod_mf, m_mfName ).isFailure()) {
      ATH_MSG_WARNING("Requesting in Alg: MuonFeatureCnvAlg to convert AOD MuonFeatureContainer name: " << m_mfName <<
		      " into xAOD " << m_xaodName << ". Input container not found or incorrect type");
      return StatusCode::SUCCESS;
    }

    // MuonFeatureDetails
    const MuonFeatureDetailsContainer* aod_mfd = nullptr;
    if (evtStore()->retrieve( aod_mfd, m_mfName ).isFailure()) {
      ATH_MSG_WARNING("Requesting in Alg: MuonFeatureCnvAlg to convert AOD MuonFeatureDetailsContainer name: " << m_mfdName <<
		      " into xAOD " << m_xaodName << ". Input container not found or incorrect type");
      return StatusCode::SUCCESS;
    }
    
    xAOD::L2StandAloneMuonContainer* xaod = new xAOD::L2StandAloneMuonContainer();
    xAOD::L2StandAloneMuonAuxContainer* aux = new xAOD::L2StandAloneMuonAuxContainer();

    xaod->setStore( aux );
    
    CHECK( m_converterTool->convertMuonFeatureContainer(aod_mf, aod_mfd, xaod) );
    
    // Record the xAOD containers:
    CHECK( evtStore()->record( xaod, m_xaodName ) );
    CHECK( evtStore()->record( aux, m_xaodName + "Aux" ) );
        
    // Return gracefully:
    return StatusCode::SUCCESS;
  }
  
} // namespace xAODMaker
