/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "TrigL2CaloHypoAlg.h"

using namespace TrigCompositeUtils;

TrigL2CaloHypoAlg::TrigL2CaloHypoAlg( const std::string& name, 
				      ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator ) {}

TrigL2CaloHypoAlg::~TrigL2CaloHypoAlg() {}

StatusCode TrigL2CaloHypoAlg::initialize() {
  ATH_MSG_INFO ( "Initializing " << name() << "..." );
  CHECK( m_hypoTools.retrieve() );

  CHECK( m_viewsKey.initialize() );

  renounce( m_clustersKey );
  CHECK( m_clustersKey.initialize() );

  renounce( m_roisKey );
  CHECK( m_roisKey.initialize() );

  CHECK( m_previousDecisionsKey.initialize() );

  CHECK( m_decisionsKey.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode TrigL2CaloHypoAlg::execute_r( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto viewsHandle = SG::makeHandle( m_viewsKey, context );
  
  auto decisions = std::make_unique<DecisionContainer>();
  auto aux = std::make_unique<DecisionAuxContainer>();
  decisions->setStore( aux.get() );


  std::map<const TrigRoiDescriptor*, const TrigCompositeUtils::Decision* > roiToDecision;
  auto previousDecisionsHandle = SG::makeHandle( m_previousDecisionsKey, context );
  for ( auto previousDecision: *previousDecisionsHandle ) {
    auto roiEL = previousDecision->objectLink<TrigRoiDescriptorCollection>( "initialRoI" );
    CHECK( roiEL.isValid() );
    const TrigRoiDescriptor* roi = *roiEL;
    roiToDecision.insert( std::make_pair( roi, previousDecision ) );
  }
  ATH_MSG_DEBUG( "RoI to decisions map size: " << roiToDecision.size() );


  std::vector<ITrigL2CaloHypoTool::ClusterInfo> toolInput;
  // exploit knowledge that there is one cluster in the view
  size_t counter = 0;
  for ( auto view: *viewsHandle ) {
    auto d = newDecisionIn( decisions.get() );
    
    auto roiHandle =  SG::makeHandle( m_roisKey, context );
    CHECK( roiHandle.setProxyDict( view ) );
    const TrigRoiDescriptor* roi = roiHandle.cptr()->at(0);

    auto clusterHandle =  SG::makeHandle( m_clustersKey, context );
    CHECK( clusterHandle.setProxyDict( view ) );
       
    toolInput.emplace_back( d, roi, clusterHandle.cptr()->at(0), roiToDecision[roi] );

    {
      auto el = ElementLink<xAOD::TrigEMClusterContainer>( view->name()+"_"+clusterHandle.key(), 0 ); // 0 because there is only one obj in per-view collection
      CHECK( el.isValid() );
      d->setObjectLink( "feature",  el );
    }
    {
      auto el = ElementLink<TrigRoiDescriptorCollection>( view->name()+"_"+m_roisKey.key(), 0 );
      CHECK( el.isValid() );
      d->setObjectLink( "roi", el );
    }
    {
      auto el = ElementLink< std::vector<SG::View*> >( m_viewsKey.key(), counter );
      CHECK( el.isValid() );
      d->setObjectLink( "view", el );
    }
  }
  
  for ( auto& tool: m_hypoTools ) {
    CHECK( tool->decide( toolInput ) );
  }
 
  {
    auto handle =  SG::makeHandle( m_decisionsKey, context );
    CHECK( handle.record( std::move( decisions ), std::move( aux ) ) );
  }

  return StatusCode::SUCCESS;
}


