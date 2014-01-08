/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPatternFinderToolInterfaces/IMuon_CDoErrorSegmentTool.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
 
#include <iostream>

// Singleton instance
static Muon::IMuon_CDoErrorSegmentTool* s_instance = 0;

static Muon::IMuon_CDoErrorSegmentTool*  instance() {
  if ( 0 == s_instance ) {
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    IMessageSvc* msgSvc = 0;
    StatusCode sc = svcLocator->service("MessageSvc", msgSvc);
    MsgStream log(msgSvc, "CDoErrorSegment");
    IToolSvc* toolSvc;
    sc = svcLocator->service("ToolSvc", toolSvc, true);
    if ( sc.isFailure() || toolSvc == 0 ) {
      log << MSG::FATAL << "Unable to retrieve ToolSvc " << endreq;
      return 0;
    }
    sc = toolSvc->retrieveTool("Muon::Muon_CDoErrorSegmentTool", s_instance);
    if ( sc.isFailure() ) {
      log << MSG::FATAL << "Unable to create Muon_CDoErrorSegmentTool" << endreq;
    } else {
      log << MSG::DEBUG << "==> New Muon_CDoErrorSegmentTool created" << endreq;
    }
  } 
  return s_instance;
}

extern "C" {
  extern void cdoerrorsegment_(){
    Muon::IMuon_CDoErrorSegmentTool* theSingleton = instance();
    if ( 0 != theSingleton ) {
      theSingleton->execute();
    }
  }
}

