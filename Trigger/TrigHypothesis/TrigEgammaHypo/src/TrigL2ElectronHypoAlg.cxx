/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include <map>
#include "GaudiKernel/Property.h"
#include "TrigL2ElectronHypoAlg.h"



TrigL2ElectronHypoAlg::TrigL2ElectronHypoAlg( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator )
{
  //declareProperty( "Property", m_nProperty );

}

TrigL2ElectronHypoAlg::~TrigL2ElectronHypoAlg()
{}

StatusCode TrigL2ElectronHypoAlg::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." );
  ATH_CHECK( m_hypoTools.retrieve() );
  ATH_CHECK( m_electronsKey.initialize() );
  ATH_CHECK( m_clusterDecisionsKey.initialize() );
  ATH_CHECK( m_decisionsKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode TrigL2ElectronHypoAlg::finalize()
{
  ATH_MSG_INFO ( "Finalizing " << name() << "..." );

  return StatusCode::SUCCESS;
}

StatusCode TrigL2ElectronHypoAlg::execute_r( const EventContext& context ) const
{  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  // obtain electrons
  auto electronsHandle = SG::makeHandle( m_electronsKey, context );
  auto clusterDecisions = SG::makeHandle( m_clusterDecisionsKey, context );
  

  // prepare decisions container and link back to the clusters, and decision on clusters
  auto decisions = std::make_unique<DecisionContainer>();
  auto aux = std::make_unique<DecisionAuxContainer>();
  decisions->setStore( aux.get() );

  // extract mapping 
  std::map<const xAOD::TrigEMCluster*, size_t> clusterToIndexMap;
  size_t clusterCounter = 0;
  for ( auto clusterDecisionIter = clusterDecisions->begin(); 
	clusterDecisionIter != clusterDecisionIter; 
	++clusterDecisionIter, ++clusterCounter ) {

    ATH_CHECK( ( *clusterDecisionIter )->hasObjectLink( "feature" ) );
    const xAOD::TrigEMCluster* cluster = ( *clusterDecisionIter )->object<xAOD::TrigEMCluster>( "feature" );
    //    ATH_CHECK( clusterLink.isValid() );    
    //    auto cluster = clusterLink.getDataPtr();
    clusterToIndexMap.insert( std::make_pair( cluster, clusterCounter ) );
  }
  
  std::vector<TrigL2ElectronHypoTool::Input> hypoToolInput;
  
  size_t electronCounter = 0;
  for ( auto electronIter = electronsHandle->begin(); electronIter != electronsHandle->end(); ++electronIter, electronCounter++ ) {
    auto d = newDecisionIn( decisions.get() );
    d->setObjectLink( "feature", ElementLink<xAOD::TrigElectronContainer>( m_electronsKey.key(), electronCounter ) );

    auto clusterPtr = (*electronIter)->emCluster();
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
    
    
    hypoToolInput.emplace_back( TrigL2ElectronHypoTool::Input{ d, 
	  *electronIter, 
	  origCluster->first,
	  clusterDecisionIDs } );
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


