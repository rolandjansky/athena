/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "TrigL2CaloHypoAlgMT.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"


using namespace TrigCompositeUtils;

TrigL2CaloHypoAlgMT::TrigL2CaloHypoAlgMT( const std::string& name, 
				      ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}

TrigL2CaloHypoAlgMT::~TrigL2CaloHypoAlgMT() {}

StatusCode TrigL2CaloHypoAlgMT::initialize() {
  ATH_MSG_INFO ( "Initializing " << name() << "..." );

  
  CHECK( m_hypoTools.retrieve() );
  
  CHECK( m_clustersKey.initialize() );
  renounce( m_clustersKey );// clusters are made in views, so they are not in the EvtStore: hide them

  return StatusCode::SUCCESS;
}

StatusCode TrigL2CaloHypoAlgMT::finalize() {   
  return StatusCode::SUCCESS;
}

  /*
OLD
    ITC -> get 1 RoI/TC
    loop over views
       make one TC per view
       get RoIs of view - from RoI input
       get clusters of view -from View input
       map the roi to a decision - from input decisions
       create new decision with one cluster, one roi, one view

NEW
    loop over ITC
       get RoI and view of TC
       get cluster of that view
       create new decision with one cluster, one roi, one view

   */

StatusCode TrigL2CaloHypoAlgMT::execute_r( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  if( not previousDecisionsHandle.isValid() ) {//implicit
    ATH_MSG_DEBUG( "No implicit RH for previous decisions "<<  decisionInput().key()<<": is this expected?" );
    return StatusCode::SUCCESS;      
  }
  
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" implicit ReadHandles for previous decisions");


  // new decisions
  auto decisions = std::make_unique<DecisionContainer>();
  auto aux = std::make_unique<DecisionAuxContainer>();
  decisions->setStore( aux.get() );

  // input for decision
  std::vector<ITrigL2CaloHypoTool::ClusterInfo> toolInput;

  // loop over previous decisions
  size_t counter=0;
  for ( auto previousDecision: *previousDecisionsHandle ) {
    //get RoI
    auto roiEL = previousDecision->objectLink<TrigRoiDescriptorCollection>( "initialRoI" );
    CHECK( roiEL.isValid() );
    const TrigRoiDescriptor* roi = *roiEL;

    // get View
    auto viewEL = previousDecision->objectLink<std::vector<SG::View*>>( "view" );
    CHECK( viewEL.isValid() );
    const SG::View* view_const = *viewEL;
    SG::View* view = const_cast<SG::View*>(view_const); // CHECK THIS!

    // get clusters of that view
    auto clusterHandle =  SG::makeHandle( m_clustersKey, context );
    CHECK( clusterHandle.setProxyDict( view ) );
    ATH_MSG_DEBUG ( "Cluster handle size: " << clusterHandle->size() << "..." );

    // create new decision
    auto d = newDecisionIn( decisions.get() );


    toolInput.emplace_back( d, roi, clusterHandle.cptr()->at(0), previousDecision );

     {
      auto el = ElementLink<xAOD::TrigEMClusterContainer>( view->name()+"_"+clusterHandle.key(), 0 ); // 0 because there is only one obj in per-view collection
      CHECK( el.isValid() );
      d->setObjectLink( "feature",  el );
    }
     d->setObjectLink( "roi", roiEL );
     d->setObjectLink( "view", viewEL );
     TrigCompositeUtils::linkToPrevious( d, decisionInput().key(), counter );
     ATH_MSG_DEBUG( "Added view, roi, cluster, previous decision to new decision "<<counter <<" for view "<<view->name()  );
     counter++;

  }

  ATH_MSG_DEBUG( "Found "<<toolInput.size()<<" inputs to tools");

   
  for ( auto& tool: m_hypoTools ) {
    CHECK( tool->decide( toolInput ) );
  }
 
  {// make output handle and debug
    auto outputHandle = SG::makeHandle(decisionOutput(), context);
    CHECK( outputHandle.record( std::move( decisions ), std::move( aux ) ) );
    ATH_MSG_DEBUG ( "Exit with "<<outputHandle->size() <<" decisions");
    TrigCompositeUtils::DecisionIDContainer allPassingIDs;
    if ( outputHandle.isValid() ) {
      for ( auto decisionObject: *outputHandle )  {
	TrigCompositeUtils::decisionIDs( decisionObject, allPassingIDs );
      }
      for ( TrigCompositeUtils::DecisionID id : allPassingIDs ) {
	ATH_MSG_DEBUG( " +++ " << HLT::Identifier( id ) );
      }
    }
  }

  return StatusCode::SUCCESS;
}
