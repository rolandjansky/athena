/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#define BOOST_BIND_GLOBAL_PLACEHOLDERS // silence Boost pragma message (fixed in Boost 1.76)
#include <boost/property_tree/json_parser.hpp>

#include "GaudiKernel/ThreadLocalContext.h"
#include "TrigConfHLTUtils/HLTUtils.h"

#include "PathResolver/PathResolver.h"

#include "TrigCostAnalysis.h"
#include "CostData.h"

#include "monitors/MonitorAlgorithm.h"
#include "monitors/MonitorAlgorithmClass.h"
#include "monitors/MonitorGlobal.h"
#include "monitors/MonitorThreadOccupancy.h"
#include "monitors/MonitorROS.h"
#include "monitors/MonitorChain.h"
#include "monitors/MonitorChainAlgorithm.h"
#include "monitors/MonitorSequence.h"


TrigCostAnalysis::TrigCostAnalysis( const std::string& name, ISvcLocator* pSvcLocator ) :
  AthAlgorithm(name, pSvcLocator),
  m_metadataTree(nullptr),
  m_fullEventDumps(0),
  m_maxViewsNumber(0) {
}


StatusCode  TrigCostAnalysis::initialize() {
  ATH_MSG_VERBOSE("In initialize()");

  ATH_MSG_DEBUG("Reading from " << m_costDataKey.key() << ", " << m_HLTMenuKey.key());
  ATH_CHECK( m_costDataKey.initialize() );
  ATH_CHECK( m_rosDataKey.initialize() );
  ATH_CHECK( m_HLTMenuKey.initialize() );

  ATH_CHECK( m_metadataDataKey.initialize( SG::AllowEmpty ) );


  if (!m_enhancedBiasTool.name().empty()) {
    ATH_CHECK( m_enhancedBiasTool.retrieve() );
  } else {
    ATH_MSG_DEBUG("No EnhancedBiasTool. Not doing additional weighting.");
    if (m_useEBWeights) {
      ATH_MSG_FATAL("Configured to use Enhanced Bias weights. Must supply weighting tool.");
      return StatusCode::FAILURE;
    }
  }

  ATH_CHECK( m_TimeRangeLengthLB > 0 );

  if (not m_additionalHashMap.empty()) {
    std::string hashFile = PathResolverFindCalibFile( m_additionalHashMap );
    if (hashFile.empty()) {
      ATH_MSG_WARNING("Could not retrieve " << m_additionalHashMap << ", won't load additional hash maps.");
    } else {
      TrigConf::HLTUtils::file2hashes(hashFile);
    }
  }
  
  ATH_CHECK( m_histSvc->regTree("/COSTSTREAM/metadata", std::make_unique<TTree>("metadata", "metadata")) );
  ATH_CHECK( m_histSvc->getTree("/COSTSTREAM/metadata", m_metadataTree) );
  
  return StatusCode::SUCCESS;
}

