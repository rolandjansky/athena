/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include <map>
#include "Gaudi/Property.h"
#include "TrigEgammaFastElectronHypoAlgMT.h"
#include "AthViews/ViewHelper.h"


using TrigCompositeUtils::createAndStore; 
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionAuxContainer;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::decisionIDs;
using TrigCompositeUtils::newDecisionIn;
using TrigCompositeUtils::linkToPrevious;
using TrigCompositeUtils::viewString;
using TrigCompositeUtils::featureString;
using TrigCompositeUtils::findLink;
using TrigCompositeUtils::LinkInfo;

TrigEgammaFastElectronHypoAlgMT::TrigEgammaFastElectronHypoAlgMT( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::HypoBase( name, pSvcLocator ) {}


StatusCode TrigEgammaFastElectronHypoAlgMT::initialize() {
  CHECK( m_hypoTools.retrieve() );
  
  CHECK( m_electronsKey.initialize() );
  if (  m_runInView)   renounce( m_electronsKey );// electrons are made in views, so they are not in the EvtStore: hide them

  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaFastElectronHypoAlgMT::execute( const EventContext& context ) const {
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  ATH_CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");
  
  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();

  // // extract mapping of cluster pointer to an index in the cluster decision collection

  // prepare decisions container and link back to the clusters, and decision on clusters  
  std::map<const xAOD::TrigEMCluster*, size_t> clusterToIndexMap;
  size_t clusterCounter = 0;
  for ( auto previousDecision : *previousDecisionsHandle){
    LinkInfo<xAOD::TrigEMClusterContainer> linkInfo = findLink<xAOD::TrigEMClusterContainer>(previousDecision, featureString());
    ElementLink<xAOD::TrigEMClusterContainer> clusterLink = linkInfo.link;

    ATH_CHECK( clusterLink.isValid() );    
    const xAOD::TrigEMCluster* cluster = *clusterLink;
    clusterToIndexMap.insert( std::make_pair( cluster, clusterCounter ) );
    clusterCounter++;
  }
  ATH_MSG_DEBUG( "Cluster ptr to decision map has size " << clusterToIndexMap.size() );

  // prepare imput for tools
  std::vector<TrigEgammaFastElectronHypoTool::ElectronInfo> hypoToolInput;
 
  for ( auto previousDecision: *previousDecisionsHandle ) {
      // get View
    const auto viewEL = previousDecision->objectLink<ViewContainer>( viewString() );
    CHECK( viewEL.isValid() );

    // get electron from that view:
    size_t electronCounter = 0;
    auto electronsHandle = ViewHelper::makeHandle( *viewEL, m_electronsKey, context );
    ATH_CHECK( electronsHandle.isValid() );
    ATH_MSG_DEBUG ( "electron handle size: " << electronsHandle->size() << "..." );

    for ( auto electronIter = electronsHandle->begin(); electronIter != electronsHandle->end(); ++electronIter, electronCounter++ ) {
      auto d = newDecisionIn( decisions );
      d->setObjectLink( featureString(), ViewHelper::makeLink<xAOD::TrigElectronContainer>( *viewEL, electronsHandle, electronCounter ) );
      
      auto clusterPtr = (*electronIter)->emCluster();
      CHECK( clusterPtr != nullptr );

      // now find matching cluster 
      // could use geometric matching but in fact the cluster owned by the decision object and the cluster owned by the electron should be the same
      // since we have a map made in advance we can make use of the index lookup w/o the need for additional loop 
      auto origCluster = clusterToIndexMap.find( clusterPtr );
      ATH_CHECK( origCluster != clusterToIndexMap.end() );
      linkToPrevious( d, decisionInput().key(), origCluster->second );
      
      // now we have DecisionObject ready to be passed to hypo tool. it has link to electron, 
      // and decisions on clusters
      // we shall avoid calling the tools for chains which were already rejected on certain cluster, but this is left to hypo tools
      DecisionIDContainer clusterDecisionIDs;
      decisionIDs( previousDecisionsHandle->at( origCluster->second ), clusterDecisionIDs );
      
      hypoToolInput.emplace_back( TrigEgammaFastElectronHypoTool::ElectronInfo{ d, *electronIter,  origCluster->first, clusterDecisionIDs } );
    }
  }

  for ( auto & tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( hypoToolInput ) );    
  } 

  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

  return StatusCode::SUCCESS;
}
