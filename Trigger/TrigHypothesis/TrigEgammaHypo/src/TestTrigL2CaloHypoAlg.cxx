/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "TestTrigL2CaloHypoAlg.h"

using namespace TrigCompositeUtils;

TestTrigL2CaloHypoAlg::TestTrigL2CaloHypoAlg( const std::string& name, 
				      ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator ) {}

TestTrigL2CaloHypoAlg::~TestTrigL2CaloHypoAlg() {}

StatusCode TestTrigL2CaloHypoAlg::initialize() {
  ATH_MSG_INFO ( "Initializing " << name() << "..." );
  ATH_MSG_DEBUG( "RunInView  = " << ( m_runInView==true ? "True" : "False" ) );
  
  CHECK( m_hypoTools.retrieve() );
  
  CHECK( m_clustersKey.initialize() );
  if (  m_runInView)   renounce( m_clustersKey );// clusters are made in views, so they are not in the EvtStore: hide them

  CHECK( m_previousDecisionsKey.initialize() );
  renounce(m_previousDecisionsKey);

  CHECK( m_decisionsKey.initialize() );
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

StatusCode TestTrigL2CaloHypoAlg::execute_r( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  // new decisions
  auto decisions = std::make_unique<DecisionContainer>();
  auto aux = std::make_unique<DecisionAuxContainer>();
  decisions->setStore( aux.get() );

  // input for decision
  std::vector<ITrigL2CaloHypoTool::ClusterInfo> toolInput;

  // loop over previous decisions
  auto previousDecisionsHandle = SG::makeHandle( m_previousDecisionsKey, context );
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
     d->setObjectLink( "previousDecisions", ElementLink<DecisionContainer>(m_previousDecisionsKey.key(), counter) );// link to previous decision object
     
     ATH_MSG_DEBUG( "Added view, roi, cluster, previous decision to new decision "<<counter <<" for view "<<view->name()  );
     counter++;

  }

  ATH_MSG_DEBUG( "Found "<<toolInput.size()<<" inputs to tools");

   
  for ( auto& tool: m_hypoTools ) {
    CHECK( tool->decide( toolInput ) );
  }
 
  {
    auto handle =  SG::makeHandle( m_decisionsKey, context );
    CHECK( handle.record( std::move( decisions ), std::move( aux ) ) );
    ATH_MSG_DEBUG ( "Exit with "<<handle->size() <<" decision objects");
  }

  return StatusCode::SUCCESS;
}


// StatusCode TestTrigL2CaloHypoAlg::execute_rold( const EventContext& context ) const {  
//   ATH_MSG_DEBUG ( "Executing " << name() << "..." );
//   auto viewsHandle = SG::makeHandle( m_viewsKey, context );
  
//   auto decisions = std::make_unique<DecisionContainer>();
//   auto aux = std::make_unique<DecisionAuxContainer>();
//   decisions->setStore( aux.get() );

//   std::map<const TrigRoiDescriptor*, const TrigCompositeUtils::Decision* > roiToDecision;
//   auto previousDecisionsHandle = SG::makeHandle( m_previousDecisionsKey, context );
//   for ( auto previousDecision: *previousDecisionsHandle ) {
//     auto roiEL = previousDecision->objectLink<TrigRoiDescriptorCollection>( "initialRoI" );
//     CHECK( roiEL.isValid() );
//     const TrigRoiDescriptor* roi = *roiEL;
//     roiToDecision.insert( std::make_pair( roi, previousDecision ) );
//   }
//   ATH_MSG_DEBUG( "RoI to decisions map size: " << roiToDecision.size() );


//   std::vector<ITrigL2CaloHypoTool::ClusterInfo> toolInput;
//   // exploit knowledge that there is one cluster in the view
//   size_t counter = 0;
//   for ( auto view: *viewsHandle ) {
//     auto d = newDecisionIn( decisions.get() );
    
//     auto roiHandle =  SG::makeHandle( m_roisKey, context );
//     CHECK( roiHandle.setProxyDict( view ) );
//     const TrigRoiDescriptor* roi = roiHandle.cptr()->at(0);

//     auto clusterHandle =  SG::makeHandle( m_clustersKey, context );
//     CHECK( clusterHandle.setProxyDict( view ) );
       
//     toolInput.emplace_back( d, roi, clusterHandle.cptr()->at(0), roiToDecision[roi] );

//     {
//       auto el = ElementLink<xAOD::TrigEMClusterContainer>( view->name()+"_"+clusterHandle.key(), 0 ); // 0 because there is only one obj in per-view collection
//       CHECK( el.isValid() );
//       d->setObjectLink( "feature",  el );
//     }
//     {
//       auto el = ElementLink<TrigRoiDescriptorCollection>( view->name()+"_"+m_roisKey.key(), 0 );
//       CHECK( el.isValid() );
//       d->setObjectLink( "roi", el );
//     }
//     {
//       auto el = ElementLink< std::vector<SG::View*> >( m_viewsKey.key(), counter );
//       CHECK( el.isValid() );
//       d->setObjectLink( "view", el );
//     }
//   }
  
//   for ( auto& tool: m_hypoTools ) {
//     CHECK( tool->decide( toolInput ) );
//   }
 
//   {
//     auto handle =  SG::makeHandle( m_decisionsKey, context );
//     CHECK( handle.record( std::move( decisions ), std::move( aux ) ) );
//   }

//   return StatusCode::SUCCESS;
// }