StatusCode TrigCostAnalysis::start() {
  SG::ReadHandle<TrigConf::HLTMenu>  hltMenuHandle = SG::makeHandle( m_HLTMenuKey );
  ATH_CHECK( hltMenuHandle.isValid() );
  ATH_MSG_INFO("Configuring from " << m_HLTMenuKey << " with " << hltMenuHandle->size() << " chains");
  using boost::property_tree::ptree; 

  // Populate the reverse-hashing dictionary with all ALG NAMES.
  // Obtain a mapping from NAME to TYPE from the configuration JSON 
  m_storeIdentifiers.clear();
  const ptree& menuData =  hltMenuHandle->data();
  for (const auto& sequencer : menuData.get_child("sequencers")) {
    ATH_MSG_VERBOSE("Found Sequencer:" << sequencer.first);
    for (const auto& alg : sequencer.second) {
      // Data stored in Gaudi format of "AlgClassType/AlgInstanceName"
      size_t breakPoint = alg.second.data().find('/');
      std::string algType = alg.second.data().substr(0, breakPoint);
      std::string algName = alg.second.data().substr(breakPoint+1, alg.second.data().size());
      std::replace(algType.begin(), algType.end(), ':', '_');
      std::replace(algName.begin(), algName.end(), ':', '_');
      m_algTypeMap[ TrigConf::HLTUtils::string2hash(algName, "ALG") ] = algType;
      ATH_MSG_VERBOSE("AlgType:" << algType << ", AlgName:" << algName );
      if (algType.find("EventViewCreatorAlgorithm") != std::string::npos) {
        ATH_MSG_VERBOSE(algType << " is identified as a ViewCreator");
        m_storeIdentifiers.insert(algName);
      }
    }
  }

  // Call TrigConf::HLTUtils::string2hash(chain.name()) for all the chains to cache the hash to name mapping
  std::vector<TrigConf::Chain> chains;
  ATH_CHECK( m_algToChainTool->getAllChains(chains));
  for (const TrigConf::Chain& chain : chains) {
    HLT::Identifier(chain.name());
    TrigConf::HLTUtils::string2hash(chain.name());

    // Populate legs' names
    const size_t legsSize {chain.legMultiplicities().size()};
    for (size_t counter = 0; legsSize > 1 && counter < legsSize; ++counter){
      HLT::Identifier l = TrigCompositeUtils::createLegName(chain.namehash(), counter);
      TrigConf::HLTUtils::string2hash(l.name());
    }
  }

  // Save identifiers and classes for additional HLTJobOptions map
  if (not m_additionalHashList.empty()){
    for (const std::string& entry : m_additionalHashList){
      size_t breakPoint = entry.find('/');
      if (breakPoint != std::string::npos){
        std::string algType = entry.substr(0, breakPoint);
        const std::string algName = entry.substr(breakPoint+1, entry.size());
        std::replace(algType.begin(), algType.end(), ':', '_');
        m_algTypeMap[ TrigConf::HLTUtils::string2hash(algName, "ALG") ] = std::move(algType);
      } else {
        TrigConf::HLTUtils::string2hash(entry, "ALG");
      }
    }
  }

  // As an initial guess, 25 should be a good uper maximum for the number of expected View instances.
  ATH_CHECK( checkUpdateMaxView(60) );
  return StatusCode::SUCCESS;
}

// TODO. Do a proper atomic check of m_maxViewsNumber. Call this from within the Monitor code.
StatusCode TrigCostAnalysis::checkUpdateMaxView(const size_t max) {
  if (max <= m_maxViewsNumber) {
    return StatusCode::SUCCESS;
  }
  const size_t current = m_maxViewsNumber;
  m_maxViewsNumber = max;
  ATH_MSG_DEBUG("Extending maximum View instances from " << current << " to " << max);
  for (size_t viewID = current; viewID <= m_maxViewsNumber; ++ viewID) {
    // Allow for this many individual View instances
    for (const std::string& store : m_storeIdentifiers) {
      std::stringstream ss;
      ss << store << "_view_" << viewID;
      TrigConf::HLTUtils::string2hash(ss.str(), "STORE");
    }
    // And this many global Slots. Though, in general, it will be the Views which are driving this. 
    std::stringstream ss;
    ss << viewID << "_StoreGateSvc_Impl";
    TrigConf::HLTUtils::string2hash(ss.str(), "STORE");
  }
  return StatusCode::SUCCESS;
}

float TrigCostAnalysis::getWeight(const EventContext& context) {
  // TODO Prescale of CostMon chain for P1
  double ebWeight = 1.0;
  if (m_useEBWeights) {
    ebWeight = m_enhancedBiasTool->getEBWeight(context);
    ATH_MSG_DEBUG("EB Weight is " << ebWeight);
  }
  return m_baseEventWeight * ebWeight;
}


TH1* TrigCostAnalysis::bookGetPointer(TH1* hist, const std::string& tDir) const {
  std::string histName(hist->GetName());
  std::string bookingString = "/COSTSTREAM/" + tDir + "/" + histName;

  if (!((m_histSvc->regHist(bookingString, hist)).isSuccess())) {
    ATH_MSG_WARNING( "Problem registering histogram with name " << histName);
    return nullptr;
  }

  return hist;
}


