/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"

#include <iostream>

extern "C" {
  extern void getnewsegfindflag_(){

    ISvcLocator* svcLocator = Gaudi::svcLocator();
    IMessageSvc* msgSvc     = 0;
    StatusCode sc           = svcLocator->service("MessageSvc", msgSvc);
    if (sc.isSuccess() && 0 != msgSvc) {
      MsgStream log(msgSvc, "GetNewSegFindFlag");
      IToolSvc* toolSvc;
      sc = svcLocator->service("ToolSvc", toolSvc, true);
      if ( sc.isSuccess() && 0 != toolSvc ) {
        IProperty* propertyServer(0);
        sc = toolSvc->retrieveTool("ThirdChainMboyMuonSegmentMaker", propertyServer, 0, false);
        if ( sc.isSuccess() && 0 != propertyServer) {
          IntegerProperty useNewSegFind = IntegerProperty("UseNewSegmentFinding", 0);
          sc = propertyServer->getProperty(&useNewSegFind);
          if (!sc.isSuccess()) {
            log << MSG::INFO << "Unable to get UseNewSegmentFinding from ThirdChainMboyMuonSegmentMaker" << endreq;
          } else {
            log << MSG::INFO << "ThirdChainMboyMuonSegmentMaker UseNewSegmentFinding set to:" << useNewSegFind.value() << endreq;
            extern int __m_mb_control_MOD_newsegfind;
            __m_mb_control_MOD_newsegfind = useNewSegFind.value();
          }
        }
      }
    }    
  }
}
