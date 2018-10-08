/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "TrigBjetEtHypoAlg.h"


TrigBjetEtHypoAlg::TrigBjetEtHypoAlg( const std::string& name, 
				      ISvcLocator* pSvcLocator ) : 
  ::HypoBase( name, pSvcLocator ) {}

TrigBjetEtHypoAlg::~TrigBjetEtHypoAlg()
{}

StatusCode TrigBjetEtHypoAlg::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." );

  ATH_MSG_DEBUG(  "declareProperty review:"   );
  ATH_MSG_DEBUG(  "   " << m_jetsKey          );

  ATH_MSG_DEBUG( "Initializing Tools" );
  ATH_CHECK( m_hypoTools.retrieve() );

  ATH_MSG_DEBUG( "Initializing HandleKeys" );
  CHECK( m_jetsKey.initialize() );
  CHECK( m_decisionsKey.initialize() ); // Output Decisions

  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlg::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlg::execute_r( const EventContext& context ) const {  
  ATH_MSG_INFO ( "Executing " << name() << "..." );

  // In case I need it (not sure). Taker from Jet code
  // Read in previous Decisions made before running this Hypo Alg.
  // The container should have only one such Decision in case we are cutting on 'j' threshold (for L1)
  SG::ReadHandle< TrigCompositeUtils::DecisionContainer > h_prevDecisions = SG::makeHandle( decisionInput(),context );
  CHECK( h_prevDecisions.isValid() );

  // Retrieve Jet Container
  SG::ReadHandle< xAOD::JetContainer > h_jetCollection = SG::makeHandle( m_jetsKey,context );
  ATH_MSG_DEBUG( "Retrieved jets from : " << m_jetsKey.key() );
  CHECK( h_jetCollection.isValid() );
  const xAOD::JetContainer *jetCollection = h_jetCollection.get();
  ATH_MSG_DEBUG( "Found " << jetCollection->size()<< " jets."  );

  // Decide (Hypo Tool)
  
  // Output
  std::unique_ptr< TrigCompositeUtils::DecisionContainer > decisions = std::make_unique< TrigCompositeUtils::DecisionContainer >();
  std::unique_ptr< TrigCompositeUtils::DecisionAuxContainer > aux = std::make_unique< TrigCompositeUtils::DecisionAuxContainer >();
  decisions->setStore( aux.get() );

  /*
  // prepare decision storage ( we could simplify it )
  // Lidija: should be checked once more
  size_t counter = 0;
  // ---->>>>>>
  xAOD::BTaggingContainer::const_iterator bTagIter = bTagHandle->begin();
  for ( ;  bTagIter != bTagHandle->end(); ++bTagIter, ++counter ) {
    TrigCompositeUtils::Decision *d = newDecisionIn( decisions.get() );
    d->setObjectLink( "feature", ElementLink<xAOD::BTaggingContainer>( m_bTagKey.key(), counter ) );
    d->setObjectLink( "roi", ElementLink<TrigRoiDescriptorCollection>( m_roisKey.key(), counter ) );
  }


  size_t index = 0;
  for ( ; index < decisions->size(); ++index ) {

    const xAOD::BTagging *bTag = bTagHandle->at( index );
    const TrigRoiDescriptor *roiDescriptor = roisHandle->at( index );
    TrigCompositeUtils::Decision *decision = decisions->at( index );

  }

    */

  {
    SG::WriteHandle< TrigCompositeUtils::DecisionContainer > handle =  SG::makeHandle( m_decisionsKey, context );
    CHECK( handle.record( std::move( decisions ), std::move( aux ) ) );
  }

  return StatusCode::SUCCESS;
}




