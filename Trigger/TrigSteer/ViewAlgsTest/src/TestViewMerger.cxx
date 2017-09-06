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
  : AthAlgorithm( name, pSvcLocator ),
//  : AthReentrantAlgorithm( name, pSvcLocator ),
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


//StatusCode TestViewMerger::execute_r( const EventContext& ctx ) const {
StatusCode TestViewMerger::execute(){
  //  auto viewsHandle = SG::makeHandle( m_viewsKey, ctx );
  auto viewsHandle = SG::makeHandle( m_viewsKey );
  if ( not viewsHandle.isValid() ) { // have to check because this is not checked at compile time
    ATH_MSG_ERROR( "Invalid views key " << m_viewsKey.key() );
    return StatusCode::FAILURE;
  }  

  auto outputClusterContainer = std::make_unique< TestClusterContainer >();
  auto outputClusterContainerAux = std::make_unique< TestClusterAuxContainer>();
  outputClusterContainer->setStore( outputClusterContainerAux.get() );


  for ( auto view : *viewsHandle ) {
    ATH_MSG_DEBUG( "Reading fromt he view: " << view->impl()->name() );
    auto context = EventContext();
    context.setExtension(  Atlas::ExtendedEventContext( view ) );
    auto clusterInViewHandle = SG::makeHandle( m_clustersViewInputKey, context );
    CHECK ( clusterInViewHandle.isValid() );
    ATH_MSG_DEBUG( "Found clusters"  );

    for ( auto c : *clusterInViewHandle.cptr() ) {
      using namespace TestEDM;
      ATH_MSG_DEBUG( "Et " << getClusterEt( c ) << " eta " << getClusterEta( c ) << " phi " << getClusterPhi( c ) );
      auto tosave = new TestCluster();
      outputClusterContainer->push_back( tosave ) ;
      copyCluster( c, tosave );
    }
  }
  {
    auto h = SG::makeHandle( m_clustersOutputKey );
    CHECK( h.record( std::move(outputClusterContainer), std::move(outputClusterContainerAux) ) );
  }


  return StatusCode::SUCCESS;
}

