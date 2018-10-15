/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include <map>
#include "GaudiKernel/Property.h"
#include "TrigL2CaloRingerHypoAlgMT.h"

using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionAuxContainer;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::decisionIDs;
using TrigCompositeUtils::newDecisionIn;
using TrigCompositeUtils::linkToPrevious;


TrigL2CaloRingerHypoAlgMT::TrigL2CaloRingerHypoAlgMT( const std::string& name, 
              ISvcLocator* pSvcLocator ) : 
  ::HypoBase( name, pSvcLocator ) {}

TrigL2CaloRingerHypoAlgMT::~TrigL2CaloRingerHypoAlgMT() {}

StatusCode TrigL2CaloRingerHypoAlgMT::initialize() {
  ATH_MSG_INFO ( "Initializing " << name() << "..." );
  ATH_CHECK( m_hypoTools.retrieve() );
  
  ATH_CHECK( m_views.initialize() );
  ATH_CHECK( m_decisionsKey.initialize() );
  
  renounce( m_outputKey );  
  ATH_CHECK( m_outputKey.initialize() );


  return StatusCode::SUCCESS;
}


StatusCode TrigL2CaloRingerHypoAlgMT::execute_r( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  
  // prepare decisions container and link back to the clusters, and decision on clusters
  auto decisions = std::make_unique<DecisionContainer>();
  auto aux = std::make_unique<DecisionAuxContainer>();
  decisions->setStore( aux.get() );


  //ATH_MSG_DEBUG( "RNNOutput ptr to decision map has size " << clusterToIndexMap.size() );

  // prepare imput for tools
  std::vector<TrigL2CaloRingerHypoToolMT::RNNOutInfo> hypoToolInput;
  
  auto viewsHandle = SG::makeHandle( m_views, context );
  for ( auto view: *viewsHandle ) {
    size_t counter = 0;
    auto rnnOutHandle = SG::makeHandle( m_outputKey, context );  
    //CHECK( electronsHandle.setProxyDict( view ) );
    //CHECK( electronsHandle.isValid() );

    for ( auto rnnOutIter = rnnOutHandle->begin(); rnnOutIter != rnnOutHandle->end(); ++rnnOutIter, counter++ ) {
      auto d = newDecisionIn( decisions.get() );
      d->setObjectLink( "feature", ElementLink<xAOD::TrigRNNOutputContainer>( view->name()+"_"+m_outputKey.key(), counter ) );
      hypoToolInput.emplace_back( TrigL2CaloRingerHypoToolMT::RNNOutInfo{ d, *rnnOutIter } );
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
