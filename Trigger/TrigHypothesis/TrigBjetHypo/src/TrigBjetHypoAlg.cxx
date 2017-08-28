/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "TrigBjetHypoAlg.h"

TrigBjetHypoAlg::TrigBjetHypoAlg( const std::string& name, 
				      ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator ),
  m_hypoTools( this ),
  m_roisKey( "RoIs" ),
  m_bTagKey( "BTagging" ),
  m_decisionsKey( "BjetHypoDecisions" ) {

  declareProperty( "HypoTools", m_hypoTools );
  declareProperty( "BTagging", m_bTagKey );
  declareProperty( "RoIs", m_roisKey );
  declareProperty( "Decisions", m_decisionsKey ); 
}

TrigBjetHypoAlg::~TrigBjetHypoAlg()
{}

StatusCode TrigBjetHypoAlg::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." );
  CHECK( m_hypoTools.retrieve() );
  CHECK( m_bTagKey.initialize() );
  CHECK( m_roisKey.initialize() );
  CHECK( m_decisionsKey.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode TrigBjetHypoAlg::finalize()
{
  ATH_MSG_INFO ( "Finalizing " << name() << "..." );

  return StatusCode::SUCCESS;
}

StatusCode TrigBjetHypoAlg::execute_r( const EventContext& context ) const
{  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto bTagHandle = SG::makeHandle( m_bTagKey, context );
  auto roisHandle = SG::makeHandle( m_roisKey, context );
  
  auto decisions = std::make_unique<DecisionContainer>();
  auto aux = std::make_unique<DecisionAuxContainer>();
  decisions->setStore( aux.get() );

  // prepare decision storage ( we could simplify it )
  // Lidija: should be checked once more
  size_t counter = 0;
  // ---->>>>>>
  for ( auto bTagIter =   bTagHandle->begin();  bTagIter != bTagHandle->end(); ++bTagIter, ++counter ) {
    auto d = newDecisionIn( decisions.get() );
    d->setObjectLink( "feature", ElementLink<xAOD::BTaggingContainer>( m_bTagKey.key(), counter ) );
    d->setObjectLink( "roi", ElementLink<TrigRoiDescriptorCollection>( m_roisKey.key(), counter ) );
  }


  size_t index = 0;
  for ( ; index < decisions->size(); ++index ) {

    auto bTag = bTagHandle->at( index );
    auto roiDescriptor = roisHandle->at( index );
    auto decision = decisions->at( index );


    for ( auto tool : m_hypoTools ) {
      // interface of the tool needs to be suitable for current system, so no TrigComposite
      // also no support for the multi-electrons yet ( will be additional method )
      if ( tool->decide( bTag, roiDescriptor ) ) {   
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




