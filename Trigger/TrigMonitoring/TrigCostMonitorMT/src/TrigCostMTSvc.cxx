/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ConcurrencyFlags.h"

#include "TrigConfHLTData/HLTUtils.h"

#include "TrigCostMTSvc.h"

#include <mutex>  // For std::unique_lock

/////////////////////////////////////////////////////////////////////////////

TrigCostMTSvc::TrigCostMTSvc(const std::string& name, ISvcLocator* pSvcLocator) :
base_class(name, pSvcLocator), // base_class = AthService
m_eventSlots(),
m_eventMonitored(),
m_slotMutex(),
m_globalMutex(),
m_algStartInfo(),
m_algStopTime(),
m_threadToAlgMap(),
m_threadToCounterMap(),
m_threadCounter(0)
{
  ATH_MSG_DEBUG("TrigCostMTSvc regular constructor");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

TrigCostMTSvc::~TrigCostMTSvc() {
  // delete[] m_eventMonitored;
  ATH_MSG_DEBUG("TrigCostMTSvc destructor()");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


StatusCode TrigCostMTSvc::initialize() {
  ATH_MSG_DEBUG("TrigCostMTSvc initialize()");
  m_eventSlots = Gaudi::Concurrency::ConcurrencyFlags::numConcurrentEvents();
  // TODO Remove this when the configuration is correctly propagated in config-then-run jobs
  if (!m_eventSlots) {
    ATH_MSG_WARNING("numConcurrentEvents() == 0. This is a misconfiguration, probably coming from running from pickle. "
      "Setting local m_eventSlots to a 'large' number until this is fixed to allow the job to proceed.");
    m_eventSlots = 100;
  }
  ATH_MSG_INFO("Initializing TrigCostMTSvc with " << m_eventSlots << " event slots");

  // We cannot have a vector here as atomics are not movable nor copyable. Unique heap arrays are supported by C++
  m_eventMonitored = std::make_unique< std::atomic<bool>[] >( m_eventSlots );
  m_slotMutex = std::make_unique< std::shared_mutex[] >( m_eventSlots );

  for (size_t i = 0; i < m_eventSlots; ++i) m_eventMonitored[i] = false;

  ATH_CHECK(m_algStartInfo.initialize(m_eventSlots));
  ATH_CHECK(m_algStopTime.initialize(m_eventSlots));

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::finalize() {
  ATH_MSG_DEBUG("TrigCostMTSvc finalize()");
  if (m_saveHashes) {
    TrigConf::HLTUtils::hashes2file();
    ATH_MSG_INFO("Calling hashes2file, saving dump of job's HLT hashing dictionary to disk.");
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::startEvent(const EventContext& context, const bool enableMonitoring) {
  const bool monitoredEvent = (enableMonitoring || m_monitorAllEvents);
  ATH_CHECK(checkSlot(context));

  m_eventMonitored[ context.slot() ] = false;

  {
    // "clear" is a whole table operation, we need it all to ourselves
    std::unique_lock lockUnique( m_slotMutex[ context.slot() ] );
    if (monitoredEvent) {
      // Empty transient thread-safe stores in preparation for recording this event's cost data
      ATH_CHECK(m_algStartInfo.clear(context, msg()));
      ATH_CHECK(m_algStopTime.clear(context, msg()));
    }

    // Enable collection of data in this slot for monitoredEvents
    m_eventMonitored[ context.slot() ] = monitoredEvent;
  }

  // As we missed the AuditType::Before of the L1Decoder (which is calling this TrigCostMTSvc::startEvent), let's add it now.
  // This will be our canonical initial timestamps for measuring this event. Similar will be done for DecisionSummaryMakerAlg at the end
  ATH_CHECK(processAlg(context, m_l1DecoderName, AuditType::Before));

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::processAlg(const EventContext& context, const std::string& caller, const AuditType type) {
  ATH_CHECK(checkSlot(context));

  TrigTimeStamp now;

  // Do per-event within-slot monitoring
  if (m_eventMonitored[ context.slot() ]) {
    // Multiple simultaneous calls allowed here, adding their data to the concurrent map.
    std::shared_lock lockShared( m_slotMutex[ context.slot() ] );

    AlgorithmIdentifier ai = AlgorithmIdentifierMaker::make(context, caller, msg());
    ATH_CHECK( ai.isValid() );

    ATH_CHECK(monitor(context, ai, now, type));

    ATH_MSG_VERBOSE("Caller '" << caller << "', '" << ai.m_store << "', slot:" << context.slot() << " "
      << (type == AuditType::Before ? "BEGAN" : "ENDED") << " at " << now.microsecondsSinceEpoch());
  }

  // MultiSlot mode: do per-event monitoring of all slots, but saving the data within the master-slot
  if (m_enableMultiSlot && context.slot() != m_masterSlot && m_eventMonitored[ m_masterSlot ]) {
    std::shared_lock lockShared( m_slotMutex[ m_masterSlot ] );

    // Note: we override the storage location of these data from all other slots to be saved in the MasterSlot
    AlgorithmIdentifier ai = AlgorithmIdentifierMaker::make(context, caller, msg(), m_masterSlot);
    ATH_CHECK( ai.isValid() );

    ATH_CHECK(monitor(context, ai, now, type));
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::monitor(const EventContext& context, const AlgorithmIdentifier& ai, const TrigTimeStamp& now, const AuditType type) {

  if (type == AuditType::Before) {

    AlgorithmPayload ap {
      now,
      std::this_thread::get_id(),
      getROIID(context),
      static_cast<uint32_t>(context.slot())
    };
    ATH_CHECK( m_algStartInfo.insert(ai, ap) );

    // Cache the AlgorithmIdentifier which has just started executing on this thread
    tbb::concurrent_hash_map<std::thread::id, AlgorithmIdentifier, ThreadHashCompare>::accessor acc;
    m_threadToAlgMap.insert(acc, ap.m_algThreadID);
    acc->second = ai;

  } else if (type == AuditType::After) {

    ATH_CHECK( m_algStopTime.insert(ai, now) );

  } else {

    ATH_MSG_ERROR("Only expecting AuditType::Before or AuditType::After");
    return StatusCode::FAILURE;

  }

  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::endEvent(const EventContext& context, SG::WriteHandle<xAOD::TrigCompositeContainer>& outputHandle) { 
  ATH_CHECK(checkSlot(context));
  if (m_eventMonitored[ context.slot() ] == false) {
    // This event was not monitored - nothing to do.
    ATH_MSG_DEBUG("Not a monitored event.");
    return StatusCode::SUCCESS;
  }

  // As we will miss the AuditType::After of the DecisionSummaryMakerAlg (which is calling this TrigCostMTSvc::endEvent), let's add it now.
  // This will be our canonical final timestamps for measuring this event. Similar was done for L1Decoder at the start
  ATH_CHECK(processAlg(context, m_decisionSummaryMakerAlgName, AuditType::After));

  // Reset eventMonitored flags
  m_eventMonitored[ context.slot() ] = false;

  // Now that this atomic is set to FALSE, additional algs in this instance which trigger this service will 
  // not be able to call TrigCostMTSvc::monitor

  // ... but processAlg might already be running in other threads... 
  // Wait to obtain an exclusive lock.
  std::unique_lock lockUnique( m_slotMutex[ context.slot() ] );
  
  // we can now perform whole-map inspection of this event's TrigCostDataStores without the danger that it will be changed further

  // Let's start by getting the global STOP time we just wrote
  uint64_t eventStopTime = 0;
  {
    const AlgorithmIdentifier myAi = AlgorithmIdentifierMaker::make(context, m_decisionSummaryMakerAlgName, msg());
    ATH_CHECK( myAi.isValid() );
    tbb::concurrent_hash_map<AlgorithmIdentifier, TrigTimeStamp, AlgorithmIdentifierHashCompare>::const_accessor stopTimeAcessor;
    if (m_algStopTime.retrieve(myAi, stopTimeAcessor).isFailure()) {
      ATH_MSG_ERROR("No end time for '" << myAi.m_caller << "', '" << myAi.m_store << "'"); // Error as we JUST entered this info!
    } else { // retrieve was a success
      eventStopTime = stopTimeAcessor->second.microsecondsSinceEpoch();
    }
  }

  // And the global START time for the event
  uint64_t eventStartTime = 0;
  {
    const AlgorithmIdentifier l1DecoderAi = AlgorithmIdentifierMaker::make(context, m_l1DecoderName, msg());
    ATH_CHECK( l1DecoderAi.isValid() );
    tbb::concurrent_hash_map<AlgorithmIdentifier, AlgorithmPayload, AlgorithmIdentifierHashCompare>::const_accessor startAcessor;
    if (m_algStartInfo.retrieve(l1DecoderAi, startAcessor).isFailure()) {
      ATH_MSG_ERROR("No alg info for '" << l1DecoderAi.m_caller << "', '" << l1DecoderAi.m_store << "'"); // Error as we know this info must be present
    } else { // retrieve was a success
      eventStartTime = startAcessor->second.m_algStartTime.microsecondsSinceEpoch();
    }
  }

  // Read payloads. Write to persistent format
  tbb::concurrent_hash_map< AlgorithmIdentifier, AlgorithmPayload, AlgorithmIdentifierHashCompare>::const_iterator beginIt;
  tbb::concurrent_hash_map< AlgorithmIdentifier, AlgorithmPayload, AlgorithmIdentifierHashCompare>::const_iterator endIt;
  tbb::concurrent_hash_map< AlgorithmIdentifier, AlgorithmPayload, AlgorithmIdentifierHashCompare>::const_iterator it;
  ATH_CHECK(m_algStartInfo.getIterators(context, msg(), beginIt, endIt));

  ATH_MSG_DEBUG("Monitored event with " << std::distance(beginIt, endIt) << " AlgorithmPayload objects.");

  for (it = beginIt; it != endIt; ++it) {
    const AlgorithmIdentifier& ai = it->first;
    const AlgorithmPayload& ap = it->second;
    uint64_t startTime = ap.m_algStartTime.microsecondsSinceEpoch();

    // Can we find the end time for this alg? If not, it is probably still running. Hence we use "now" as the default time.
    uint64_t stopTime = eventStopTime;
    {
      tbb::concurrent_hash_map<AlgorithmIdentifier, TrigTimeStamp, AlgorithmIdentifierHashCompare>::const_accessor stopTimeAcessor;
      if (m_algStopTime.retrieve(ai, stopTimeAcessor).isFailure()) {
        ATH_MSG_DEBUG("No end time for '" << ai.m_caller << "', '" << ai.m_store << "'");
      } else { // retrieve was a success
        stopTime = stopTimeAcessor->second.microsecondsSinceEpoch();
      }
      // stopTimeAcessor goes out of scope - lock released
    }

    // It is possible (when in the master-slot) to catch just the END of an Alg's exec from another slot, and then the START of the same
    // alg executing in the next event in that same other-slot.
    // This gives us an end time which is before the start time. Disregard these entries.
    if (startTime > stopTime) {
      ATH_MSG_VERBOSE("Disregard start-time:" << startTime << " > stop-time:" << stopTime 
        << " for " << TrigConf::HLTUtils::hash2string( ai.callerHash(), "ALG") << " in slot " << ap.m_slot << ", this is slot " << context.slot());
      continue;
    }

    // Lock the start and stop times to be no later than eventStopTime.
    // E.g. it's possible for an alg in another slot to start or stop running after 'processAlg(context, m_decisionSummaryMakerAlgName, AuditType::After))'
    // but before 'lockUnique( m_slotMutex[ context.slot() ] )', creating a timestamp after the nominal end point for this event.
    // If the alg starts afterwards, we disregard it in lieu of setting to have zero walltime.
    // If the alg stops afterwards, we truncate its stop time to be no later than eventStopTime
    if (startTime > eventStopTime) {
      ATH_MSG_VERBOSE("Disregard " << TrigConf::HLTUtils::hash2string( ai.callerHash(), "ALG") << " as it started after endEvent() was finished being called" );
      continue;
    }
    if (stopTime > eventStopTime) {
      ATH_MSG_VERBOSE(TrigConf::HLTUtils::hash2string( ai.callerHash(), "ALG") << " stopped after endEvent() was called, but before the cost container was locked," 
        << " truncating its ending time stamp from " << stopTime << " to " << eventStopTime);
      stopTime = eventStopTime;
    }

    // Do the same, locking the start and stop times to be no earlier than eventStartTime
    // If the alg stops before eventStartTime, we disregard it in lieu of setting it to have zero walltime
    // If the alg starts before eventStartTime, we truncate its start time to be no later than eventStopTime
    if (stopTime < eventStartTime) {
      ATH_MSG_VERBOSE("Disregard " << TrigConf::HLTUtils::hash2string( ai.callerHash(), "ALG") << " as it stopped before startEvent() was finished being called" );
      continue;
    }
    if (startTime < eventStartTime) {
      ATH_MSG_VERBOSE(TrigConf::HLTUtils::hash2string( ai.callerHash(), "ALG") << " started just after the cost container was unlocked, but before the L1Decoder record was written." 
        << " truncating its starting time stamp from " << startTime << " to " << eventStartTime);
      startTime = eventStartTime;
    }

    // Make a new TrigComposite to persist monitoring payload for this alg
    xAOD::TrigComposite* tc = new xAOD::TrigComposite();
    outputHandle->push_back( tc ); 
    // tc is now owned by storegate and, and has an aux store provided by the TrigCompositeCollection

    const uint32_t threadID = static_cast<uint32_t>( std::hash< std::thread::id >()(ap.m_algThreadID) );
    uint32_t threadEnumerator = 0; 
    {
      // We can have multiple slots get here at the same time
      std::lock_guard<std::mutex> lock(m_globalMutex);
      const std::unordered_map<uint32_t, uint32_t>::const_iterator mapIt = m_threadToCounterMap.find(threadID);
      if (mapIt == m_threadToCounterMap.end()) {
        threadEnumerator = m_threadCounter;
        m_threadToCounterMap.insert( std::make_pair(threadID, m_threadCounter++) );
      } else {
        threadEnumerator = mapIt->second;
      }
    }

    bool result = true;
    result &= tc->setDetail("alg", ai.callerHash());
    result &= tc->setDetail("store", ai.storeHash());
    result &= tc->setDetail("view", ai.m_viewID);
    result &= tc->setDetail("thread", threadEnumerator);
    result &= tc->setDetail("thash", threadID);
    result &= tc->setDetail("slot", ap.m_slot);
    result &= tc->setDetail("roi", ap.m_algROIID);
    result &= tc->setDetail("start", startTime);
    result &= tc->setDetail("stop", stopTime);
    if (!result) ATH_MSG_WARNING("Failed to append one or more details to trigger cost TC");
  }

  if (msg().level() <= MSG::VERBOSE) {
    ATH_MSG_VERBOSE("--- Trig Cost Event Summary ---");
    for ( const xAOD::TrigComposite* tc : *outputHandle ) {
      ATH_MSG_VERBOSE("Algorithm:'" << TrigConf::HLTUtils::hash2string( tc->getDetail<TrigConf::HLTHash>("alg"), "ALG") << "'");
      ATH_MSG_VERBOSE("  Store:'" << TrigConf::HLTUtils::hash2string( tc->getDetail<TrigConf::HLTHash>("store"), "STORE") << "'");
      ATH_MSG_VERBOSE("  View ID:" << tc->getDetail<int16_t>("view"));
      ATH_MSG_VERBOSE("  Thread #:" << tc->getDetail<uint32_t>("thread") );
      ATH_MSG_VERBOSE("  Thread ID Hash:" << tc->getDetail<uint32_t>("thash") );
      ATH_MSG_VERBOSE("  Slot:" << tc->getDetail<uint32_t>("slot") );
      ATH_MSG_VERBOSE("  RoI ID Hash:" << tc->getDetail<int32_t>("roi") );
      ATH_MSG_VERBOSE("  Start Time:" << tc->getDetail<uint64_t>("start") << " mu s");
      ATH_MSG_VERBOSE("  Stop Time:" << tc->getDetail<uint64_t>("stop") << " mu s");
    }
  }
  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::checkSlot(const EventContext& context) const {
  if (context.slot() >= m_eventSlots) {
    ATH_MSG_FATAL("Job is using event slot #" << context.slot() << ", but we only reserved space for: " << m_eventSlots);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

int32_t TrigCostMTSvc::getROIID(const EventContext& context) {
  if (Atlas::hasExtendedEventContext(context)) {
    const TrigRoiDescriptor* roi = Atlas::getExtendedEventContext(context).roiDescriptor();
    if (roi) return static_cast<int32_t>(roi->roiId());
  }
  return AlgorithmIdentifier::s_noView;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool TrigCostMTSvc::isMonitoredEvent(const EventContext& context) const {
  if (m_eventMonitored[ context.slot() ]) {
    return true;
  }
  if (m_enableMultiSlot) {
    return m_eventMonitored[ m_masterSlot ];
  }
  return false;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

size_t TrigCostMTSvc::ThreadHashCompare::hash(const std::thread::id& thread) {
  return static_cast<size_t>( std::hash< std::thread::id >()(thread) );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool TrigCostMTSvc::ThreadHashCompare::equal(const std::thread::id& x, const std::thread::id& y) {
  return (x == y);
}
