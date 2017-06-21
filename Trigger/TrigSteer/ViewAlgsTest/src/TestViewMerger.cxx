/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include "CxxUtils/make_unique.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "AthContainers/ConstDataVector.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "AthViews/ViewHelper.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"


#include "./TestViewMerger.h"

TestViewMerger::TestViewMerger( const std::string& name, ISvcLocator* pSvcLocator )
  : AthReentrantAlgorithm( name, pSvcLocator ),
    m_viewsKey( "Views" ),   
    m_clustersViewInputKey( "ClustersViewInput" ),
    m_clustersOutputKey( "ClustersOutput" ) {
   
  declareProperty( "ClustersViewInput", m_clustersViewInputKey );
  declareProperty( "Views", m_viewsKey, "Name of the generated view" );
  declareProperty( "ClustersOutput", m_clustersOutputKey );
}

StatusCode TestViewMerger::initialize() {
  CHECK( m_clustersViewInputKey.initialize() );
  renounce( m_clustersViewInputKey );
  CHECK( m_viewsKey.initialize() );
  CHECK( m_clustersOutputKey.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode TestViewMerger::execute_r( const EventContext& ctx ) const {
  auto viewsHandle = SG::makeHandle( m_viewsKey, ctx );
  if ( not viewsHandle.isValid() ) {
    ATH_MSG_ERROR("Invalid views key " << m_viewsKey.key() );
    return StatusCode::FAILURE;
  }  
  for ( auto view : *viewsHandle ) {
    ATH_MSG_DEBUG( "Reading fromt he view: " << view->impl()->name() );
  }


  // for now we are not outputting
  //  {
  //    auto handle = SG::makeHandle( m_clustersOutputHandle );
  //    CHECK( handle.record( std::move( clustersOutputContainer ), std::move( clustersOutputContainerAux ) ) );
  //  }
  // Store the views for re-use/book-keeping
  //  {
  //    auto handle = SG::makeHandle( m_views );
  //    CHECK( handle.record( std::move( viewVector ) ) );
  //  }

  return StatusCode::SUCCESS;
}

