/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CopyTimings.h"

#include "RecEvent/RecoTimingObj.h"

#include <iostream>
#include <typeinfo>

//================================================================
CopyTimings::CopyTimings(const std::string &name, ISvcLocator *pSvcLocator) :
  OverlayAlgBase(name, pSvcLocator) {}

//================================================================
StatusCode CopyTimings::overlayInitialize()
{
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode CopyTimings::overlayFinalize()
{
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode CopyTimings::overlayExecute() {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "CopyTimings::execute() begin"<< endmsg;

  std::auto_ptr<RecoTimingObj> ap(m_storeGateData->retrievePrivateCopy<RecoTimingObj>("EVNTtoHITS_timings"));
  if (!m_storeGateOutput->record(ap, "EVNTtoHITS_timings").isSuccess()) {
    log << MSG::ERROR << "problem recording object p=" << ap.get() << ", key=" << "EVNTtoHITS_timings" << endmsg;
    return StatusCode::FAILURE;
  }

  log << MSG::DEBUG << "CopyTimings::execute() end"<< endmsg;
  return StatusCode::SUCCESS;
}

//================================================================
//EOF
