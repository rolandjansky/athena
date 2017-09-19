/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "MergeViews.h"

MergeViews::MergeViews(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm( name, pSvcLocator ) {}

StatusCode MergeViews::initialize() {
  CHECK( m_viewsKey.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode MergeViews::execute_r( const EventContext& context ) const {
  auto viewsHandle = SG::makeHandle( m_viewsKey, context );
  const std::vector<SG::View*>& viewsRef = *viewsHandle;

  for ( auto & tool : m_mergingTools ) {
    CHECK( tool->merge( context, viewsRef ) );
  }

  return StatusCode::SUCCESS;
}
