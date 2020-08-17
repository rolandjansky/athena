/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigNavigationCnvAlg.h"

using namespace xAODMaker;



TrigNavigationCnvAlg::TrigNavigationCnvAlg( const std::string& name,
					    ISvcLocator* svcLoc )
  : AthReentrantAlgorithm( name, svcLoc ),
    m_cnvTool( "xAODMaker::TrigNavigationCnvTool/TrigNavigationCnvTool",
	       this ) {
}

StatusCode TrigNavigationCnvAlg::initialize() {
  ATH_MSG_DEBUG( " AOD Key L2: " << m_aodKeyL2 << " doL2:" << m_doL2 );
  ATH_MSG_DEBUG( " AOD Key EF: " << m_aodKeyEF << " doEF:" << m_doEF );
  ATH_MSG_DEBUG( " AOD Key HLT: " << m_aodKeyHLT << " doHLT:" << m_doHLT );
  ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );
  
  CHECK( m_cnvTool.retrieve() );

  if ((size_t)m_doL2 + (size_t)m_doEF + (size_t)m_doHLT > 1) {
    ATH_MSG_ERROR("Cannot convert more than one level. Asked for doL2:" << m_doL2 << " doEF:" << m_doEF << " doHLT:" << m_doHLT);
    return StatusCode::FAILURE;
  }

  CHECK( m_aodKeyL2.initialize(m_doL2) );
  CHECK( m_aodKeyEF.initialize(m_doEF) );
  CHECK( m_aodKeyHLT.initialize(m_doHLT) );
  CHECK( m_xaodKey.initialize() );
  
  return StatusCode::SUCCESS;
}

StatusCode TrigNavigationCnvAlg::execute(const EventContext& ctx) const {
  const SG::ReadHandleKey<HLT::HLTResult>& key = (m_doL2 ? m_aodKeyL2 : (m_doEF ? m_aodKeyEF : m_aodKeyHLT));
  SG::ReadHandle<HLT::HLTResult> hltRH{key, ctx};
  CHECK( hltRH.isValid() );
  const HLT::HLTResult* aod = hltRH.cptr();
  std::unique_ptr<xAOD::TrigNavigationAuxInfo> aux = std::make_unique<xAOD::TrigNavigationAuxInfo>();
  std::unique_ptr<xAOD::TrigNavigation> xaod = std::make_unique<xAOD::TrigNavigation>();
  xaod->setStore(aux.get());
  CHECK( m_cnvTool->convert( aod, xaod.get() ) );
  SG::WriteHandle<xAOD::TrigNavigation> navWH{m_xaodKey, ctx};
  CHECK( navWH.record( std::move(xaod), std::move(aux)) );  
  return StatusCode::SUCCESS;
}

