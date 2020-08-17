/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "Gaudi/Property.h"
#include "TrigEgammaFastPhotonHypoAlgMT.h"
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

TrigEgammaFastPhotonHypoAlgMT::TrigEgammaFastPhotonHypoAlgMT( const std::string& name, 
				      ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}


StatusCode TrigEgammaFastPhotonHypoAlgMT::initialize() {
  ATH_CHECK( m_hypoTools.retrieve() );
  
  ATH_CHECK( m_photonsKey.initialize() );
  renounce( m_photonsKey );// clusters are made in views, so they are not in the EvtStore: hide them

  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaFastPhotonHypoAlgMT::execute( const EventContext& context ) const {
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  ATH_CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");

  // map between cluster pointer and index 
  
  std::map<const xAOD::TrigEMCluster*, size_t> clusterToIndexMap;
  size_t clusterCounter = 0;
  for ( auto previousDecision : *previousDecisionsHandle){
    auto clusterELInfo = TrigCompositeUtils::findLink<xAOD::TrigEMClusterContainer>( previousDecision, "feature" );
    
    if( not clusterELInfo.isValid() ) {
      ATH_MSG_ERROR("Can not obtain the link to Cluster");
      ATH_MSG_ERROR( TrigCompositeUtils::dump( previousDecision, [](const xAOD::TrigComposite* tc){
	    return tc->name() + " " + (tc->object<xAOD::TrigEMCluster>("feature") == 0 ? "has no cluster": "has cluster");
	  }) );
      return StatusCode::FAILURE;
    }
    const xAOD::TrigEMCluster* cluster = *(clusterELInfo.link);
    clusterToIndexMap.insert( std::make_pair( cluster, clusterCounter ) );
    clusterCounter++;
  }
  ATH_MSG_DEBUG( "Cluster ptr to decision map has size " << clusterToIndexMap.size() );

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();

  std::vector<TrigEgammaFastPhotonHypoTool::PhotonInfo> hypoToolInput;
 
  for ( auto previousDecision: *previousDecisionsHandle ) {
    //previousDecision->objectLink< ViewContainer >( "view" );
    const auto viewEL = previousDecision->objectLink<ViewContainer>( viewString() );
      
    ATH_CHECK( viewEL.isValid() );
    
    // get electron from that view:
    size_t photonCounter = 0;
    auto photonsHandle = ViewHelper::makeHandle( *viewEL, m_photonsKey, context );  

    ATH_CHECK( photonsHandle.isValid() );
    ATH_MSG_DEBUG ( "electron handle size: " << photonsHandle->size() << "..." );

    for ( auto photonIter = photonsHandle->begin(); photonIter != photonsHandle->end(); ++photonIter, photonCounter++ ) {
      auto d = newDecisionIn( decisions, name() );
      d->setObjectLink( featureString(), ViewHelper::makeLink<xAOD::TrigPhotonContainer>( *viewEL, photonsHandle, photonCounter ) );
      
      auto clusterPtr = (*photonIter)->emCluster();
      ATH_CHECK( clusterPtr != nullptr );

      // now find matching cluster 
      // could use geometric matching but in fact the cluster owned by the decision object and the cluster owned by the photon should be the same
      // since we have a map made in advance we can make use of the index lookup w/o the need for additional loop 
      auto origCluster = clusterToIndexMap.find( clusterPtr );
      ATH_CHECK( origCluster != clusterToIndexMap.end() );
      linkToPrevious( d, decisionInput().key(), origCluster->second );
      
      // now we have DecisionObject ready to be passed to hypo tool. it has link to photon, 
      // and decisions on clusters
      // we shall avoid calling the tools for chains which were already rejected on certain cluster, but this is left to hypo tools
      DecisionIDContainer clusterDecisionIDs;
      decisionIDs( previousDecisionsHandle->at( origCluster->second ), clusterDecisionIDs );
      
      hypoToolInput.emplace_back( TrigEgammaFastPhotonHypoTool::PhotonInfo{ d, *photonIter,  origCluster->first, clusterDecisionIDs } );
    }
  }

  for ( auto & tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( hypoToolInput ) );    
  } 

  
  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

  return StatusCode::SUCCESS;
}