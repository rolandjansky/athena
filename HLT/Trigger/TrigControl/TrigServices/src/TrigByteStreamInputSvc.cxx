 /*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigByteStreamInputSvc.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "EventInfo/EventInfo.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrigKernel/HltExceptions.h"

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
  std::lock_guard<std::mutex> lock( m_readerMutex ); // probably don't need a lock for InputSvc

  // not a nice solution - depends on the Gaudi::currentContext being set correctly upstream
  const EventContext context{ Gaudi::Hive::currentContext() };

  eformat::write::FullEventFragment l1r;
  hltinterface::DataCollector::Status status = hltinterface::DataCollector::instance()->getNext(l1r);
  if (status == hltinterface::DataCollector::Status::STOP) {
    ATH_MSG_DEBUG("No more events available");
    throw hltonl::Exception::NoMoreEvents();
  }
  else if (status != hltinterface::DataCollector::Status::OK) {
    ATH_MSG_ERROR("Unhandled return Status " << static_cast<int>(status) << " from DataCollector::getNext");
    return nullptr;
  }

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
  EventCache* cache = m_eventsCache.get(context);

  // free the memory allocated to the previous event processed in the current slot
  // -- if we make sure ROBDataProviderSvc does this, then TrigByteStreamInputSvc won't need a cache
  releaseEvent(cache);

  // put the new raw event into the cache
  cache->rawEvent = newRawEvent;

  m_robDataProviderSvc->setNextEvent(context,newRawEvent);
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
// Implementation of ByteStreamInputSvc::generateDataHeader
// Unlike offline, we do not actually generate any DataHeader here. We only create and record an EventInfo address.
// =============================================================================
StatusCode TrigByteStreamInputSvc::generateDataHeader() {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  
  IOpaqueAddress* iop = new ByteStreamAddress(ClassID_traits<EventInfo>::ID(), "ByteStreamEventInfo", "");
  CHECK(m_evtStore->recordAddress("ByteStreamEventInfo",iop));

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
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
