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
m_eventMonitored(),
m_algStartInfo(),
m_algStopTime()
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

  ATH_CHECK(m_algStartInfo.initialize(m_eventSlots));
  ATH_CHECK(m_algStopTime.initialize(m_eventSlots));

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
  const bool monitoredEvent = (enableMonitoring || m_monitorAllEvents);
  ATH_CHECK(checkSlot(context));
  // "clear" is a whole table operation, we need it all to ourselves
  std::unique_lock lockUnique( m_slotMutex[ context.slot() ] );
  if (monitoredEvent) {
    // Empty transient thread-safe stores in preparation for recording this event's cost data
    ATH_CHECK(m_algStartInfo.clear(context, msg()));
    ATH_CHECK(m_algStopTime.clear(context, msg()));
  }
  // Now allow processAlg to start populating the 
  m_eventMonitored[ context.slot() ] = monitoredEvent;
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::processAlg(const EventContext& context, const std::string& caller, const AuditType type) {
  ATH_CHECK(checkSlot(context));
  if (!isMonitoredEvent(context)) return StatusCode::SUCCESS;

  // Multiple simultaneous calls allowed here, adding their data to the concurrent map.
  std::shared_lock lockShared( m_slotMutex[ context.slot() ] );

  AlgorithmIdentifier ai = AlgorithmIdentifierMaker::make(context, caller, msg());
  ATH_CHECK( ai.isValid() );

  if (type == AuditType::Before) {

    AlgorithmPayload ap {
      TrigTimeStamp(),
      std::this_thread::get_id(),
      getROIID(context),
      static_cast<uint32_t>(context.slot())
    };

    ATH_CHECK( m_algStartInfo.insert(ai, ap) );

    ATH_MSG_DEBUG("Caller '" << caller << "', '" << ai.m_store << "', began");

  } else if (type == AuditType::After) {

    ATH_CHECK( m_algStopTime.insert(ai, TrigTimeStamp()) );

    ATH_MSG_DEBUG("Caller '" << caller << "', '" << ai.m_store << "', ended");

  }

  return StatusCode::SUCCESS;
} 

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::endEvent(const EventContext& context, SG::WriteHandle<xAOD::TrigCompositeContainer>& outputHandle) { 
  ATH_CHECK(checkSlot(context));
  if (!isMonitoredEvent(context)) return StatusCode::SUCCESS;

  // Reset eventMonitored flag
  m_eventMonitored[ context.slot() ] = false;
  // Now that this atomic is set to FALSE, additional algs in this event which trigger this service will return on the 
  // second line of TrigCostMTSvc::processAlg. 

  // ... but processAlg might already be running in other threads... 
  // Wait to obtain an exclusive lock.
  std::unique_lock lockUnique( m_slotMutex[ context.slot() ] );
  
  // we can now perform whole-map inspection of this event's TrigCostDataStores without the danger that it will be changed further

  // Read payloads. Write to persistent format
  tbb::concurrent_hash_map< AlgorithmIdentifier, AlgorithmPayload, AlgorithmIdentifierHashCompare>::const_iterator beginIt;
  tbb::concurrent_hash_map< AlgorithmIdentifier, AlgorithmPayload, AlgorithmIdentifierHashCompare>::const_iterator endIt;
  tbb::concurrent_hash_map< AlgorithmIdentifier, AlgorithmPayload, AlgorithmIdentifierHashCompare>::const_iterator it;
  ATH_CHECK(m_algStartInfo.getIterators(context, msg(), beginIt, endIt));

  for (it = beginIt; it != endIt; ++it) {
    const AlgorithmIdentifier& ai = it->first;
    const AlgorithmPayload& ap = it->second;

    // Can we find the end time for this alg? Skip if not
    TrigTimeStamp stopTime;
    if (m_algStopTime.retrieve(ai, stopTime).isFailure()) {
      ATH_MSG_DEBUG("No end time for '" << ai.m_caller << "', '" << ai.m_store << "'");
      continue;
    }

    // Make a new TrigComposite to persist monitoring payload for this alg
    xAOD::TrigComposite* tc = new xAOD::TrigComposite();
    outputHandle->push_back( tc ); 
    // tc is now owned by storegate and, and has an aux store provided by the TrigCompositeCollection

    bool result = true;
    result &= tc->setDetail("alg", ai.callerHash());
    result &= tc->setDetail("store", ai.storeHash());
    result &= tc->setDetail("view", ai.m_viewID);
    result &= tc->setDetail("thread", static_cast<uint32_t>( std::hash< std::thread::id >()(ap.m_algThreadID) ));
    result &= tc->setDetail("slot", ap.m_slot);
    result &= tc->setDetail("roi", ap.m_algROIID);
    result &= tc->setDetail("start", ap.m_algStartTime.microsecondsSinceEpoch());
    result &= tc->setDetail("stop", stopTime.microsecondsSinceEpoch());
    if (!result) ATH_MSG_WARNING("Failed to append one or more details to trigger cost TC");
  }

  if (msg().level() <= MSG::VERBOSE) {
    ATH_MSG_INFO("--- Trig Cost Event Summary ---");
    for ( const xAOD::TrigComposite* tc : *outputHandle ) {
      ATH_MSG_VERBOSE("Algorithm:'" << TrigConf::HLTUtils::hash2string( tc->getDetail<TrigConf::HLTHash>("alg"), "ALG") << "'");
      ATH_MSG_VERBOSE("  Store:'" << TrigConf::HLTUtils::hash2string( tc->getDetail<TrigConf::HLTHash>("store"), "STORE") << "'");
      ATH_MSG_VERBOSE("  View ID:" << tc->getDetail<int16_t>("view"));
      ATH_MSG_VERBOSE("  Thread ID Hash:" << tc->getDetail<uint32_t>("thread") );
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

bool TrigCostMTSvc::isMonitoredEvent(const EventContext& context) const {
  return m_eventMonitored[ context.slot() ];
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

int32_t TrigCostMTSvc::getROIID(const EventContext& context) {
  if (context.hasExtension<Atlas::ExtendedEventContext>()) {
    const TrigRoiDescriptor* roi = context.getExtension<Atlas::ExtendedEventContext>().roiDescriptor();
    if (roi) return static_cast<int32_t>(roi->roiId());
  }
  return AlgorithmIdentifier::s_noView;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
