/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file EventSplit.cxx
 * @brief Implementation of class EventSplit
 */
 
#include <string>
#include "EventSplit.h"

// the user data-class defintions
#include "AthenaPoolExampleData/ExampleClass.h"
#include "AthenaPoolExampleData/ExampleHit.h"
#include "AthenaPoolExampleData/ExampleTrack.h"
#include "AthenaPoolTestData/TrigPath.h"
#include "AthenaPoolTestData/FauxTriggerMap.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PropertyMgr.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "StoreGate/StoreGateSvc.h"

//___________________________________________________________________________
EventSplit::EventSplit(const std::string& name, ISvcLocator* pSvcLocator) : 
   AthFilterAlgorithm(name, pSvcLocator), 
   m_sGevent(0),
   m_l1bits(0),
   m_l2bits(0) 
{
   // Declare the properties
   declareProperty("L1bitmask", m_l1bits);
   declareProperty("L2bitmask", m_l2bits);
}

EventSplit::~EventSplit()
{}

StatusCode EventSplit::initialize() 
{
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in initialize()" << endreq;

   // Locate the StoreGateSvc and initialize our local ptr
   StatusCode sc = service("StoreGateSvc", m_sGevent);
   if (!sc.isSuccess() || 0 == m_sGevent) {
      log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
   }
   return(sc);
}

StatusCode EventSplit::execute() 
{
   StatusCode sc = StatusCode::SUCCESS;
   MsgStream log(msgSvc(), name());
   log << MSG::DEBUG << "in execute()" << endreq;

   // Get the event header, print out event and run number
   //const DataHandle<xAOD::EventInfo> evt;
   const DataHandle<EventInfo> evt;
   sc = m_sGevent->retrieve(evt);
   if (sc.isFailure()) {
      log << MSG::FATAL << "Could not find event" << endreq;
      return(StatusCode::FAILURE);
   }
   if (!evt.isValid()) {
      log << MSG::FATAL << "Could not find event" << endreq;
      return(StatusCode::FAILURE);
   }
   log << MSG::INFO << "EventInfo event: " << evt->event_ID()->event_number() 
	            << " run: " << evt->event_ID()->run_number() << endreq;
   // Get the DataObject, print out its contents
   log << MSG::INFO << "Get Smart data ptr 1" << endreq;
   
   this->setFilterPassed(false);
   // Let's look for the TriggerMap information
   if (m_sGevent->contains<FauxTriggerMap>("MultiTestTrigMap")) {
      const DataHandle<FauxTriggerMap> eh;
      sc = m_sGevent->retrieve(eh, "MultiTestTrigMap");
      if (!sc.isSuccess()) {
         log << MSG::ERROR << "Could not find FauxTriggerMap" << endreq;
         //return(StatusCode::FAILURE);
         return(StatusCode::SUCCESS);
      }
      // Now check for requested bits in L1 
      log << MSG::DEBUG << "Trigmask = " << static_cast<int>(eh->bits()) << endreq;
      if (m_l1bits & eh->bits()) {
	 setFilterPassed(true);
	 log << MSG::INFO << "L1 passed" << endreq;
      } else {
	 log << MSG::INFO << "L1 failed" << endreq;
      }
   }
   else {
      log << MSG::WARNING << "Could not find TrigMap object!" << endreq;
   }
   // Let's look for the TriggerInfo object
   return(StatusCode::SUCCESS);
}

StatusCode EventSplit::finalize() 
{
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in finalize()" << endreq;
   return(StatusCode::SUCCESS);
}
