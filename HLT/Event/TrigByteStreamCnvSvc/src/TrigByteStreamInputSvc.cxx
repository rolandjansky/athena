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

  eformat::write::FullEventFragment l1r;
  using DCStatus = hltinterface::DataCollector::Status;
  auto status = DCStatus::NO_EVENT;
  do {
    status = hltinterface::DataCollector::instance()->getNext(l1r);
    if (status == DCStatus::NO_EVENT)
      ATH_MSG_ERROR("Failed to read new event, DataCollector::getNext returned Status::NO_EVENT. Trying again.");
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

  // convert write::FullEventFragment to read::FullEventFragment (RawEvent)
  const eformat::write::node_t* top = l1r.bind();
  const size_t l1rFragmentSize = l1r.size_word();
  uint32_t* buf = new uint32_t[l1rFragmentSize];
  auto copiedSize = eformat::write::copy(*top,buf,l1rFragmentSize);
  if(copiedSize!=l1rFragmentSize){
    ATH_MSG_ERROR("Event serialization failed");
    return nullptr;
  }

  // newRawEvent points to memory allocated by buf
  RawEvent* newRawEvent = new RawEvent(buf);

  // find the cache corresponding to the current slot
  EventCache* cache = m_eventsCache.get(*eventContext);

  // free the memory allocated to the previous event processed in the current slot
  // -- if we make sure ROBDataProviderSvc does this, then TrigByteStreamInputSvc won't need a cache
  releaseEvent(cache);

  // put the new raw event into the cache
  cache->rawEvent = newRawEvent;

  m_robDataProviderSvc->setNextEvent(*eventContext,newRawEvent);
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return newRawEvent;
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
void TrigByteStreamInputSvc::releaseEvent(EventCache* cache)
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  if (cache->rawEvent) {
    OFFLINE_FRAGMENTS_NAMESPACE::PointerType fragment = cache->rawEvent->start();
    delete[] fragment;
    fragment = nullptr;
    delete cache->rawEvent;
    cache->rawEvent = nullptr;
    // cache->eventStatus = 0;
  }
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
}

// =============================================================================
TrigByteStreamInputSvc::EventCache::~EventCache() {
  delete rawEvent;
  rawEvent = 0;
}
