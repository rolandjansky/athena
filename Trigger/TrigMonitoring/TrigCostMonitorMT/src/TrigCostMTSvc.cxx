/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCostMTSvc.h"

#include "GaudiKernel/ConcurrencyFlags.h"

/////////////////////////////////////////////////////////////////////////////

TrigCostMTSvc::TrigCostMTSvc(const std::string& name, ISvcLocator* pSvcLocator) :
base_class(name, pSvcLocator), // base_class = AthService
m_eventMonitored(),
m_algStartTimes(),
m_algStopTimes(),
m_algThreadID()
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
  ATH_MSG_INFO(m_eventSlots);
  // We cannot have a vector here as atomics are not movable nor copyable. Unique heap arrays are supported by C++
  m_eventMonitored = std::make_unique< std::atomic<bool>[] >( m_eventSlots );
  ATH_CHECK(m_algStartTimes.initialize(m_eventSlots));
  ATH_CHECK(m_algStopTimes.initialize(m_eventSlots));
  ATH_CHECK(m_algThreadID.initialize(m_eventSlots));
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::startEvent(const EventContext& context, const bool enableMonitoring) {
  const bool monitoredEvent = (enableMonitoring || m_monitorAll);
  ATH_CHECK(checkSlot(context));
  m_eventMonitored[ context.slot() ] = monitoredEvent; 
  if (monitoredEvent) {
    // Empty transient thread-safe stores in preparation for recording this event's cost data
    ATH_CHECK(m_algStartTimes.clear(context, msg()));
    ATH_CHECK(m_algStopTimes.clear(context, msg()));
    ATH_CHECK(m_algThreadID.clear(context, msg()));
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::processAlg(const EventContext& context, const std::string& caller, const AuditType type) {
  ATH_CHECK(checkSlot(context));
  if (!isMonitoredEvent(context)) return StatusCode::SUCCESS;

  if (type == AuditType::Before) {

    ATH_CHECK( m_algStartTimes.insert(context, caller, msg(), TrigTimeStamp()) );
    ATH_CHECK( m_algThreadID.insert(context, caller, msg(), std::this_thread::get_id()) );

    ATH_MSG_DEBUG("Caller '" << caller << "', '" << context.getExtension<Atlas::ExtendedEventContext>().proxy()->name() << "' began");

  } else if (type == AuditType::After) {

    ATH_CHECK( m_algStopTimes.insert(context, caller, msg(), TrigTimeStamp()) );
    ATH_MSG_DEBUG("Caller '" << caller << "', '" << context.getExtension<Atlas::ExtendedEventContext>().proxy()->name() << "' ended");

  }

  return StatusCode::SUCCESS;
} 

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::endEvent(const EventContext& context, SG::WriteHandle<xAOD::TrigCompositeContainer>& outputHandle) { 
  if (!isMonitoredEvent(context)) return StatusCode::SUCCESS;

  // Reset eventMonitored flag
  ATH_CHECK(checkSlot(context));
  m_eventMonitored[ context.slot() ] = false;

  // Read payloads. Write to persistent format
  tbb::concurrent_hash_map< AlgorithmIdentifier, TrigTimeStamp, AlgorithmIdentifierHashCompare>::const_iterator beginIt;
  tbb::concurrent_hash_map< AlgorithmIdentifier, TrigTimeStamp, AlgorithmIdentifierHashCompare>::const_iterator endIt;
  ATH_CHECK(m_algStartTimes.getIterators(context, msg(), beginIt, endIt));

  for (tbb::concurrent_hash_map< AlgorithmIdentifier, TrigTimeStamp, AlgorithmIdentifierHashCompare>::const_iterator it = beginIt; it != endIt; ++it) {
    const AlgorithmIdentifier& ai = it->first;
    const TrigTimeStamp& startTime = it->second;
    // Can we find the end time for this alg? Skip if not
    TrigTimeStamp stopTime;
    if (m_algStopTimes.retrieve(context, ai, msg(), stopTime).isFailure()) {
      ATH_MSG_DEBUG("No end time for '" << ai.m_caller << "', '" << ai.m_store << "'");
      continue;
    }
    std::thread::id threadID;
    if (m_algThreadID.retrieve(context, ai, msg(), threadID).isFailure()) {
      ATH_MSG_WARNING("Did not get thread ID for '" << ai.m_caller << "', '" << ai.m_store << "'");
      // Not fatal
    }

    // Make a new TrigComposite to persist monitoring payload for this alg
    xAOD::TrigComposite* tc = new xAOD::TrigComposite();
    (*outputHandle).push_back( tc ); 
    // tc is now owned by storegate and, and has an aux store provided by the TrigCompositeCollection

    tc->setName(ai.m_caller);
    bool result = true;
    
    result &= tc->setDetail("store", ai.m_store);
    result &= tc->setDetail("view", ai.m_viewID);
    result &= tc->setDetail("threadID", static_cast<uint32_t>( std::hash< std::thread::id >()(threadID) ));
    std::pair<uint32_t,uint32_t> startTimePair = startTime.secondsAndMicrosecondsSinceEpoch();
    std::pair<uint32_t,uint32_t> stopTimePair = stopTime.secondsAndMicrosecondsSinceEpoch();
    result &= tc->setDetail("startTimeSeconds", startTimePair.first);
    result &= tc->setDetail("startTimeMicroseconds", startTimePair.second);
    result &= tc->setDetail("stopTimeSeconds", stopTimePair.first);
    result &= tc->setDetail("stopTimeMicroseconds", stopTimePair.second);
    if (!result) ATH_MSG_WARNING("Failed to append one or more details to trigger cost TC");
  }

  if (m_printTimes && msg().level() <= MSG::INFO) {
    ATH_MSG_INFO("--- Trig Cost Event Summary ---");
    for ( const xAOD::TrigComposite* tc : *outputHandle ) {
      ATH_MSG_INFO("Algorithm:'" << tc->name() << "'");
      ATH_MSG_INFO("  Store:'" << tc->getDetail<std::string>("store") << "'");
      ATH_MSG_INFO("  View ID:" << tc->getDetail<int16_t>("view"));
      ATH_MSG_INFO("  Thread ID Hash:" << tc->getDetail<uint32_t>("threadID") );
      ATH_MSG_INFO("  Start Time Seconds:" << tc->getDetail<uint32_t>("startTimeSeconds") << " s");
      ATH_MSG_INFO("  Start Time Microseconds:" << tc->getDetail<uint32_t>("startTimeMicroseconds") << " mu s");
      ATH_MSG_INFO("  Stop Time Seconds:" << tc->getDetail<uint32_t>("stopTimeSeconds") << " s");
      ATH_MSG_INFO("  Stop Time Microseconds:" << tc->getDetail<uint32_t>("stopTimeMicroseconds") << " mu s");
    }
  }
  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::checkSlot(const EventContext& context) const {
  if (context.slot() >= m_eventSlots) {
    ATH_MSG_FATAL("Job is using more event slots (" << context.slot() << ") than we configured for, m_eventSlots = " << m_eventSlots);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool TrigCostMTSvc::isMonitoredEvent(const EventContext& context) const {
  return m_eventMonitored[ context.slot() ];
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
