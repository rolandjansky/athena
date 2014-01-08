/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPatternFinderToolInterfaces/IMuon_CDoCSCSegmentsTool.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
 
#include <iostream>

// Singleton instance
static Muon::IMuon_CDoCSCSegmentsTool* s_instance = 0;

static Muon::IMuon_CDoCSCSegmentsTool*  instance() {
  if ( 0 == s_instance ) {
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    IMessageSvc* msgSvc = 0;
    StatusCode sc = svcLocator->service("MessageSvc", msgSvc);
    MsgStream log(msgSvc, "CDoCSCSegments");
    IToolSvc* toolSvc;
    sc = svcLocator->service("ToolSvc", toolSvc, true);
    if ( sc.isFailure() || toolSvc == 0 ) {
      log << MSG::FATAL << "Unable to retrieve ToolSvc " << endreq;
      return 0;
    }
    sc = toolSvc->retrieveTool("Muon::Muon_CDoCSCSegmentsTool", s_instance);
    if ( sc.isFailure() ) {
      log << MSG::FATAL << "Unable to create Muon_CDoCSCSegmentsTool" << endreq;
    } else {
      log << MSG::DEBUG << "==> New Muon_CDoCSCSegmentsTool created" << endreq;
    }
  } 
  return s_instance;
}

extern "C" {
  extern void cdocscsegments_(){
    Muon::IMuon_CDoCSCSegmentsTool* theSingleton = instance();
    if ( 0 != theSingleton ) {
      theSingleton->execute();
    }
  }
}

