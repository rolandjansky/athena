/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "TrigL2CaloHypoAlg.h"

using namespace TrigCompositeUtils;

TrigL2CaloHypoAlg::TrigL2CaloHypoAlg( const std::string& name, 
				      ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator ),
  m_hypoTools( this ),
  m_clustersKey( "CaloClusters" ),
  m_roisKey( "RoIs" ),
  m_decisionsKey( "CaloHypoDecisions" ) {

  declareProperty( "HypoTools", m_hypoTools );
  declareProperty( "CaloClusters", m_clustersKey );
  declareProperty( "RoIs", m_roisKey );
  declareProperty( "Decisions", m_decisionsKey ); 
}

TrigL2CaloHypoAlg::~TrigL2CaloHypoAlg()
{}

StatusCode TrigL2CaloHypoAlg::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." );
  CHECK( m_hypoTools.retrieve() );
  CHECK( m_clustersKey.initialize() );
  CHECK( m_roisKey.initialize() );
  CHECK( m_decisionsKey.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode TrigL2CaloHypoAlg::finalize()
{
  ATH_MSG_INFO ( "Finalizing " << name() << "..." );

  return StatusCode::SUCCESS;
}

StatusCode TrigL2CaloHypoAlg::execute_r( const EventContext& context ) const
{  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto clustersHandle = SG::makeHandle( m_clustersKey, context );
  auto roisHandle = SG::makeHandle( m_roisKey, context );
  
  auto decisions = std::make_unique<DecisionContainer>();
  auto aux = std::make_unique<DecisionAuxContainer>();
  decisions->setStore( aux.get() );

  // prepare decision storage ( we could simplify it )
  size_t counter = 0;
  for ( auto clusterIter =   clustersHandle->begin();  clusterIter != clustersHandle->end(); ++clusterIter, ++counter ) {
    auto d = newDecisionIn( decisions.get() );
    d->setObjectLink( "feature", ElementLink<xAOD::TrigEMClusterContainer>( m_clustersKey.key(), counter ) );
    d->setObjectLink( "roi", ElementLink<TrigRoiDescriptorCollection>( m_roisKey.key(), counter ) );
  }


  size_t index = 0;
  for ( ; index < decisions->size(); ++index ) {

    auto cluster = clustersHandle->at( index );
    auto roi = roisHandle->at( index );
    auto decision = decisions->at( index );


    for ( auto tool : m_hypoTools ) {
      // interface of the tool needs to be suitable for current system, so no TrigComposite
      // also no support for the multi-electrons yet ( will be additional method )
      if ( tool->decide( cluster, roi ) ) {   
	addDecisionID( tool->decisionId(), decision );	  
	ATH_MSG_DEBUG( " + " << tool->name() );
      } else {
	ATH_MSG_DEBUG( " - " << tool->name() );
      }
    }
    
  }

  {
    auto handle =  SG::makeHandle( m_decisionsKey, context );
    CHECK( handle.record( std::move( decisions ), std::move( aux ) ) );
  }

  return StatusCode::SUCCESS;
}


