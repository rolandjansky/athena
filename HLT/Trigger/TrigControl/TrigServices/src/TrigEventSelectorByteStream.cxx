/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEventSelectorByteStream.h"
#include "hltinterface/DataCollector.h"
#include "eformat/write/FullEventFragment.h"

// =============================================================================
// Standard constructor
// =============================================================================
TrigEventSelectorByteStream::TrigEventSelectorByteStream(const std::string& name, ISvcLocator* svcLoc)
: Service(name, svcLoc),
  m_noMoreEvents(false) {

}
// =============================================================================
// Standard destructor
// =============================================================================
TrigEventSelectorByteStream::~TrigEventSelectorByteStream()
{
}

// =============================================================================
// Implementation of IInterface::queryInterface
// =============================================================================
StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) {
  verbose() << "start of " << __FUNCTION__ << endmsg;

  if(IEvtSelector::interfaceID().versionMatch(riid))
    *ppvInterface = static_cast<IEvtSelector*>(this);
  else
    return Service::queryInterface(riid, ppvInterface);

  addRef();
  verbose() << "end of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of IEvtSelector::next(Context&)
// =============================================================================
StatusCode TrigEventSelectorByteStream::next(Context& c) const
{
  verbose() << "start of " << __FUNCTION__ << endmsg;
  eformat::write::FullEventFragment l1r;
  hltinterface::DataCollector::Status status = hltinterface::DataCollector::instance()->getNext(&l1r);
  if (status == hltinterface::DataCollector::Status::OK) {
    verbose() << "Received a new event (L1 result) from DataCollector" << endmsg;
  }
  else if (status == hltinterface::DataCollector::Status::STOP) {
    m_noMoreEvents = true;
    debug() << "No more events available from DataCollector" << endmsg;
    return StatusCode::FAILURE;
  }
  else {
    error() << "Unhandled status returned from hltinterface::DataCollector::getNext" << endmsg;
    return StatusCode::FAILURE;
  }
  verbose() << "end of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

// =============================================================================
// Unimplemented methods of IEvtSelector
// =============================================================================
#define TRIGEVENTSELECTORBYTESTREAM_UNIMPL \
  fatal() << "Misconfiguration - the method " << __FUNCTION__ << " cannot be used online" << endmsg; \
  return StatusCode::FAILURE;

StatusCode TrigEventSelectorByteStream::createContext(Context*& c) const
{
  TRIGEVENTSELECTORBYTESTREAM_UNIMPL
}

StatusCode TrigEventSelectorByteStream::next(Context& c, int jump)
{
  TRIGEVENTSELECTORBYTESTREAM_UNIMPL
}

StatusCode TrigEventSelectorByteStream::previous(Context& c)
{
  TRIGEVENTSELECTORBYTESTREAM_UNIMPL
}

StatusCode TrigEventSelectorByteStream::previous(Context& c, int jump) const
{
  TRIGEVENTSELECTORBYTESTREAM_UNIMPL
}

StatusCode TrigEventSelectorByteStream::last(Context& refContext) const
{
  TRIGEVENTSELECTORBYTESTREAM_UNIMPL
}

StatusCode TrigEventSelectorByteStream::rewind(Context& c) const
{
  TRIGEVENTSELECTORBYTESTREAM_UNIMPL
}

StatusCode TrigEventSelectorByteStream::createAddress(const Context& c, IOpaqueAddress*& iop) const
{
  TRIGEVENTSELECTORBYTESTREAM_UNIMPL
}

StatusCode TrigEventSelectorByteStream::releaseContext(Context*& c) const
{
  TRIGEVENTSELECTORBYTESTREAM_UNIMPL
}

StatusCode TrigEventSelectorByteStream::resetCriteria(const std::string& cr, Context& c) const
{
  TRIGEVENTSELECTORBYTESTREAM_UNIMPL
}