StatusCode TrigCostAnalysis::execute() {
  const EventContext& context = Gaudi::Hive::currentContext();

  MonitoredRange* range = nullptr;
  ATH_CHECK(getRange(context, range));

  if (!range) {

    ATH_MSG_DEBUG("Not monitoring event");
    return StatusCode::SUCCESS;

  }

  ATH_MSG_DEBUG("Monitoring event " << context.eventID().event_number() << " in LB " << context.eventID().lumi_block() << " in range " << range->getName());
  
  SG::ReadHandle<xAOD::TrigCompositeContainer> costDataHandle(m_costDataKey, context);
  ATH_CHECK( costDataHandle.isValid() );

  SG::ReadHandle<xAOD::TrigCompositeContainer> rosDataHandle(m_rosDataKey, context);
  ATH_CHECK( rosDataHandle.isValid() );


  if (!m_metadataDataKey.empty()){
    SG::ReadHandle<xAOD::TrigCompositeContainer> metadataDataHandle(m_metadataDataKey, context);
    ATH_CHECK( metadataDataHandle.isValid() );

    for (const xAOD::TrigComposite* tc : *metadataDataHandle) {
      try {
        std::lock_guard<std::mutex> lock(m_addHostnameMutex);
        const std::string hostname = tc->getDetail<std::string>("hostname");
        m_hostnames.insert(hostname);
      } catch ( const std::exception& ) {
        ATH_MSG_WARNING("Missing HLT_TrigCostMetadataContainer EDM hostname for event " << context.eventID().event_number());
      }

    }
  }

  // Save indexes of algorithm in costDataHandle
  std::map<std::string, std::set<size_t>> chainToAlgIdx;
  std::map<std::string, std::set<size_t>> chainToUniqAlgs; // List for unique algorithms for each chain
  std::map<std::string, std::map<int16_t, std::set<size_t>>> seqToAlgIdx; // Map of algorithms split in views
  std::map<std::string, std::vector<TrigConf::Chain>> algToChain;
  ATH_CHECK( m_algToChainTool->getChainsForAllAlgs(context, algToChain) );

  // Retrieve active sequences and algorithms
  std::map<std::string, std::string> algToSeq;
  ATH_CHECK(m_algToChainTool->getAllActiveSequences(context, algToSeq));

  for (const xAOD::TrigComposite* tc : *costDataHandle) {
    const uint32_t nameHash = tc->getDetail<TrigConf::HLTHash>("alg");
    const std::string algName = TrigConf::HLTUtils::hash2string(nameHash, "ALG");

    size_t i = 0;
    for (const TrigConf::Chain& chain : algToChain[algName]){
      chainToAlgIdx[chain.name()].insert(tc->index());
      ++i;
    }

    if (i == 1){
      ATH_MSG_DEBUG("Algorithm " << algName << " executed uniquely for " << algToChain[algName][0].name() << " chain");
      chainToUniqAlgs[algToChain[algName][0].name()].insert(tc->index());
    }

    if (algToSeq.count(algName)){
      const int16_t view = tc->getDetail<int16_t>("view");
      seqToAlgIdx[algToSeq[algName]][view].insert(tc->index());
    }
  }

  const std::set<TrigCompositeUtils::DecisionID> seededChains = m_algToChainTool->retrieveActiveChains(context, "HLTNav_L1");
  std::vector<TrigCompositeUtils::AlgToChainTool::ChainInfo> seededChainsInfo;

  // Skip empty events, where only cost chain was active
  bool skipMonitoringThisEvent = false;
  if ((seededChains.size() == 1 && seededChains.count(TrigConf::HLTUtils::string2hash("HLT_noalg_CostMonDS_L1All")))
    || (seededChains.size() == 2 && seededChains.count(TrigConf::HLTUtils::string2hash("HLT_noalg_CostMonDS_L1All")) && seededChains.count(TrigConf::HLTUtils::string2hash("HLT_noalg_L1All"))) ){
    skipMonitoringThisEvent = true;
  }

  for (auto id : seededChains){
      TrigCompositeUtils::AlgToChainTool::ChainInfo chainInfo;
      ATH_CHECK(m_algToChainTool->getChainInfo(context, id, chainInfo));
      seededChainsInfo.push_back(chainInfo);
  }

  const uint32_t onlineSlot = getOnlineSlot( costDataHandle.get() );
  CostData costData;
  ATH_CHECK( costData.set(costDataHandle.get(), rosDataHandle.get(), onlineSlot) );
  costData.setRosToRobMap(m_rosToRob);
  costData.setChainToAlgMap(chainToAlgIdx);
  costData.setChainToUniqAlgMap(chainToUniqAlgs);
  costData.setSequencersMap(seqToAlgIdx);
  costData.setSeededChains(seededChainsInfo);
  costData.setLb( context.eventID().lumi_block() );
  costData.setTypeMap( m_algTypeMap );
  if (!m_enhancedBiasTool.name().empty() && !m_enhancedBiasTool->isMC()) {
    double liveTime = m_enhancedBiasTool->getEBLiveTime(context);
    bool liveTimeIsPerEvent = true;
    if (liveTime == 0.0) { // Note: This comes from a direct "return 0.", hence no delta
      liveTime = m_enhancedBiasTool->getLBLength(context);
      liveTimeIsPerEvent = false;
    }
    costData.setLivetime( liveTime, liveTimeIsPerEvent );
  }

  ATH_CHECK( range->newEvent( costData, getWeight(context), skipMonitoringThisEvent ) );

  if (checkDoFullEventDump(context, costData)) {
    ATH_CHECK( dumpEvent(context) );
  }

  return StatusCode::SUCCESS;
}


