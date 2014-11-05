/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigSteeringEvent/HLTResult.h"
#include "xAODTrigger/TrigNavigation.h"
#include "xAODTrigger/TrigNavigationAuxInfo.h"

// Local include(s):
#include "TrigNavigationCnvAlg.h"

using namespace xAODMaker;



TrigNavigationCnvAlg::TrigNavigationCnvAlg( const std::string& name,
					    ISvcLocator* svcLoc )
  : AthAlgorithm( name, svcLoc ),
    m_cnvTool( "xAODMaker::TrigNavigationCnvTool/TrigNavigationCnvTool",
	       this ) {
  
  declareProperty( "AODKeys", m_aodKeys = {"HLTResult_HLT", "HLTResult_EF", "HLTResult_L2"}, "Copy content from this HLT results in order, stop wiht the first present" );
  declareProperty( "xAODKey", m_xaodKey = "TrigNavigation" );
  declareProperty( "CnvTool", m_cnvTool );
}

StatusCode TrigNavigationCnvAlg::initialize() {
  ATH_MSG_DEBUG( " AOD Key: " << m_aodKeys );
  ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );
  
  CHECK( m_cnvTool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode TrigNavigationCnvAlg::execute() {
  
  for ( auto key: m_aodKeys ) {
    if ( evtStore()->contains(ClassID_traits<HLT::HLTResult>::ID(), key) ) {
      ATH_MSG_DEBUG("Using the HLTResult#" << key << " other keys: " << m_aodKeys << " absent or skipped " );
      const HLT::HLTResult* aod;
      CHECK( evtStore()->retrieve( aod, key ) );
      xAOD::TrigNavigationAuxInfo* aux = new xAOD::TrigNavigationAuxInfo();
      xAOD::TrigNavigation* xaod = new xAOD::TrigNavigation();
      xaod->setStore(aux);
      CHECK( m_cnvTool->convert( aod, xaod ) );
      CHECK( evtStore()->record( aux, m_xaodKey + "Aux." ) );
      CHECK( evtStore()->record( xaod, m_xaodKey ) );	 
      break;
    }
  }
  
  return StatusCode::SUCCESS;
}

