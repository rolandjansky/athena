/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file WriteData.cxx
 *  @brief This file contains the implementation for the WriteData class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: WriteData.cxx,v 1.25 2008-12-10 21:28:11 gemmeren Exp $
 **/

#include "WriteData.h"

// the user data-class defintions
#include "AthenaPoolExampleData/ExampleHitContainer.h"

#include "GaudiKernel/MsgStream.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "StoreGate/StoreGateSvc.h"

using namespace AthPoolEx;

//___________________________________________________________________________
WriteData::WriteData(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator), p_SGevent("StoreGateSvc", name) {
}
//___________________________________________________________________________
WriteData::~WriteData() {
}
//___________________________________________________________________________
StatusCode WriteData::initialize() {
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in initialize()" << endreq;

   // locate the StoreGateSvc and initialize our local ptr
   StatusCode sc = p_SGevent.retrieve();
   if (!sc.isSuccess() || 0 == p_SGevent) {
      log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
   }
   return(sc);
}
//___________________________________________________________________________
StatusCode WriteData::execute() {
   StatusCode sc = StatusCode::SUCCESS;
   MsgStream log(msgSvc(), name());
   log << MSG::DEBUG << "in execute()" << endreq;

   const DataHandle<EventInfo> evt;
   sc = p_SGevent->retrieve(evt);
   if (sc.isFailure() || evt == 0) {
      log << MSG::FATAL << "Could not find event" << endreq;
      return(StatusCode::FAILURE);
   }
   int eventNumber = evt->event_ID()->event_number();
   int runNumber = evt->event_ID()->run_number();
   log << MSG::INFO << "EventInfo event: " << eventNumber << "  run: " << runNumber << endreq;

   ExampleHitContainer* hitCont = new ExampleHitContainer();
   for (int i = 0; i < 10; i++) {
      ExampleHit* hitObj = new ExampleHit();
      hitObj->setX(1.2345 + eventNumber * 100 + 3.21 * i);
      hitObj->setY(-2.345 + runNumber * 100 - 5.6789 * i);
      hitObj->setZ(34.567 - eventNumber * 100 + 87.6 / i);
      hitObj->setDetector("DummyHitDetector");
      hitCont->push_back(hitObj);
   }
   sc = p_SGevent->record(hitCont, "MyHits");
   if (sc.isFailure()) {
      log << MSG::ERROR << "Could not register ExampleHitContainer/MyHits" << endreq;
      return(StatusCode::FAILURE);
   }
   if (eventNumber == 14) {
      log << MSG::INFO << "Now creating ExampleHit alias for event: " << eventNumber << endreq;
      sc = p_SGevent->setAlias(hitCont, "PetersHits");
      if (sc.isFailure()) {
         log << MSG::ERROR << "Could not register alias for ExampleHitContainer/MyHits" << endreq;
         return(StatusCode::FAILURE);
      }
   }
   log << MSG::INFO << "registered all data" << endreq;
   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode WriteData::finalize() {
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in finalize()" << endreq;
   return(StatusCode::SUCCESS);
}