bool TrigCostAnalysis::checkDoFullEventDump(const EventContext& context, const CostData& costData) {
  if (costData.isMasterSlot()
    and m_fullEventDumps < m_maxFullEventDumps 
    and context.eventID().event_number() % m_fullEventDumpProbability == 0)
  {
    ++m_fullEventDumps;
    return true;
  }
  return false;
}


StatusCode TrigCostAnalysis::registerMonitors(MonitoredRange* range) {
  ATH_CHECK(range != nullptr);
  if (m_doMonitorAlgorithm) {
    ATH_CHECK( range->addMonitor(std::make_unique<MonitorAlgorithm>("Algorithm_HLT", range)) );
    ATH_MSG_DEBUG("Registering Algorithm_HLT Monitor for range " << range->getName() << ". Size:" << range->getMonitors().size());
  }
  if (m_doMonitorAlgorithmClass) {
    ATH_CHECK( range->addMonitor(std::make_unique<MonitorAlgorithmClass>("Algorithm_Class_HLT", range)) );
    ATH_MSG_DEBUG("Registering Algorithm_Class_HLT Monitor for range " << range->getName() << ". Size:" << range->getMonitors().size());
  }
  if (m_doMonitorGlobal) {
    ATH_CHECK( range->addMonitor(std::make_unique<MonitorGlobal>("Global_HLT", range)) );
    ATH_MSG_DEBUG("Registering Global_HLT Monitor for range " << range->getName() << ". Size:" << range->getMonitors().size());
  }
  if (m_doMonitorThreadOccupancy) {
    ATH_CHECK( range->addMonitor(std::make_unique<MonitorThreadOccupancy>("Thread_Occupancy_HLT", range)) );
    ATH_MSG_DEBUG("Registering Thread_Occupancy_HLT Monitor for range " << range->getName() << ". Size:" << range->getMonitors().size());
  }
  if (m_doMonitorROS) {
    ATH_CHECK( range->addMonitor(std::make_unique<MonitorROS>("ROS_HLT", range)) );
    ATH_MSG_DEBUG("Registering ROS_HLT Monitor for range " << range->getName() << ". Size:" << range->getMonitors().size());
  }
  if (m_doMonitorChain) {
    ATH_CHECK( range->addMonitor(std::make_unique<MonitorChain>("Chain_HLT", range)) );
    ATH_MSG_DEBUG("Registering Chain_HLT Monitor for range " << range->getName() << ". Size:" << range->getMonitors().size());
  }
  if (m_doMonitorChainAlgorithm) {
    ATH_CHECK( range->addMonitor(std::make_unique<MonitorChainAlgorithm>("Chain_Algorithm_HLT", range)) );
    ATH_MSG_DEBUG("Registering Chain_Algorihtm_HLT Monitor for range " << range->getName() << ". Size:" << range->getMonitors().size());
  }
  if (m_doMonitorSequence) {
    ATH_CHECK( range->addMonitor(std::make_unique<MonitorSequence>("Sequence_HLT", range)) );
    ATH_MSG_DEBUG("Registering Sequence_HLT Monitor for range " << range->getName() << ". Size:" << range->getMonitors().size());
  }
  // if (m_do...) {}
  
  // Set the verbosity for the monitors
  for (const std::unique_ptr<MonitorBase>& monitor : range->getMonitors()){
    monitor->msg().setLevel(msg().level());
  }

  return StatusCode::SUCCESS;
}


