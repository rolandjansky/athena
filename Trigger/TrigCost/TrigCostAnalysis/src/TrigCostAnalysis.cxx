/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ThreadLocalContext.h"
#include "TrigConfHLTData/HLTUtils.h"

#include "PathResolver/PathResolver.h"

#include "TrigCostAnalysis.h"
#include "CostData.h"

#include "monitors/MonitorAlgorithm.h"
#include "monitors/MonitorAlgorithmClass.h"
#include "monitors/MonitorGlobal.h"
#include "monitors/MonitorThreadOccupancy.h"

TrigCostAnalysis::TrigCostAnalysis( const std::string& name, ISvcLocator* pSvcLocator ) :
  AthHistogramAlgorithm(name, pSvcLocator),
  m_fullEventDumps(0),
  m_maxViewsNumber(0) {
}


StatusCode  TrigCostAnalysis::initialize() {
  ATH_MSG_VERBOSE("In initialize()");

  ATH_MSG_DEBUG("Reading from " << m_costDataKey.key() << ", " << m_HLTMenuKey.key());
  ATH_CHECK( m_costDataKey.initialize() );
  ATH_CHECK( m_HLTMenuKey.initialize() );

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
      size_t breakPoint = alg.second.data().find("/");
      std::string algType = alg.second.data().substr(0, breakPoint);
      const std::string algName = alg.second.data().substr(breakPoint+1, alg.second.data().size());
      while(algType.find(":") != std::string::npos) {
        algType.replace(algType.find(":"), 1, "_");
      }
      m_algTypeMap[ TrigConf::HLTUtils::string2hash(algName, "ALG") ] = algType;
      ATH_MSG_VERBOSE("AlgType:" << algType << ", AlgName:" << algName );
      if (algType.find("EventViewCreatorAlgorithm") != std::string::npos) {
        ATH_MSG_VERBOSE(algType << " is identified as a ViewCreator");
        m_storeIdentifiers.insert(algName);
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


TH1* TrigCostAnalysis::bookGetPointer_fwd(TH1* hist, const std::string& tDir) {
  return bookGetPointer(hist, tDir);
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

  const uint32_t onlineSlot = getOnlineSlot( costDataHandle.get() );
  CostData costData;
  ATH_CHECK( costData.set(costDataHandle.get(), onlineSlot) );
  costData.setLb( context.eventID().lumi_block() );
  costData.setTypeMap( m_algTypeMap );
  if (!m_enhancedBiasTool.name().empty()) {
    double liveTime = m_enhancedBiasTool->getEBLiveTime(context);
    bool liveTimeIsPerEvent = true;
    if (liveTime == 0.0) { // Note: This comes from a direct "return 0.", hence no delta
      liveTime = m_enhancedBiasTool->getLBLength(context);
      liveTimeIsPerEvent = false;
    }
    costData.setLivetime( liveTime, liveTimeIsPerEvent );
  }

  ATH_CHECK( range->newEvent( costData, getWeight(context) ) );

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
  // if (m_do...) {}
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
  return StatusCode::SUCCESS;
}
