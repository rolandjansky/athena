/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "TrigL2PhotonHypoAlgMT.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "TrigL2PhotonHypoTool.h"


using namespace TrigCompositeUtils;

TrigL2PhotonHypoAlgMT::TrigL2PhotonHypoAlgMT( const std::string& name, 
				      ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}

TrigL2PhotonHypoAlgMT::~TrigL2PhotonHypoAlgMT() {}

StatusCode TrigL2PhotonHypoAlgMT::initialize() {
  ATH_MSG_INFO ( "Initializing " << name() << "..." );

  
  CHECK( m_hypoTools.retrieve() );
  
  CHECK( m_photonsKey.initialize() );
  renounce( m_photonsKey );// clusters are made in views, so they are not in the EvtStore: hide them

  return StatusCode::SUCCESS;
}

StatusCode TrigL2PhotonHypoAlgMT::finalize() {   
  return StatusCode::SUCCESS;
}

  
StatusCode TrigL2PhotonHypoAlgMT::execute_r( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  if( not previousDecisionsHandle.isValid() ) {//implicit
    ATH_MSG_DEBUG( "No implicit RH for previous decisions "<<  decisionInput().key()<<": is this expected?" );
    return StatusCode::SUCCESS;      
  }
  
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" implicit ReadHandles for previous decisions");

  // map between cluster pointer and index 
  
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

  auto decisions = std::make_unique<DecisionContainer>();
  auto aux = std::make_unique<DecisionAuxContainer>();
  decisions->setStore( aux.get() );


  std::vector<TrigL2PhotonHypoTool::PhotonInfo> hypoToolInput;
 
  for ( auto previousDecision: *previousDecisionsHandle ) {
    // get View
    auto viewEL = previousDecision->objectLink<std::vector<SG::View*>>( "view" );
    CHECK( viewEL.isValid() );
    const SG::View* view_const = *viewEL;
    SG::View* view = const_cast<SG::View*>(view_const); // CHECK THIS!

    // get electron from that view:
    size_t photonCounter = 0;
    auto photonsHandle = SG::makeHandle( m_photonsKey, context );  
    CHECK( photonsHandle.setProxyDict( view ) );
    CHECK( photonsHandle.isValid() );
    ATH_MSG_DEBUG ( "electron handle size: " << photonsHandle->size() << "..." );

    for ( auto photonIter = photonsHandle->begin(); photonIter != photonsHandle->end(); ++photonIter, photonCounter++ ) {
      auto d = newDecisionIn( decisions.get() );
      d->setObjectLink( "feature", ElementLink<xAOD::TrigPhotonContainer>( view->name()+"_"+m_photonsKey.key(), photonCounter ) );
      
      auto clusterPtr = (*photonIter)->emCluster();
      CHECK( clusterPtr != nullptr );

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
      
      hypoToolInput.emplace_back( TrigL2PhotonHypoTool::PhotonInfo{ d, *photonIter,  origCluster->first, clusterDecisionIDs } );
    }
  }

  for ( auto & tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( hypoToolInput ) );    
  } 

  auto outputHandle = SG::makeHandle(decisionOutput(), context);
  CHECK( outputHandle.record(std::move(decisions), std::move(aux) ) );
  
  ATH_MSG_DEBUG( "Exiting with "<< outputHandle->size() <<" decisions");
  //debug
  for (auto outh: *outputHandle){
    TrigCompositeUtils::DecisionIDContainer objDecisions;      
    TrigCompositeUtils::decisionIDs( outh, objDecisions );
    
    ATH_MSG_DEBUG("Number of positive decisions for this input: " << objDecisions.size() );
    
    for ( TrigCompositeUtils::DecisionID id : objDecisions ) {
      ATH_MSG_DEBUG( " --- found new decision " << HLT::Identifier( id ) );
    }  
    
  }

  return StatusCode::SUCCESS;
}


bool TrigL2PhotonHypoAlgMT::recursivelyFindFeature( const TrigCompositeUtils::Decision* start, ElementLink<xAOD::TrigEMClusterContainer>& clusterlink) const{
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

