/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file AthenaPoolExampleAlgorithms/src/WriteData.cxx
 *  @brief This file contains the implementation for the WriteData class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: WriteData.cxx,v 1.25 2008-12-10 21:28:11 gemmeren Exp $
 **/

#include "WriteData.h"

// the user data-class defintions
#include "AthenaPoolExampleData/ExampleHitContainer.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

using namespace AthPoolEx;

//___________________________________________________________________________
WriteData::WriteData(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator) {
}
//___________________________________________________________________________
WriteData::~WriteData() {
}
//___________________________________________________________________________
StatusCode WriteData::initialize() {
   ATH_MSG_INFO("in initialize()");
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode WriteData::execute() {
   ATH_MSG_DEBUG("in execute()");

   const DataHandle<EventInfo> evt;
   if (evtStore()->retrieve(evt).isFailure()) {
      ATH_MSG_FATAL("Could not find event");
      return StatusCode::FAILURE;
   }
   int eventNumber = evt->event_ID()->event_number();
   int runNumber = evt->event_ID()->run_number();
   ATH_MSG_INFO("EventInfo event: " << eventNumber << "  run: " << runNumber);

   ExampleHitContainer* hitCont = new ExampleHitContainer();
   for (int i = 0; i < 10; i++) {
      ExampleHit* hitObj = new ExampleHit();
      hitObj->setX(1.2345 + eventNumber * 100 + 3.21 * i);
      hitObj->setY(-2.345 + runNumber * 100 - 5.6789 * i);
      hitObj->setZ(34.567 - eventNumber * 100 + 87.6 / (i + 0.456));
      hitObj->setDetector("DummyHitDetector");
      hitCont->push_back(hitObj);
   }
   if (evtStore()->record(hitCont, "MyHits").isFailure()) {
      ATH_MSG_ERROR("Could not register ExampleHitContainer/MyHits");
      return StatusCode::FAILURE;
   }
   if (eventNumber == 14) {
      ATH_MSG_INFO("Now creating ExampleHit alias for event: " << eventNumber);
      if (evtStore()->setAlias(hitCont, "PetersHits").isFailure()) {
         ATH_MSG_ERROR("Could not register alias for ExampleHitContainer/MyHits");
         return StatusCode::FAILURE;
      }
   }
   ATH_MSG_INFO("registered all data");
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode WriteData::finalize() {
   ATH_MSG_INFO("in finalize()");
   return StatusCode::SUCCESS;
}
