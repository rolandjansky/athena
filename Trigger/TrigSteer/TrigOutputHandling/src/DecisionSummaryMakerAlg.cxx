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
  ATH_CHECK( m_l1SummaryKey.initialize() );
  
  for ( auto& pair: m_lastStepForChain ) {
    struct { std::string chain, collection; } conf { pair.first, pair.second };    
    m_collectionFilter[ conf.collection ].insert( HLT::Identifier( conf.chain).numeric() );
    ATH_MSG_DEBUG( "Final decision of the chain " << conf.chain << " will be read from " << conf.collection );
  }

  if (m_enableCostMonitoring) {
    CHECK( m_trigCostSvcHandle.retrieve() );
    CHECK( m_costWriteHandleKey.initialize() );
  }
  
  return StatusCode::SUCCESS;
}

StatusCode DecisionSummaryMakerAlg::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode DecisionSummaryMakerAlg::execute(const EventContext& context) const {
  SG::WriteHandle<TrigCompositeUtils::DecisionContainer> outputHandle = TrigCompositeUtils::createAndStore( m_summaryKey, context );
  auto container = outputHandle.ptr();


  TrigCompositeUtils::Decision* passRawOutput = TrigCompositeUtils::newDecisionIn( container, "HLTPassRaw" );
  TrigCompositeUtils::Decision* prescaledOutput = TrigCompositeUtils::newDecisionIn( container, "HLTPrescaled" );
  TrigCompositeUtils::Decision* rerunOutput = TrigCompositeUtils::newDecisionIn( container, "HLTRerun" );

  // Collate final decisions into the passRawOutput object
  for ( auto& key: m_finalDecisionKeys ) {
    auto handle{ SG::makeHandle(key, context) };
    if ( not handle.isValid() )  {
      ATH_MSG_DEBUG("missing input " <<  key.key() << " likely rejected");
      continue;
    }
    const auto thisCollFilter = m_collectionFilter.find( key.key() );
    if ( thisCollFilter == m_collectionFilter.end() ) {
      ATH_MSG_WARNING( "The colleciton " << key.key() << " is not configured to contain any final decision, remove it from the configuration of " << name() << " to save time" );
      continue;
    }

    TrigCompositeUtils::DecisionIDContainer sum;
    for ( const TrigCompositeUtils::Decision* decisionObject: *handle ) {
      sum.insert( TrigCompositeUtils::decisionIDs(decisionObject).begin(), TrigCompositeUtils::decisionIDs(decisionObject).end() );  // copy from vector
    }
    
    TrigCompositeUtils::DecisionIDContainer finalIDs;
    std::set_intersection(  sum.begin(), sum.end(),
          thisCollFilter->second.begin(), thisCollFilter->second.end(), 
          std::inserter(finalIDs, finalIDs.begin() ) ); // should be faster than remove_if
    
    TrigCompositeUtils::decisionIDs( passRawOutput ).insert( TrigCompositeUtils::decisionIDs( passRawOutput ).end(), 
          finalIDs.begin(), finalIDs.end() );
    
  }
  if ( msgLvl( MSG::DEBUG ) ) {
    ATH_MSG_DEBUG( "Number of positive decisions " <<  TrigCompositeUtils::decisionIDs( passRawOutput ).size() << " passing chains");
    for ( auto d: TrigCompositeUtils::decisionIDs( passRawOutput ) ) {
      ATH_MSG_DEBUG( HLT::Identifier( d ) );
    }
  }

  // Get the data from the L1 decoder, this is where prescales were applied
  SG::ReadHandle<TrigCompositeUtils::DecisionContainer> l1DecoderSummary( m_l1SummaryKey, context );
  const TrigCompositeUtils::Decision* l1SeededChains = nullptr;
  const TrigCompositeUtils::Decision* activeChains = nullptr;
  const TrigCompositeUtils::Decision* rerunChains = nullptr;
  for (const TrigCompositeUtils::Decision* d : *l1DecoderSummary) {
    if (d->name() == "l1seeded") {
      l1SeededChains = d;
    } else if (d->name() == "unprescaled") {
      activeChains = d;
    } else if (d->name() == "rerun") {
      rerunChains = d;
    } else {
      ATH_MSG_WARNING("DecisionSummaryMakerAlg encountered an unknown set of decisions from the L1Decoder, name '" << d->name() << "'");
    }
  }

  if (l1SeededChains == nullptr || activeChains == nullptr || rerunChains == nullptr) {
    ATH_MSG_ERROR("Unable to read in the summary from the L1Decoder. Cannot write to the HLT output summary the prescale status of HLT chains.");
    return StatusCode::FAILURE;
  }

  // Get chains which were prescaled out. This is the set of chains which were seeded but which were NOT active (in the first pass)
  HLT::IDVec prescaledIDs;
  std::set_difference( 
        TrigCompositeUtils::decisionIDs(l1SeededChains).begin(), TrigCompositeUtils::decisionIDs(l1SeededChains).end(),
        TrigCompositeUtils::decisionIDs(activeChains).begin(),   TrigCompositeUtils::decisionIDs(activeChains).end(),
        std::back_inserter(prescaledIDs) );
  // Save this to the output
  TrigCompositeUtils::decisionIDs( prescaledOutput ).insert( TrigCompositeUtils::decisionIDs( prescaledOutput ).end(), 
        prescaledIDs.begin(), prescaledIDs.end() );

  // Save the set of chains which were flagged as only executing in rerun. This is a direct copy
  TrigCompositeUtils::DecisionIDContainer rerunIDs;
  TrigCompositeUtils::decisionIDs( rerunChains, rerunIDs ); // Extract from rerunChains (a Decision*) into rerunIDs (a set<int>)
  TrigCompositeUtils::decisionIDs( rerunOutput ).insert( TrigCompositeUtils::decisionIDs( rerunOutput ).end(), 
        rerunIDs.begin(), rerunIDs.end() );

  // Do cost monitoring
  if (m_enableCostMonitoring) {
    SG::WriteHandle<xAOD::TrigCompositeContainer> costMonOutput = TrigCompositeUtils::createAndStore(m_costWriteHandleKey, context);
    // Populate collection (assuming monitored event, otherwise collection will remain empty)
    ATH_CHECK(m_trigCostSvcHandle->endEvent(context, costMonOutput));
  }

  return StatusCode::SUCCESS;
}

