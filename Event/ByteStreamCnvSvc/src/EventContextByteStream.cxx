/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** @file EventContextByteStream.cxx
 *  @brief This file contains the implementation for the EventContextByteStream class.
 *  @author Peter van Gemmeren <gemmeren@bnl.gov>
 *  @author Hong Ma <hma@bnl.gov>
 **/

#include "EventContextByteStream.h"
#include "CxxUtils/checker_macros.h"

//________________________________________________________________________________
EventContextByteStream::EventContextByteStream(const IEvtSelector* selector) : m_evtSelector(selector) {
}
//________________________________________________________________________________
EventContextByteStream::EventContextByteStream(const EventContextByteStream& ctxt) : IEvtSelector::Context(),
        m_evtSelector(ctxt.m_evtSelector) {
}
//________________________________________________________________________________
EventContextByteStream::~EventContextByteStream() {
}
//________________________________________________________________________________
void* EventContextByteStream::identifier() const {
   IEvtSelector* id ATLAS_THREAD_SAFE = const_cast<IEvtSelector*> (m_evtSelector);
   return id;
}
