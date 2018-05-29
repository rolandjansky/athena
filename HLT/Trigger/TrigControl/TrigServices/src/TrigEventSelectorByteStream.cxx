/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEventSelectorByteStream.h"

#include "ByteStreamData/RawEvent.h"
#include "ByteStreamCnvSvc/ByteStreamInputSvc.h"

#include "hltinterface/DataCollector.h"

// ============================================================================
// Standard constructor
// ============================================================================
TrigEventSelectorByteStream::TrigEventSelectorByteStream(const std::string& name, ISvcLocator* svcLoc)
: Service(name, svcLoc),
	m_eventSource("ByteStreamInputSvc", name) {
  declareProperty("ByteStreamInputSvc", m_eventSource);
}
// ============================================================================
// Standard destructor
// ============================================================================
TrigEventSelectorByteStream::~TrigEventSelectorByteStream()
{
}

// ============================================================================
// Implementation of IInterface::queryInterface
// ============================================================================
StatusCode TrigEventSelectorByteStream::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  verbose() << "start of " << __FUNCTION__ << endmsg;

  if(IEvtSelector::interfaceID().versionMatch(riid))
    *ppvInterface = static_cast<IEvtSelector*>(this);
  else
    return Service::queryInterface(riid, ppvInterface);

  addRef();
  verbose() << "end of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

// ============================================================================
// Implementation of Service::initialize
// ============================================================================
StatusCode TrigEventSelectorByteStream::initialize() {
  StatusCode sc = StatusCode::SUCCESS;

  sc = m_eventSource.retrieve();
  if (sc.isFailure()) {
    error() << "Failed to retrieve the event source service" << endmsg;
    return sc;
  }

  return sc;
}

// ============================================================================
// Implementation of Service::finalize
// ============================================================================
StatusCode TrigEventSelectorByteStream::finalize() {
  m_eventSource.release().ignore();
  return StatusCode::SUCCESS;
}

// ============================================================================
// Implementation of IEvtSelector::next(Context&)
// There is actually no event selection here, we process all events online
// ============================================================================
StatusCode TrigEventSelectorByteStream::next(IEvtSelector::Context& c) const
{
  verbose() << "start of " << __FUNCTION__ << endmsg;

  // RawEvent is a typedef for FullEventFragment
  const RawEvent* ptrRawEvent = nullptr;

  // Try to get the next event from the event source
  try {
    ptrRawEvent = m_eventSource->nextEvent();
  }
  catch (...) { // what can we actually catch here?
    error() << "Failed to get next event from the event source" << endmsg;
    return StatusCode::FAILURE;
  }

  // Check if something was returned
  if (ptrRawEvent == nullptr) {
    error() << "Event source returned nullptr from nextEvent()" << endmsg;
    return StatusCode::FAILURE;
  }

  verbose() << "end of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

// ============================================================================
// Unimplemented methods of IEvtSelector
// ============================================================================
#define TRIGEVENTSELECTORBYTESTREAM_UNIMPL \
  fatal() << "Misconfiguration - the method " << __FUNCTION__ << " cannot be used online" << endmsg; \
  return StatusCode::FAILURE;

StatusCode TrigEventSelectorByteStream::createContext(IEvtSelector::Context*& c) const
{
  TRIGEVENTSELECTORBYTESTREAM_UNIMPL
}

StatusCode TrigEventSelectorByteStream::next(IEvtSelector::Context& c, int jump) const
{
  TRIGEVENTSELECTORBYTESTREAM_UNIMPL
}

StatusCode TrigEventSelectorByteStream::previous(IEvtSelector::Context& c) const
{
  TRIGEVENTSELECTORBYTESTREAM_UNIMPL
}

StatusCode TrigEventSelectorByteStream::previous(IEvtSelector::Context& c, int jump) const
{
  TRIGEVENTSELECTORBYTESTREAM_UNIMPL
}

StatusCode TrigEventSelectorByteStream::last(IEvtSelector::Context& refContext) const
{
  TRIGEVENTSELECTORBYTESTREAM_UNIMPL
}

StatusCode TrigEventSelectorByteStream::rewind(IEvtSelector::Context& c) const
{
  TRIGEVENTSELECTORBYTESTREAM_UNIMPL
}

StatusCode TrigEventSelectorByteStream::createAddress(const IEvtSelector::Context& c, IOpaqueAddress*& iop) const
{
  TRIGEVENTSELECTORBYTESTREAM_UNIMPL
}

StatusCode TrigEventSelectorByteStream::releaseContext(IEvtSelector::Context*& c) const
{
  TRIGEVENTSELECTORBYTESTREAM_UNIMPL
}

StatusCode TrigEventSelectorByteStream::resetCriteria(const std::string& cr, IEvtSelector::Context& c) const
{
  TRIGEVENTSELECTORBYTESTREAM_UNIMPL
}
