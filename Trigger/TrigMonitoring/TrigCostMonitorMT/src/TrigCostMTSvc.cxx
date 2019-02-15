/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ConcurrencyFlags.h"

#include "TrigConfHLTData/HLTUtils.h"

#include "TrigCostMTSvc.h"

/////////////////////////////////////////////////////////////////////////////

TrigCostMTSvc::TrigCostMTSvc(const std::string& name, ISvcLocator* pSvcLocator) :
base_class(name, pSvcLocator), // base_class = AthService
m_eventMonitored(),
m_algStartTime(),
m_algStopTime(),
m_algThreadID(),
m_algROIID()
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
  ATH_MSG_INFO("Initializing TrigCostMTSvc with " << m_eventSlots << " event slots");
  // We cannot have a vector here as atomics are not movable nor copyable. Unique heap arrays are supported by C++
  m_eventMonitored = std::make_unique< std::atomic<bool>[] >( m_eventSlots );

  ATH_CHECK(m_algStartTime.initialize(m_eventSlots));
  ATH_CHECK(m_algStopTime.initialize(m_eventSlots));
  ATH_CHECK(m_algThreadID.initialize(m_eventSlots));
  ATH_CHECK(m_algROIID.initialize(m_eventSlots));

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::finalize() {
  ATH_MSG_DEBUG("TrigCostMTSvc finalize()");
  if (m_saveHashes) TrigConf::HLTUtils::hashes2file();
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::startEvent(const EventContext& context, const bool enableMonitoring) {
  const bool monitoredEvent = (enableMonitoring || m_monitorAll);
  ATH_CHECK(checkSlot(context));
  m_eventMonitored[ context.slot() ] = monitoredEvent; 
  if (monitoredEvent) {
    // Empty transient thread-safe stores in preparation for recording this event's cost data
    ATH_CHECK(m_algStartTime.clear(context, msg()));
    ATH_CHECK(m_algStopTime.clear(context, msg()));
    ATH_CHECK(m_algThreadID.clear(context, msg()));
    ATH_CHECK(m_algROIID.clear(context, msg()));
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::processAlg(const EventContext& context, const std::string& caller, const AuditType type) {
  ATH_CHECK(checkSlot(context));
  if (!isMonitoredEvent(context)) return StatusCode::SUCCESS;

  AlgorithmIdentifier ai = AlgorithmIdentifierMaker::make(context, caller, msg());
  ATH_CHECK( ai.isValid() );

  if (type == AuditType::Before) {

    ATH_CHECK( m_algStartTime.insert(ai, TrigTimeStamp()) );
    ATH_CHECK( m_algThreadID.insert(ai,  std::this_thread::get_id()) );
    ATH_CHECK( m_algROIID.insert(ai,     getROIID(context)) );

    ATH_MSG_DEBUG("Caller '" << caller << "', '" << ai.m_store << "', began");

  } else if (type == AuditType::After) {

    ATH_CHECK( m_algStopTime.insert(ai, TrigTimeStamp()) );

    ATH_MSG_DEBUG("Caller '" << caller << "', '" << ai.m_store << "', ended");

  }

  return StatusCode::SUCCESS;
} 

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::endEvent(const EventContext& context, SG::WriteHandle<xAOD::TrigCompositeContainer>& outputHandle) { 
  if (!isMonitoredEvent(context)) return StatusCode::SUCCESS;

  // Reset eventMonitored flag
  ATH_CHECK(checkSlot(context));
  m_eventMonitored[ context.slot() ] = false;
  // Now that this atomic is set to FALSE, additional algs in this event which trigger this service will return on the 
  // second line of TrigCostMTSvc::processAlg. 
  // Hence we can now perform whole-map inspection of this event's TrigCostDataStores

  // Read payloads. Write to persistent format
  tbb::concurrent_hash_map< AlgorithmIdentifier, TrigTimeStamp, AlgorithmIdentifierHashCompare>::const_iterator beginIt;
  tbb::concurrent_hash_map< AlgorithmIdentifier, TrigTimeStamp, AlgorithmIdentifierHashCompare>::const_iterator endIt;
  tbb::concurrent_hash_map< AlgorithmIdentifier, TrigTimeStamp, AlgorithmIdentifierHashCompare>::const_iterator it;
  ATH_CHECK(m_algStartTime.getIterators(context, msg(), beginIt, endIt));

  for (it = beginIt; it != endIt; ++it) {
    const AlgorithmIdentifier& ai = it->first;
    const TrigTimeStamp& startTime = it->second;
    // Can we find the end time for this alg? Skip if not
    TrigTimeStamp stopTime;
    if (m_algStopTime.retrieve(ai, stopTime).isFailure()) {
      ATH_MSG_DEBUG("No end time for '" << ai.m_caller << "', '" << ai.m_store << "'");
      continue;
    }
    // Can we find the threadID for this alg? It would be very strange not to be able to
    std::thread::id threadID;
    if (m_algThreadID.retrieve(ai, threadID).isFailure()) {
      ATH_MSG_WARNING("Did not get thread ID for '" << ai.m_caller << "', '" << ai.m_store << "'");
    }
    // Can we find the ROI for this alg's view? Note: it may not have an ROI
    int32_t ROIID = 0;
    if (m_algROIID.retrieve(ai, ROIID).isFailure()) {
      ATH_MSG_VERBOSE("Did not get ROI ID for '" << ai.m_caller << "', '" << ai.m_store << "'");
    }

    // Make a new TrigComposite to persist monitoring payload for this alg
    xAOD::TrigComposite* tc = new xAOD::TrigComposite();
    outputHandle->push_back( tc ); 
    // tc is now owned by storegate and, and has an aux store provided by the TrigCompositeCollection

    bool result = true;
    result &= tc->setDetail("alg", ai.callerHash());
    result &= tc->setDetail("store", ai.storeHash());
    result &= tc->setDetail("view", ai.m_viewID);
    result &= tc->setDetail("thread", static_cast<uint32_t>( std::hash< std::thread::id >()(threadID) ));
    result &= tc->setDetail("roi", ROIID);
    result &= tc->setDetail("start", startTime.microsecondsSinceEpoch());
    result &= tc->setDetail("stop", stopTime.microsecondsSinceEpoch());
    if (!result) ATH_MSG_WARNING("Failed to append one or more details to trigger cost TC");
  }

  if (m_printTimes && msg().level() <= MSG::INFO) {
    ATH_MSG_INFO("--- Trig Cost Event Summary ---");
    for ( const xAOD::TrigComposite* tc : *outputHandle ) {
      ATH_MSG_INFO("Algorithm:'" << TrigConf::HLTUtils::hash2string( tc->getDetail<TrigConf::HLTHash>("alg"), "ALG") << "'");
      ATH_MSG_INFO("  Store:'" << TrigConf::HLTUtils::hash2string( tc->getDetail<TrigConf::HLTHash>("store"), "STORE") << "'");
      ATH_MSG_INFO("  View ID:" << tc->getDetail<int16_t>("view"));
      ATH_MSG_INFO("  Thread ID Hash:" << tc->getDetail<uint32_t>("thread") );
      ATH_MSG_INFO("  RoI ID Hash:" << tc->getDetail<int32_t>("roi") );
      ATH_MSG_INFO("  Start Time:" << tc->getDetail<uint64_t>("start") << " mu s");
      ATH_MSG_INFO("  Stop Time:" << tc->getDetail<uint64_t>("stop") << " mu s");
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

int32_t TrigCostMTSvc::getROIID(const EventContext& context) {
  if (context.hasExtension<Atlas::ExtendedEventContext>()) {
    const TrigRoiDescriptor* roi = context.getExtension<Atlas::ExtendedEventContext>().roiDescriptor();
    // TODO check this line against a full build (get junk data locally)
    if (roi) return static_cast<int32_t>(roi->roiId());
  }
  return AlgorithmIdentifier::s_noView;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
