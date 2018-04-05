/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include <map>
#include "GaudiKernel/Property.h"
#include "TestTrigL2ElectronHypoAlg.h"



using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionAuxContainer;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::decisionIDs;
using TrigCompositeUtils::newDecisionIn;
using TrigCompositeUtils::linkToPrevious;


TestTrigL2ElectronHypoAlg::TestTrigL2ElectronHypoAlg( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator ) {}

TestTrigL2ElectronHypoAlg::~TestTrigL2ElectronHypoAlg() {}

StatusCode TestTrigL2ElectronHypoAlg::initialize() {
  ATH_MSG_INFO ( "Initializing " << name() << "..." );

  CHECK( m_hypoTools.retrieve() );
  
  CHECK( m_electronsKey.initialize() );
  if (  m_runInView)   renounce( m_electronsKey );// clusters are made in views, so they are not in the EvtStore: hide them

  CHECK( m_previousDecisionsKey.initialize() );
  renounce(m_previousDecisionsKey);

  CHECK( m_decisionsKey.initialize() );

 
  return StatusCode::SUCCESS;
}


StatusCode TestTrigL2ElectronHypoAlg::execute_r( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  auto previousDecisionsHandle = SG::makeHandle( m_previousDecisionsKey, context );
  if( not previousDecisionsHandle.isValid() ) {//implicit
    ATH_MSG_DEBUG( "No implicit RH for previous decisions "<<   m_previousDecisionsKey.key()<<": is this expected?" );
    return StatusCode::SUCCESS;      
  }

  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" implicit ReadHandles for previous decisions");
  
  
  auto decisions = std::make_unique<DecisionContainer>();
  auto aux = std::make_unique<DecisionAuxContainer>();
  decisions->setStore( aux.get() );

  // // extract mapping of cluster pointer to an index in the cluster decision collection

  // prepare decisions container and link back to the clusters, and decision on clusters  
  std::map<const xAOD::TrigEMCluster*, size_t> clusterToIndexMap;
  size_t clusterCounter = 0;
  for ( auto previousDecision : *previousDecisionsHandle){
    ElementLink<xAOD::TrigEMClusterContainer> clusterLink;
    recursivelyFindFeature(previousDecision, clusterLink);
    ATH_CHECK( clusterLink.isValid() );    
    const xAOD::TrigEMCluster* cluster = *clusterLink;
    clusterToIndexMap.insert( std::make_pair( cluster, clusterCounter ) );
    clusterCounter++;
  }
  ATH_MSG_DEBUG( "Cluster ptr to decision map has size " << clusterToIndexMap.size() );

  // prepare imput for tools
  std::vector<TrigL2ElectronHypoTool::ElectronInfo> hypoToolInput;
 
  for ( auto previousDecision: *previousDecisionsHandle ) {
      // get View
    auto viewEL = previousDecision->objectLink<std::vector<SG::View*>>( "view" );
    CHECK( viewEL.isValid() );
    const SG::View* view_const = *viewEL;
    SG::View* view = const_cast<SG::View*>(view_const); // CHECK THIS!

    // get electron from that view:
    size_t electronCounter = 0;
    auto electronsHandle = SG::makeHandle( m_electronsKey, context );  
    CHECK( electronsHandle.setProxyDict( view ) );
    CHECK( electronsHandle.isValid() );
    ATH_MSG_DEBUG ( "electron handle size: " << electronsHandle->size() << "..." );

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
      linkToPrevious( d, m_previousDecisionsKey.key(), origCluster->second );
      
      // now we have DecisionObject ready to be passed to hypo tool. it has link to electron, 
      // and decisions on clusters
      // we shall avoid calling the tools for chains which were already rejected on certain cluster, but this is left to hypo tools
      DecisionIDContainer clusterDecisionIDs;
      decisionIDs( previousDecisionsHandle->at( origCluster->second ), clusterDecisionIDs );
      
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




bool TestTrigL2ElectronHypoAlg::recursivelyFindFeature( const TrigCompositeUtils::Decision* start, ElementLink<xAOD::TrigEMClusterContainer>& clusterlink) const{
    //recursively find in the seeds
    if ( start->hasObjectLink( "feature" ) ) {
      clusterlink=start->objectLink<xAOD::TrigEMClusterContainer>( "feature" );
      return true;
    }
    if  (TrigCompositeUtils::hasLinkToPrevious(start) ){
      auto thelinkToPrevious =TrigCompositeUtils::linkToPrevious( start);      
      return recursivelyFindFeature( *thelinkToPrevious, clusterlink);
    }
    return false;
  }



