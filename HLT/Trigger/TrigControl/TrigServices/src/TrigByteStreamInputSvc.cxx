 /*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigByteStreamInputSvc.h"

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
    return nullptr;
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
    // missing error handling
  }

  RawEvent* newRawEvent = new RawEvent(buf);

/* we will probably not use the InputSvc cache,
 * but pass the RawEvent ownership to ROBDataProvider
  // put the new raw event into cache
  EventCache* cache = m_eventsCache.get(context);
  releaseEvent(cache);
  cache->rawEvent = newRawEvent;
*/

/* ========== DEBUGGING A CRASH IN ROBDATAPROVIDER ==========
  debug() << "trying to get the first child from l1r" << endmsg;
  auto l1r_robf = l1r.first_child();
  debug() << "calling l1r_robf.rod_ndata()" << endmsg;
  uint32_t n = l1r_robf->rod_ndata(); // <----- THIS ALREADY CRASHES
  debug() << "l1r_robf->rod_ndata() = " << n << endmsg;

  uint32_t nchildren = newRawEvent->nchildren();
  debug() << "new raw event has " << nchildren << " children" << endmsg;
  for (uint32_t ich = 0; ich<nchildren; ++ich) {
    debug() << "trying to retrieve child " << ich << endmsg;
    const uint32_t* child = newRawEvent->child_check(ich);
    debug() << "retrieved child " << ich << " at address " << MSG::hex << child << MSG::dec << endmsg;
    debug() << "trying to create ROBFragment from the child pointer" << endmsg;
    OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment robf(child);
    debug() << "successfully created ROBFragment from the child pointer" << endmsg;
    debug() << "calling robf.rod_ndata()" << endmsg;
    n = robf.rod_ndata();
    debug() << "robf.rod_ndata() = " << n << endmsg;
  }
*/

  // temporary solution until crashes are resolved - memory leak here!
  // ROBDataProviderSvc implementation never deletes the raw event
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
