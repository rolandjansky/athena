 /*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigByteStreamInputSvc.h"
#include "TrigKernel/HltExceptions.h"

#include "hltinterface/DataCollector.h"
#include "eformat/write/FullEventFragment.h"

// =============================================================================
// Standard constructor
// =============================================================================
TrigByteStreamInputSvc::TrigByteStreamInputSvc(const std::string& name, ISvcLocator* svcLoc)
: ByteStreamInputSvc(name, svcLoc),
  m_robDataProviderSvc("ROBDataProviderSvc", name) {}

// =============================================================================
// Standard destructor
// =============================================================================
TrigByteStreamInputSvc::~TrigByteStreamInputSvc() {}

// =============================================================================
// Implementation of IInterface::queryInterface
// =============================================================================
StatusCode TrigByteStreamInputSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  verbose() << "start of " << __FUNCTION__ << endmsg;

  if(ByteStreamInputSvc::interfaceID().versionMatch(riid))
    *ppvInterface = static_cast<ByteStreamInputSvc*>(this);
  else
    return AthService::queryInterface(riid, ppvInterface);

  addRef();
  verbose() << "end of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of Service::initialize
// =============================================================================
StatusCode TrigByteStreamInputSvc::initialize() {
  StatusCode sc = StatusCode::SUCCESS;

  sc = m_robDataProviderSvc.retrieve();
  if (sc.isFailure()) {
    error() << "Failed to retrieve the ROB data provider" << endmsg;
    return sc;
  }

  return sc;
}

// =============================================================================
// Implementation of Service::finalize
// =============================================================================
StatusCode TrigByteStreamInputSvc::finalize() {
  if (m_robDataProviderSvc.release().isFailure()) {
    warning() << "Cannot release rob data provider" << endmsg;
  }
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of ByteStreamInputSvc::nextEvent
// =============================================================================
const RawEvent* TrigByteStreamInputSvc::nextEvent() {
  std::lock_guard<std::mutex> lock( m_readerMutex );

  // not a nice solution - depends on the Gaudi::currentContext being set correctly upstream
  const EventContext context{ Gaudi::Hive::currentContext() };

  eformat::write::FullEventFragment l1r;
  hltinterface::DataCollector::Status status = hltinterface::DataCollector::instance()->getNext(l1r);
  if (status == hltinterface::DataCollector::Status::STOP) {
    debug() << "No more events available" << endmsg;
    throw hltonl::Exception::NoMoreEvents();
  }
  else if (status != hltinterface::DataCollector::Status::OK) {
    error() << "Unhandled return Status " << static_cast<int>(status) << " from DataCollector::getNext" << endmsg;
    return nullptr;
  }

  // convert write::FullEventFragment to read::FullEventFragment (RawEvent)
  const eformat::write::node_t* top = l1r.bind();
  const size_t l1rFragmentSize = l1r.size_word();
  uint32_t* buf = new uint32_t[l1rFragmentSize];
  auto copiedSize = eformat::write::copy(*top,buf,l1rFragmentSize);
  if(copiedSize!=l1rFragmentSize){
    error() << "Event serialization failed" << endmsg;
    return nullptr;
  }

  // newRawEvent points to memory allocated by buf
  RawEvent* newRawEvent = new RawEvent(buf);

  // find the cache corresponding to the current slot
  EventCache* cache = m_eventsCache.get(context);
  
  // free the memory allocated to the previous event processed in the current slot
  // -- if we make sure ROBDataProviderSvc does this, then TrigByteStreamInputSvc won't need a cache
  releaseEvent(cache);

  // put the new raw event into the cache
  cache->rawEvent = newRawEvent;

  m_robDataProviderSvc->setNextEvent(context,newRawEvent);
  return newRawEvent;
}

// =============================================================================
// Implementation of ByteStreamInputSvc::previousEvent
// =============================================================================
const RawEvent* TrigByteStreamInputSvc::previousEvent() {
  fatal() << "The method " << __FUNCTION__ << " is not implemented for online running" << endmsg;
  return nullptr;
}

// =============================================================================
// Implementation of ByteStreamInputSvc::currentEvent
// =============================================================================
const RawEvent* TrigByteStreamInputSvc::currentEvent() const {
  fatal() << "The method " << __FUNCTION__ << " is not implemented for online running" << endmsg;
  return nullptr;
}

// =============================================================================
void TrigByteStreamInputSvc::releaseEvent(EventCache* cache)
{
  if (cache->rawEvent) {
    OFFLINE_FRAGMENTS_NAMESPACE::PointerType fragment = cache->rawEvent->start();
    delete[] fragment;
    fragment = nullptr;
    delete cache->rawEvent;
    cache->rawEvent = nullptr;
    // cache->eventStatus = 0;
  }
}

// =============================================================================
TrigByteStreamInputSvc::EventCache::~EventCache() {
  delete rawEvent;
  rawEvent = 0;
}