StatusCode TrigCostAnalysis::getRange(const EventContext& context, MonitoredRange*& range) {
  std::string rangeName;
  range = nullptr;
  bool includeEndOfLB = false;

  if (m_singleTimeRange) {
    rangeName = m_singleTimeRangeName;
  } else {
    const EventIDBase::number_type lumiBlock = context.eventID().lumi_block();
    const size_t lumiBlockRangeStart = lumiBlock - (lumiBlock % m_TimeRangeLengthLB);
    const size_t lumiBlockRangeStop  = lumiBlockRangeStart + m_TimeRangeLengthLB - 1;
    std::stringstream ss;
    ss << "LumiBlock_" << std::setfill('0') << std::setw(5) << lumiBlockRangeStart;
    if (includeEndOfLB && lumiBlockRangeStop != lumiBlockRangeStart) {
      ss << "_" << lumiBlockRangeStop;
    }
    rangeName = ss.str();
  }

  std::unordered_map<std::string, std::unique_ptr<MonitoredRange>>::iterator it;
  it = m_monitoredRanges.find(rangeName);

  // If we don't have a MonitoredRange with this name, try and make one.
  if (it == m_monitoredRanges.end()) {
    if (m_monitoredRanges.size() < m_maxTimeRange) {
      auto result = m_monitoredRanges.insert( 
        std::make_pair(rangeName, std::make_unique<MonitoredRange>(rangeName, this))
      );
      it = result.first; // Returns pair. First: map iterator. Second: insertion boolean
      ATH_CHECK(registerMonitors( it->second.get() ));
    } else {
      range = nullptr; // Not monitoring any more ranges
      return StatusCode::SUCCESS;
    }
  }

  range = it->second.get(); // Pointer to MonitoredRange
  return StatusCode::SUCCESS;
}

uint32_t TrigCostAnalysis::getOnlineSlot(const xAOD::TrigCompositeContainer* costCollection) const {
  // Online, Slot 0 can be configured as the master-slot. In this mode, events in Slot 0
  // hoover up data about algorithm executions in other slots too.
  // This all-slots data stored in the master slot is exploited by dumpEvent, and some specialist monitors.
  // Other monitors will want to avoid it for fear of double-counting 
  if (costCollection->size() == 0) {
    return 0;
  }
  const uint32_t initialSlot = costCollection->at(0)->getDetail<uint32_t>("slot");
  for ( const xAOD::TrigComposite* tc : *costCollection ) {
    const uint32_t algSlot = tc->getDetail<uint32_t>("slot");
    if (algSlot == 0 or algSlot != initialSlot) {
      return 0; // If we see a zero, or two different slot numbers, then we're in the master slot
    }
  }
  // If we saw exclusivly one non-zero slot, then we're in that slot
  return initialSlot;
}


