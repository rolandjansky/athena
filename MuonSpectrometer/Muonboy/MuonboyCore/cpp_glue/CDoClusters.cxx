/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPatternFinderToolInterfaces/IMuon_CDoClustersTool.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"

#include <iostream>

// Singleton instance
static Muon::IMuon_CDoClustersTool* s_instance = 0;

static Muon::IMuon_CDoClustersTool*  
instance() {
  if ( 0 == s_instance ) {
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    IMessageSvc* msgSvc     = 0;
    StatusCode sc           = svcLocator->service("MessageSvc", msgSvc);
    if (sc.isSuccess() && 0 != msgSvc) {
      MsgStream log(msgSvc, "CDoClusters");
      IToolSvc* toolSvc;
      sc = svcLocator->service("ToolSvc", toolSvc, true);
      if ( sc.isFailure() || toolSvc == 0 ) {
        log << MSG::FATAL << "Unable to retrieve ToolSvc " << endreq;
        return 0;
      }
      sc = toolSvc->retrieveTool("Muon::Muon_CDoClustersTool", s_instance);
      if ( sc.isFailure() ) {
        log << MSG::FATAL << "Unable to create Muon_CDoClustersTool" << endreq;
      } else {
        log << MSG::DEBUG << "==> New Muon_CDoClustersTool created" << endreq;
      }
    } else {
      std::cout << "***** CDoClusters: Unable to create MessageSvc" << std::endl;
    }
  } 
  return s_instance;
}

extern "C" {
  extern void cdoclusters_(){
    Muon::IMuon_CDoClustersTool* theSingleton = instance();
    if ( 0 != theSingleton ) {
      theSingleton->execute();
      
      // The interim implementation just calls the F-90 subroutine
      extern void doclusters_();
      doclusters_(); 
      
      // Retrieve the Clusters from F-90
      
      // Push the TrackRoadCollection into StoreGate
      ISvcLocator* svcLocator = Gaudi::svcLocator();
      IMessageSvc* msgSvc     = 0;
      StatusCode sc           = svcLocator->service("MessageSvc", msgSvc);
      if (sc.isSuccess() && 0 != msgSvc) {
        MsgStream log(msgSvc, "CDoClusters");
        StoreGateSvc* sgSvc = 0;
        StatusCode sc       = svcLocator->service("StoreGateSvc", sgSvc);
        if (sc.isSuccess() && 0 != sgSvc) {
////          sc = sgSvc->record(roads, "MuonTrackRoads");
          if ( !sc.isSuccess()) {
            log << MSG::FATAL << "Unable to register MuonClusters collection " << endreq;            
          }
        } else {
          log << MSG::FATAL << "Unable to retrieve StoreGateSvc " << endreq;
        } 
      } else {
        std::cout << "***** CDoClusters: Unable to create MessageSvc" << std::endl;            
      }
    }
  }
}

