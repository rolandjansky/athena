/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigNavigationCnvAlg.h"

using namespace xAODMaker;

TrigNavigationCnvAlg::TrigNavigationCnvAlg( const std::string& name,
                                            ISvcLocator* svcLoc )
  : AthReentrantAlgorithm( name, svcLoc ) {
}

StatusCode TrigNavigationCnvAlg::initialize() {
  ATH_MSG_DEBUG( " AOD Key: " << m_aodKey );
  ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );
  
  CHECK( m_aodKey.initialize() );
  CHECK( m_xaodKey.initialize() );

  CHECK( m_cnvTool.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode TrigNavigationCnvAlg::execute(const EventContext& ctx) const {
  auto aod = SG::makeHandle(m_aodKey, ctx);
  ATH_CHECK( aod.isValid() );

  std::unique_ptr<xAOD::TrigNavigationAuxInfo> aux = std::make_unique<xAOD::TrigNavigationAuxInfo>();
  std::unique_ptr<xAOD::TrigNavigation> xaod = std::make_unique<xAOD::TrigNavigation>();
  xaod->setStore(aux.get());

  ATH_CHECK( m_cnvTool->convert( aod.cptr(), xaod.get() ) );
  ATH_CHECK( SG::makeHandle(m_xaodKey, ctx).record( std::move(xaod), std::move(aux)) );

  return StatusCode::SUCCESS;
}

