/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include <map>
#include "GaudiKernel/Property.h"
#include "TrigL2ElectronHypoAlg.h"

using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionAuxContainer;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::decisionIDs;
using TrigCompositeUtils::newDecisionIn;
using TrigCompositeUtils::linkToPrevious;


TrigL2ElectronHypoAlg::TrigL2ElectronHypoAlg( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator ) {}

TrigL2ElectronHypoAlg::~TrigL2ElectronHypoAlg() {}

StatusCode TrigL2ElectronHypoAlg::initialize() {
  ATH_MSG_INFO ( "Initializing " << name() << "..." );
  ATH_CHECK( m_hypoTools.retrieve() );
  
  ATH_CHECK( m_views.initialize() );
  ATH_CHECK( m_decisionsKey.initialize() );
  ATH_CHECK( m_clusterDecisionsKey.initialize() );
  
  renounce( m_electronsKey );  
  ATH_CHECK( m_electronsKey.initialize() );


  return StatusCode::SUCCESS;
}


StatusCode TrigL2ElectronHypoAlg::execute_r( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  
  // prepare decisions container and link back to the clusters, and decision on clusters
  auto decisions = std::make_unique<DecisionContainer>();
  auto aux = std::make_unique<DecisionAuxContainer>();
  decisions->setStore( aux.get() );

  // // extract mapping of cluster pointer to an index in the cluster decision collection
  auto clusterDecisions = SG::makeHandle( m_clusterDecisionsKey, context );
  std::map<const xAOD::TrigEMCluster*, size_t> clusterToIndexMap;
  size_t clusterCounter = 0;
  for ( auto clusterDecisionIter = clusterDecisions->begin(); 
  	clusterDecisionIter != clusterDecisions->end(); 
  	++clusterDecisionIter, ++clusterCounter ) {

    ATH_CHECK( ( *clusterDecisionIter )->hasObjectLink( "feature" ) );
    const xAOD::TrigEMCluster* cluster = ( *clusterDecisionIter )->object<xAOD::TrigEMCluster>( "feature" );
    //    ATH_CHECK( clusterLink.isValid() );    
    //    auto cluster = clusterLink.getDataPtr();
    clusterToIndexMap.insert( std::make_pair( cluster, clusterCounter ) );
  }
  ATH_MSG_DEBUG( "Cluster ptr to decision map has size " << clusterToIndexMap.size() );

  // prepare imput for tools
  std::vector<TrigL2ElectronHypoTool::ElectronInfo> hypoToolInput;
  
  auto viewsHandle = SG::makeHandle( m_views, context );
  for ( auto view: *viewsHandle ) {
    size_t electronCounter = 0;
    auto electronsHandle = SG::makeHandle( m_electronsKey, context );  
    CHECK( electronsHandle.setProxyDict( view ) );
    CHECK( electronsHandle.isValid() );

    for ( auto electronIter = electronsHandle->begin(); electronIter != electronsHandle->end(); ++electronIter, electronCounter++ ) {
      auto d = newDecisionIn( decisions.get() );
      d->setObjectLink( "feature", ElementLink<xAOD::TrigElectronContainer>( view->name()+"_"+m_electronsKey.key(), electronCounter ) );
      
      auto clusterPtr = (*electronIter)->emCluster();
      CHECK( clusterPtr != nullptr );

      // now find matching cluster 
      // could use geometric matching but in fact the cluster owned by the decision object and the cluster owned by the electron should be the same
      // since we have a map made in advance we can make use of the index lookup w/o the need for additional loop 
      auto origCluster = clusterToIndexMap.find( clusterPtr );
      ATH_CHECK( origCluster != clusterToIndexMap.end() );
      linkToPrevious( d, m_clusterDecisionsKey.key(), origCluster->second );
      
      // now we have DecisionObject ready to be passed to hypo tool. it has link to electron, 
      // and decisions on clusters
      // we shall avoid calling the tools for chains which were already rejected on certain cluster, but this is left to hypo tools
      DecisionIDContainer clusterDecisionIDs;
      decisionIDs( clusterDecisions->at( origCluster->second ), clusterDecisionIDs );
      
      hypoToolInput.emplace_back( TrigL2ElectronHypoTool::ElectronInfo{ d, *electronIter,  origCluster->first, clusterDecisionIDs } );
    }
  }

  for ( auto & tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( hypoToolInput ) );
    
  } 

  {
    auto handle =  SG::makeHandle( m_decisionsKey, context );
    CHECK( handle.record( std::move( decisions ), std::move( aux ) ) );
  }

  return StatusCode::SUCCESS;
}


