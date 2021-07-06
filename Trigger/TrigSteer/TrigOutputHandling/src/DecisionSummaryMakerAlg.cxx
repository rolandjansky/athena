/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "DecisionSummaryMakerAlg.h"

DecisionSummaryMakerAlg::DecisionSummaryMakerAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode DecisionSummaryMakerAlg::initialize() {
  renounceArray( m_finalDecisionKeys );
  ATH_CHECK( m_finalDecisionKeys.initialize() );
  ATH_CHECK( m_summaryKey.initialize() );
  ATH_CHECK( m_l1SummaryKey.initialize() );

  for ( auto& [chain, collections]: m_lastStepForChain ) {
    for ( auto& collection: collections ) {
      m_collectionFilter[ collection ].insert( HLT::Identifier( chain ).numeric() );
      ATH_MSG_DEBUG( "Final decision of the chain " << chain << " will be read from " << collection );
    }
  }

  ATH_CHECK( m_costWriteHandleKey.initialize( m_doCostMonitoring ) );
  ATH_CHECK( m_rosWriteHandleKey.initialize( m_doCostMonitoring ) );
  if (m_doCostMonitoring) {
    ATH_CHECK( m_trigCostSvcHandle.retrieve() );
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

  Decision* passRawOutput = newDecisionIn( container, summaryPassNodeName() );
  Decision* prescaledOutput = newDecisionIn( container, summaryPrescaledNodeName() );

  DecisionIDContainer allPassingFinalIDs;

  // Collate final decisions into the passRawOutput object
  for ( auto& key: m_finalDecisionKeys ) {
    auto handle{ SG::makeHandle(key, context) };
    if ( not handle.isValid() )  {
      ATH_MSG_DEBUG("Input " <<  key.key() << " not present, did not run in this event.");
      continue;
    }
    const auto thisCollFilter = m_collectionFilter.find( key.key() );
    if ( thisCollFilter == m_collectionFilter.end() ) {
      ATH_MSG_WARNING( "The collection " << key.key() << " is not configured to contain any final decision,"
                       << "remove it from the configuration of " << name() << " to save time" );
      continue;
    }

    for ( const Decision* decisionObject: *handle ) {
      // Get passing chains from this decisionObject
      DecisionIDContainer passingIDs;
      decisionIDs(decisionObject, passingIDs);

      // Filter out chains for which this is NOT the final step of their processing
      DecisionIDContainer passingFinalIDs;
      std::set_intersection( passingIDs.begin(), passingIDs.end(),
          thisCollFilter->second.begin(), thisCollFilter->second.end(),
          std::inserter(passingFinalIDs, passingFinalIDs.begin() ) ); // should be faster than remove_if

      if (passingFinalIDs.empty()) {
        continue;
      }

      // Create a node to act as a filter between the final summary node and this HypoAlg.
      // This ensures that we follow this edge (now two edges) in the graph only for the chains in
      // passingFinalIDs rather than for other chains which are active in decisionObject which
      // may accept the event via other objects

      // filter -> HypoAlg
      Decision* filter = newDecisionIn( container, decisionObject, summaryFilterNodeName(), context );
      decisionIDs(filter).insert( decisionIDs(filter).end(), passingFinalIDs.begin(), passingFinalIDs.end() );

      // HLTPassRaw -> filter
      linkToPrevious(passRawOutput, filter, context);

      // Accumulate and de-duplicate passed IDs for which this hypo was the Chain's final step
      allPassingFinalIDs.insert( passingFinalIDs.begin(), passingFinalIDs.end() );
    }
  }

  // Copy decisions set into passRawOutput's persistent vector
  decisionIDs(passRawOutput).insert( decisionIDs(passRawOutput).end(),
    allPassingFinalIDs.begin(), allPassingFinalIDs.end() );

  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG( "Number of positive decisions " <<  allPassingFinalIDs.size() << " passing chains");
    for ( auto d: allPassingFinalIDs ) {
      ATH_MSG_DEBUG( HLT::Identifier( d ) );
    }
  }

  // Get the data from the L1 decoder, this is where prescales were applied
  SG::ReadHandle<DecisionContainer> l1DecoderSummary( m_l1SummaryKey, context );
  const Decision* l1SeededChains = nullptr; // Activated by L1
  const Decision* activeChains = nullptr; // Activated and passed prescale check
  const Decision* prescaledChains = nullptr; // Activated but failed prescale check
  for (const Decision* d : *l1DecoderSummary) {
    if (d->name() == "l1seeded") {
      l1SeededChains = d;
    } else if (d->name() == "unprescaled") {
      activeChains = d;
    } else if (d->name() == "prescaled") {
      prescaledChains = d;
    } else {
      ATH_MSG_ERROR("DecisionSummaryMakerAlg encountered an unknown set of decisions from the L1Decoder, name '" << d->name() << "'");
      return StatusCode::FAILURE;
    }
  }

  if (l1SeededChains == nullptr || activeChains == nullptr || prescaledChains == nullptr) {
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

  // Do cost monitoring
  if (m_doCostMonitoring) {
    SG::WriteHandle<xAOD::TrigCompositeContainer> costMonOutput = createAndStore(m_costWriteHandleKey, context);
    SG::WriteHandle<xAOD::TrigCompositeContainer> rosMonOutput = createAndStore(m_rosWriteHandleKey, context);
    // Populate collection (assuming monitored event, otherwise collection will remain empty)
    ATH_CHECK(m_trigCostSvcHandle->endEvent(context, costMonOutput, rosMonOutput));
  }

  // Set the algorithm's filter status. This controlls the running of finalisation algs which we only want to execute
  // in events which are accepted by one ore more chains.
  bool filterStatus = true;
  if (m_setFilterStatus) {
    filterStatus = (not allPassingFinalIDs.empty());
  }
  setFilterPassed(filterStatus, context );

  return StatusCode::SUCCESS;
}