StatusCode TrigCostAnalysis::dumpEvent(const EventContext& context) const {
  ++m_fullEventDumps;
  SG::ReadHandle<xAOD::TrigCompositeContainer> costDataHandle(m_costDataKey, context);

  std::stringstream ss;
  size_t algID = 0;

  for ( const xAOD::TrigComposite* tc : *costDataHandle ) {
    const uint64_t start = tc->getDetail<uint64_t>("start"); // in mus
    const uint64_t stop  = tc->getDetail<uint64_t>("stop"); // in mus
    const uint32_t slot  = tc->getDetail<uint32_t>("slot");
    const uint64_t start_ms_round = std::llround( start * 1e-3 ); // in ms
    const uint64_t stop_ms_round  = std::llround( stop  * 1e-3 ); // in ms
    const uint32_t threadID = tc->getDetail<uint32_t>("thread");

    ss << "{id:" << algID++;
    ss << ", group:" << threadID;
    ss << ", className:'slot" << slot << "'";
    ss << ", content:'" << TrigConf::HLTUtils::hash2string( tc->getDetail<TrigConf::HLTHash>("alg"), "ALG");
    ss << "<br>" << TrigConf::HLTUtils::hash2string( tc->getDetail<TrigConf::HLTHash>("store"), "STORE") << "'";
    ss << ", duration:" << (stop - start); // For tooltip display: in mus
    ss << ", start:" << start_ms_round; // in ms
    if (stop_ms_round > start_ms_round) {
      ss << ", end:" << stop_ms_round;
    } else {
      ss << ", type:'point'";
    }
    ss  << "}," << std::endl;
  }

  ATH_MSG_DEBUG("Full Event Summary for event " << context.eventID().event_number());
  ATH_MSG_DEBUG(ss.str());
  // TODO Persist this

  return StatusCode::SUCCESS;
}


StatusCode TrigCostAnalysis::finalize() {
  ATH_MSG_VERBOSE("In finalize()");

  writeMetadata();
  return StatusCode::SUCCESS;
}


void TrigCostAnalysis::writeMetadata() {
  if (!m_metadataTree) {
    return;
  }

  auto runNumber = m_enhancedBiasTool->getRunNumber();
  m_metadataTree->Branch("runNumber", &runNumber);

  std::string hostnamesList = "";
  if (m_hostnames.size() > 1){
    ATH_MSG_DEBUG("Found many hostnames for this run");
    for (auto name : m_hostnames) hostnamesList += name + ",";
    hostnamesList.pop_back();
  } else {
    hostnamesList = *m_hostnames.begin();
  }
  m_metadataTree->Branch("hostname", &hostnamesList);

  SG::ReadHandle<TrigConf::HLTMenu>  hltMenuHandle = SG::makeHandle( m_HLTMenuKey );
  std::string menuStr;
  if ( hltMenuHandle.isValid() ){
    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, hltMenuHandle->data());
    menuStr = ss.str();
    m_metadataTree->Branch("HLTMenu", &menuStr);
  }

  bool ChainMonitor = (const bool&) m_doMonitorChain;
  bool ChainAlgorithmMonitor = (const bool&) m_doMonitorChainAlgorithm;
  bool AlgorithmMonitor = (const bool&) m_doMonitorAlgorithm;
  bool AlgorithmClassMonitor = (const bool&) m_doMonitorAlgorithmClass;
  bool ROSMonitor = (const bool&) m_doMonitorROS;
  bool GlobalsMonitor = (const bool&) m_doMonitorGlobal;
  bool ThreadMonitor = (const bool&) m_doMonitorThreadOccupancy;

  m_metadataTree->Branch("ChainMonitor", &ChainMonitor);
  m_metadataTree->Branch("ChainAlgorithmMonitor", &ChainAlgorithmMonitor);
  m_metadataTree->Branch("AlgorithmMonitor", &AlgorithmMonitor);
  m_metadataTree->Branch("AlgorithmClassMonitor", &AlgorithmClassMonitor);
  m_metadataTree->Branch("ROSMonitor", &ROSMonitor);
  m_metadataTree->Branch("GlobalsMonitor", &GlobalsMonitor);
  m_metadataTree->Branch("ThreadMonitor", &ThreadMonitor);

  float BaseEventWeight = (const float&) m_baseEventWeight;
  std::string AdditionalHashMap = (const std::string&) m_additionalHashMap;
  bool DoEBWeighting = (const bool&) m_useEBWeights;

  m_metadataTree->Branch("AdditionalHashMap", &AdditionalHashMap);
  m_metadataTree->Branch("DoEBWeighting", &DoEBWeighting);
  m_metadataTree->Branch("BaseEventWeight", &BaseEventWeight);

  std::string atlasProject = std::getenv("AtlasProject");
  std::string atlasVersion = std::getenv("AtlasVersion");
  m_metadataTree->Branch("AtlasProject", &atlasProject);
  m_metadataTree->Branch("AtlasVersion", &atlasVersion);

  m_metadataTree->Fill();
}
