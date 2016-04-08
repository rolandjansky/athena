/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file EventContextMuonCalibStream.cxx
 * @brief This file contains the implementation for the
EventContextMuonCalibStream class.
 *  @author Peter van Gemmeren <gemmeren@bnl.gov>
 *  @author Hong Ma <hma@bnl.gov>
 *  @author Domizia Orestano <domizia.orestano@cern.ch> 
 **/

#include "MuonCalibStreamCnvSvc/EventContextMuonCalibStream.h"
#include "MuonCalibStreamCnvSvc/EventSelectorMuonCalibStream.h"

//________________________________________________________________________________
EventContextMuonCalibStream::EventContextMuonCalibStream(const IEvtSelector*
selector) :
        m_evtSelector(selector) {
}
//________________________________________________________________________________
EventContextMuonCalibStream::EventContextMuonCalibStream(const
EventContextMuonCalibStream& ctxt) :
        IEvtSelector::Context(),
        m_evtSelector(ctxt.m_evtSelector) {
}
//________________________________________________________________________________
EventContextMuonCalibStream::~EventContextMuonCalibStream() {
}
//________________________________________________________________________________
void* EventContextMuonCalibStream::identifier() const {
 return((void*)(m_evtSelector));
}
