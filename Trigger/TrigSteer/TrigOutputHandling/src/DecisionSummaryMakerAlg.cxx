/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionSummaryMakerAlg.h"

DecisionSummaryMakerAlg::DecisionSummaryMakerAlg(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthReentrantAlgorithm(name, pSvcLocator) {}

DecisionSummaryMakerAlg::~DecisionSummaryMakerAlg() {}

StatusCode DecisionSummaryMakerAlg::initialize() {
  //ATH_MSG_DEBUG("Use macros for logging!");
  renounceArray( m_finalDecisionKeys );
  ATH_CHECK( m_finalDecisionKeys.initialize() ); 
  ATH_CHECK( m_summaryKey.initialize() );
  
  for ( auto& pair: m_lastStepForChain ) {
    struct { std::string chain, collection; } conf { pair.first, pair.second };    
    m_collectionFilter[ conf.collection ].insert( HLT::Identifier( conf.chain).numeric() );
  }
  
  return StatusCode::SUCCESS;
}

StatusCode DecisionSummaryMakerAlg::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode DecisionSummaryMakerAlg::execute_r(const EventContext& context) const
{
  auto outputHandle{ SG::makeHandle( m_summaryKey, context) };
  auto container = std::make_unique<TrigCompositeUtils::DecisionContainer>();
  auto aux = std::make_unique<TrigCompositeUtils::DecisionAuxContainer>();
  container->setStore( aux.get() );


  TrigCompositeUtils::Decision* output = TrigCompositeUtils::newDecisionIn( container.get(), "HLTSummary" );
  
  for ( auto& key: m_finalDecisionKeys ) {
    auto handle{ SG::makeHandle(key, context) };
    if ( not handle.isValid() )  {
      ATH_MSG_DEBUG("missing input " <<  key.key() << " likely rejected");
      continue;
    }
    auto thisCollFilter = m_collectionFilter.find(key.key() );
    if ( thisCollFilter == m_collectionFilter.end() ) {
      ATH_MSG_WARNING( "The colleciton " << key.key() << " is not configured to contain any final decision, remove it from the configuration of " << name() << " to save time" );
      continue;
    }

    TrigCompositeUtils::DecisionIDContainer sum;
    for ( const TrigCompositeUtils::Decision* decisionObject: *handle ) {
      sum.insert( TrigCompositeUtils::decisionIDs(decisionObject).begin(), TrigCompositeUtils::decisionIDs(decisionObject).end() );  // copy from vector
    }
    
    TrigCompositeUtils::DecisionIDContainer final;
    std::set_intersection(  sum.begin(), sum.end(),
			    thisCollFilter->second.begin(), thisCollFilter->second.end(), 
			    std::inserter(final, final.begin() ) ); // should be faster than remove_if
    
    TrigCompositeUtils::decisionIDs( output ).insert( TrigCompositeUtils::decisionIDs( output ).end(), 
						    sum.begin(), sum.end() );
    
  }
  if ( msgLvl( MSG::DEBUG ) ) {
    ATH_MSG_DEBUG( "Number of positive decisions " <<  TrigCompositeUtils::decisionIDs( output ).size() << " passing chains");
    for ( auto d: TrigCompositeUtils::decisionIDs( output ) ) {
      ATH_MSG_DEBUG( HLT::Identifier( d ) );
    }
  }
  ATH_CHECK( outputHandle.record( std::move( container), std::move( aux )) );

  return StatusCode::SUCCESS;
}

