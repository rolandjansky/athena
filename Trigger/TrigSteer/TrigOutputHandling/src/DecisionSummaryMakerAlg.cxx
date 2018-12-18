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

  using namespace TrigCompositeUtils;

  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore( m_summaryKey, context );
  auto container = outputHandle.ptr();

  Decision* passRawOutput = newDecisionIn( container, "HLTPassRaw" );
  Decision* prescaledOutput = newDecisionIn( container, "HLTPrescaled" );
  Decision* rerunOutput = newDecisionIn( container, "HLTRerun" );

  // Collate final decisions into the passRawOutput object
  for ( auto& key: m_finalDecisionKeys ) {
    auto handle{ SG::makeHandle(key, context) };
    if ( not handle.isValid() )  {
      ATH_MSG_DEBUG("missing input " <<  key.key() << " likely rejected");
      continue;
    }
    const auto thisCollFilter = m_collectionFilter.find( key.key() );
    if ( thisCollFilter == m_collectionFilter.end() ) {
      ATH_MSG_WARNING( "The collection " << key.key() << " is not configured to contain any final decision," 
                       << "remove it from the configuration of " << name() << " to save time" );
      continue;
    }

    DecisionIDContainer sum;
    for ( const Decision* decisionObject: *handle ) {
      sum.insert( decisionIDs(decisionObject).begin(), decisionIDs(decisionObject).end() );  // copy from vector
    }
    
    DecisionIDContainer finalIDs;
    std::set_intersection( sum.begin(), sum.end(),
          thisCollFilter->second.begin(), thisCollFilter->second.end(), 
          std::inserter(finalIDs, finalIDs.begin() ) ); // should be faster than remove_if
    
    decisionIDs( passRawOutput ).insert( decisionIDs( passRawOutput ).end(),
          finalIDs.begin(), finalIDs.end() ); // Copy decisions set into passRawOutput's persistent vector
    
  }
  if ( msgLvl( MSG::DEBUG ) ) {
    ATH_MSG_DEBUG( "Number of positive decisions " <<  decisionIDs( passRawOutput ).size() << " passing chains");
    for ( auto d: decisionIDs( passRawOutput ) ) {
      ATH_MSG_DEBUG( HLT::Identifier( d ) );
    }
  }

  // TODO collate final rerun decisions into the rerunOutput object

  // Get the data from the L1 decoder, this is where prescales were applied
  SG::ReadHandle<DecisionContainer> l1DecoderSummary( m_l1SummaryKey, context );
  const Decision* l1SeededChains = nullptr; // Activated by L1
  const Decision* activeChains = nullptr; // Activated and passed prescale check
  const Decision* prescaledChains = nullptr; // Activated but failed prescale check
  const Decision* rerunChains = nullptr; // Set to activate in the second pass
  for (const Decision* d : *l1DecoderSummary) {
    if (d->name() == "l1seeded") {
      l1SeededChains = d;
    } else if (d->name() == "unprescaled") {
      activeChains = d;
    } else if (d->name() == "prescaled") {
      prescaledChains = d;
    } else if (d->name() == "rerun") {
      rerunChains = d;
    } else {
      ATH_MSG_WARNING("DecisionSummaryMakerAlg encountered an unknown set of decisions from the L1Decoder, name '" << d->name() << "'");
    }
  }

  if (l1SeededChains == nullptr || activeChains == nullptr || prescaledChains == nullptr || rerunChains == nullptr) {
    ATH_MSG_ERROR("Unable to read in the summary from the L1Decoder. Cannot write to the HLT output summary the prescale status of HLT chains.");
    return StatusCode::FAILURE;
  }

  // l1SeededChains is not currently used here

  // activeChains is not currently used here

  // Get chains which were prescaled out. This is the set of chains which were seeded but which were NOT active (in the first pass)
  DecisionIDContainer prescaledIDs;
  decisionIDs( prescaledChains, prescaledIDs ); // Extract from prescaledChains (a Decision*) into prescaledIDs (a set<int>)
  decisionIDs( prescaledOutput ).insert( decisionIDs( prescaledOutput ).end(),
        prescaledIDs.begin(), prescaledIDs.end() ); // Save this to the output

  // Save the set of chains which were flagged as only executing in rerun.
  DecisionIDContainer rerunIDs;
  decisionIDs( rerunChains, rerunIDs ); 
  decisionIDs( rerunOutput ).insert( decisionIDs( rerunOutput ).end(),
        rerunIDs.begin(), rerunIDs.end() );

  // Do cost monitoring
  if (m_enableCostMonitoring) {
    SG::WriteHandle<xAOD::TrigCompositeContainer> costMonOutput = createAndStore(m_costWriteHandleKey, context);
    // Populate collection (assuming monitored event, otherwise collection will remain empty)
    ATH_CHECK(m_trigCostSvcHandle->endEvent(context, costMonOutput));
  }

  return StatusCode::SUCCESS;
}

