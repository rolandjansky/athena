/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Trigger includes
#include "TrigByteStreamInputSvc.h"
#include "TrigKernel/HltExceptions.h"

// Athena includes
#include "AthenaKernel/EventContextClid.h"
#include "EventInfo/EventInfo.h"
#include "StoreGate/StoreGateSvc.h"

// TDAQ includes
#include "hltinterface/DataCollector.h"
#include "eformat/write/FullEventFragment.h"

// =============================================================================
// Standard constructor
// =============================================================================
TrigByteStreamInputSvc::TrigByteStreamInputSvc(const std::string& name, ISvcLocator* svcLoc)
: ByteStreamInputSvc(name, svcLoc),
  m_robDataProviderSvc("ROBDataProviderSvc", name),
  m_evtStore("StoreGateSvc", name) {}

// =============================================================================
// Standard destructor
// =============================================================================
TrigByteStreamInputSvc::~TrigByteStreamInputSvc() {}

// =============================================================================
// Implementation of IInterface::queryInterface
// =============================================================================
StatusCode TrigByteStreamInputSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);

  if(ByteStreamInputSvc::interfaceID().versionMatch(riid))
    *ppvInterface = static_cast<ByteStreamInputSvc*>(this);
  else
    return AthService::queryInterface(riid, ppvInterface);

  addRef();
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of Service::initialize
// =============================================================================
StatusCode TrigByteStreamInputSvc::initialize() {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);

  CHECK(m_robDataProviderSvc.retrieve());
  CHECK(m_evtStore.retrieve());

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of Service::finalize
// =============================================================================
StatusCode TrigByteStreamInputSvc::finalize() {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  if (m_robDataProviderSvc.release().isFailure()) {
    ATH_MSG_WARNING("Cannot release rob data provider");
  }
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of ByteStreamInputSvc::nextEvent
// =============================================================================
const RawEvent* TrigByteStreamInputSvc::nextEvent() {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);

  // Get the EventContext via event store because the interface doesn't allow passing it explicitly as an argument
  // and we don't want to use ThreadLocalContext. Don't use ReadHandle here because it calls ThreadLocalContext if
  // not given a context (which we want to retrieve).
  const EventContext* eventContext = nullptr;
  if (m_evtStore->retrieve(eventContext).isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve EventContext from the event store, new event cannot be read");
    return nullptr;
  }

  ATH_MSG_DEBUG("Reading new event for event context " << *eventContext);

  // Find the cache corresponding to the current slot
  EventCache* cache = m_eventsCache.get(*eventContext);

  // Free the memory allocated to the previous event processed in the current slot
  cache->releaseEvent();

  using DCStatus = hltinterface::DataCollector::Status;
  DCStatus status = DCStatus::NO_EVENT;
  do {
    try {
      status = hltinterface::DataCollector::instance()->getNext(cache->rawData);
      if (status == DCStatus::NO_EVENT)
        ATH_MSG_ERROR("Failed to read new event, DataCollector::getNext returned Status::NO_EVENT. Trying again.");
    }
    catch (const std::exception& ex) {
      ATH_MSG_ERROR("Failed to read new event, caught an unexpected exception: " << ex.what()
                    << ". Throwing hltonl::Exception::EventSourceCorrupted" );
      throw hltonl::Exception::EventSourceCorrupted();
    }
    catch (...) {
      ATH_MSG_ERROR("Failed to read new event, caught an unexpected exception. "
                    << "Throwing hltonl::Exception::EventSourceCorrupted" );
      throw hltonl::Exception::EventSourceCorrupted();
    }
  } while (status == DCStatus::NO_EVENT);

  if (status == DCStatus::STOP) {
    ATH_MSG_DEBUG("No more events available");
    throw hltonl::Exception::NoMoreEvents();
  }
  else if (status != DCStatus::OK) {
    ATH_MSG_ERROR("Unhandled return Status " << static_cast<int>(status) << " from DataCollector::getNext");
    return nullptr;
  }
  ATH_MSG_VERBOSE("DataCollector::getNext returned Status::OK");

  // Create a cached FullEventFragment object from the cached raw data
  cache->fullEventFragment.reset(new RawEvent(cache->rawData.get()));

  // Give the FullEventFragment pointer to ROBDataProviderSvc and also return it
  m_robDataProviderSvc->setNextEvent(*eventContext, cache->fullEventFragment.get());
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return cache->fullEventFragment.get();
}

// =============================================================================
// Implementation of ByteStreamInputSvc::previousEvent
// =============================================================================
const RawEvent* TrigByteStreamInputSvc::previousEvent() {
  ATH_MSG_FATAL("The method " << __FUNCTION__ << " is not implemented for online running");
  return nullptr;
}

// =============================================================================
// Implementation of ByteStreamInputSvc::currentEvent
// =============================================================================
const RawEvent* TrigByteStreamInputSvc::currentEvent() const {
  ATH_MSG_FATAL("The method " << __FUNCTION__ << " is not implemented for online running");
  return nullptr;
}

// =============================================================================
void TrigByteStreamInputSvc::EventCache::releaseEvent() {
  if (this->rawData) {
    delete[] this->rawData.release();
  }
  if (this->fullEventFragment) {
    delete this->fullEventFragment.release();
  }
}

// =============================================================================
TrigByteStreamInputSvc::EventCache::~EventCache() {
  releaseEvent();
}
