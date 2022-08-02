/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "DecisionSummaryMakerAlg.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "CxxUtils/phihelper.h"

#include <algorithm>
#include <sstream>
#include <unordered_set>

namespace {
  const std::string s_expressStreamName{"express"};
}

DecisionSummaryMakerAlg::DecisionSummaryMakerAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode DecisionSummaryMakerAlg::initialize() {
  renounceArray( m_finalDecisionKeys );
  ATH_CHECK( m_finalDecisionKeys.initialize() );
  ATH_CHECK( m_summaryKey.initialize() );
  ATH_CHECK( m_streamsSummaryKey.initialize() );
  ATH_CHECK( m_hltSeedingSummaryKey.initialize() );
  ATH_CHECK( m_hltMenuKey.initialize() );

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
  ATH_CHECK( m_prescaler.retrieve() );
  if (!m_monTool.empty()) {
    ATH_CHECK(m_monTool.retrieve());
  }

  return StatusCode::SUCCESS;
}

StatusCode DecisionSummaryMakerAlg::start() {
  SG::ReadHandle<TrigConf::HLTMenu> hltMenu{m_hltMenuKey};
  ATH_CHECK(hltMenu.isValid());

  // Fill the map of Chain ID -> stream names, omitting express which is treated separately due to express prescaling
  for (const TrigConf::Chain& chain : *hltMenu) {
    std::vector<std::string> streams = chain.streams();
    streams.erase(std::remove(streams.begin(),streams.end(),s_expressStreamName), streams.end());
    m_chainToStreamsMap.insert({HLT::Identifier(chain.name()).numeric(),std::move(streams)});
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
  Decision* passExpressOutput = newDecisionIn( container, summaryPassExpressNodeName() );
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

  // Monitor RoI updates between initial and final RoI
  monitorRoIs(passRawOutput);

  // Get the data from the HLTSeeding, this is where prescales were applied
  SG::ReadHandle<DecisionContainer> hltSeedingSummary( m_hltSeedingSummaryKey, context );
  const Decision* l1SeededChains = nullptr; // Activated by L1
  const Decision* activeChains = nullptr; // Activated and passed prescale check
  const Decision* prescaledChains = nullptr; // Activated but failed prescale check
  for (const Decision* d : *hltSeedingSummary) {
    if (d->name() == "l1seeded") {
      l1SeededChains = d;
    } else if (d->name() == "unprescaled") {
      activeChains = d;
    } else if (d->name() == "prescaled") {
      prescaledChains = d;
    } else {
      ATH_MSG_ERROR("DecisionSummaryMakerAlg encountered an unknown set of decisions from the HLTSeeding, name '" << d->name() << "'");
      return StatusCode::FAILURE;
    }
  }

  if (l1SeededChains == nullptr || activeChains == nullptr || prescaledChains == nullptr) {
    ATH_MSG_ERROR("Unable to read in the summary from the HLTSeeding. Cannot write to the HLT output summary the prescale status of HLT chains.");
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

  // Calculate and save express stream prescale decisions
  // TODO: this involves pointless conversions set<uint> -> vector<HLT::Identifier> -> set<uint>, adapt IPrescalingTool to operate on set<uint>
  HLT::IDVec allPassingFinalIDsVec{allPassingFinalIDs.begin(),allPassingFinalIDs.end()}; // Convert set to vector
  HLT::IDVec expressIDsVec;
  ATH_CHECK( m_prescaler->prescaleChains(context, allPassingFinalIDsVec, expressIDsVec, /*forExpressStream=*/ true) );
  DecisionIDContainer expressIDs; // Convert vector to set
  for (const HLT::Identifier& id : expressIDsVec) {
    expressIDs.insert(id.numeric());
  }
  decisionIDs( passExpressOutput ).insert( decisionIDs( passExpressOutput ).end(),
                                           expressIDs.begin(),
                                           expressIDs.end() ); // Save this to the output

  // Fill and write out a set of passed streams
  std::unordered_set<std::string> passStreamsSet;
  for (const DecisionID chainID : allPassingFinalIDs) {
    if (const auto it = m_chainToStreamsMap.find(chainID); it!=m_chainToStreamsMap.cend()) {
      for (const std::string& streamName : it->second) {
        passStreamsSet.insert(streamName);
      }
    } else {
      ATH_MSG_ERROR("Passed chain " << HLT::Identifier(chainID).name() << " not in m_chainToStreamsMap");
    }
  }
  // Add express
  if (!expressIDs.empty()) {
    passStreamsSet.insert(s_expressStreamName);
  }
  SG::WriteHandle<std::vector<std::string>> passStreams{m_streamsSummaryKey, context};
  ATH_CHECK(passStreams.record(std::make_unique<std::vector<std::string>>(passStreamsSet.begin(),passStreamsSet.end())));

  if (msgLvl(MSG::DEBUG)) {
    std::ostringstream streamNames;
    bool first{true};
    for (const std::string& s : passStreamsSet) {
      if (first) {first=false;}
      else {streamNames << ", ";}
      streamNames << s;
    }
    ATH_MSG_DEBUG("This event is accepted to " << passStreamsSet.size() << " streams: " << streamNames.str());
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

void DecisionSummaryMakerAlg::monitorRoIs(const TrigCompositeUtils::Decision* terminusNode) const {
  using namespace TrigCompositeUtils;
  using RoILinkVec = std::vector<LinkInfo<TrigRoiDescriptorCollection>>;

  auto printDecision = [this](const Decision* d){
    ATH_MSG_INFO("The decision corresponds to chain(s):");
    for (const DecisionID id : decisionIDs(d)) {
      ATH_MSG_INFO("-- " << HLT::Identifier(id).name());
    }
  };
  auto printDecisionAndRoI = [this,&printDecision](const Decision* d, const TrigRoiDescriptor& roi){
    printDecision(d);
    ATH_MSG_INFO("and final RoI: " << roi);
  };

  // Loop over all final RoIs
  const RoILinkVec allFinalRoIs = findLinks<TrigRoiDescriptorCollection>(terminusNode, roiString(), TrigDefs::lastFeatureOfType);
  for (const auto& finalRoILink : allFinalRoIs) {
    // Get the final TrigRoiDescriptor reference
    if (!finalRoILink.isValid() || *(finalRoILink.link)==nullptr) {
      ATH_MSG_WARNING("Encountered invalid final RoI link");
      printDecision(finalRoILink.source);
      continue;
    }
    const TrigRoiDescriptor& finalRoI = **(finalRoILink.link);

    // Skip full-scan
    if (finalRoI.isFullscan()) {continue;}

    // Get all initial RoIs associated with this final RoI (should be exactly one)
    const RoILinkVec initialRoIs = findLinks<TrigRoiDescriptorCollection>(finalRoILink.source, initialRoIString(), TrigDefs::lastFeatureOfType);

    // Warn if the number of initial RoIs differs from one
    if (initialRoIs.empty()) {
      ATH_MSG_WARNING("Encountered decision node with no " << initialRoIString() << " link");
      printDecisionAndRoI(finalRoILink.source, finalRoI);
      continue;
    }
    if (initialRoIs.size()>1) {
      ATH_MSG_WARNING("Encountered decision node with multiple (" << initialRoIs.size() << ") "
                      << initialRoIString() << " links");
      printDecisionAndRoI(finalRoILink.source, finalRoI);
    }

    // Get the initial TrigRoiDescriptor reference
    const auto& initialRoILink = initialRoIs.front();
    if (!initialRoILink.isValid() || *(initialRoILink.link)==nullptr) {
      ATH_MSG_WARNING("Encountered invalid initial RoI link");
      printDecisionAndRoI(finalRoILink.source, finalRoI);
      continue;
    }
    const TrigRoiDescriptor& initialRoI = **(initialRoILink.link);

    // Skip full-scan
    if (initialRoI.isFullscan()) {continue;}

    // Fill the monitoring histograms
    Monitored::Scalar dEta{"RoIsDEta", finalRoI.eta() - initialRoI.eta()};
    Monitored::Scalar dPhi{"RoIsDPhi", CxxUtils::deltaPhi(finalRoI.phi(), initialRoI.phi())};
    Monitored::Scalar dZed{"RoIsDZed", finalRoI.zed() - initialRoI.zed()};
    Monitored::Group(m_monTool, dEta, dPhi, dZed);

    // Print warnings on large updates
    if (m_warnOnLargeRoIUpdates.value()) {
      if (std::abs(dEta) > 1.0 || std::abs(dPhi) > 1.0 || std::abs(dZed) > 200) {
        ATH_MSG_WARNING("Large RoI difference between initial and final RoI: "
                        << "dEta=" << dEta << ", dPhi=" << dPhi << ", dZed=" << dZed
                        << "initialRoI: " << initialRoI << ", finalRoI: " << finalRoI);
        printDecision(finalRoILink.source);
      }
    }
  }
}
