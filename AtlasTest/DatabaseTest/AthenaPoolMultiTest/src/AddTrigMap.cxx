/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AddTrigMap.cxx 
 * @brief Implementation of class AddTrigMap
*/

#include "AddTrigMap.h"

// the user data-class defintions
#include "AthenaPoolTestData/TrigPath.h"
#include "AthenaPoolTestData/FauxTriggerMap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/AlgFactory.h"
                                                          
#include "StoreGate/StoreGateSvc.h"

#include <string>
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

    
AddTrigMap::AddTrigMap(const std::string& name, 
			ISvcLocator* pSvcLocator) :
              AthAlgorithm(name, pSvcLocator), m_sGevent(0)
{}

AddTrigMap::~AddTrigMap() 
{}

StatusCode AddTrigMap::initialize() 
{ 
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in initialize()" << endreq;
                                                                                
   // Locate the StoreGateSvc and initialize our local ptr
   StatusCode sc = service("StoreGateSvc", m_sGevent);
   if (!sc.isSuccess() || 0 == m_sGevent) {
            log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
   } 
   return sc;
}

StatusCode AddTrigMap::execute() 
{
   StatusCode sc = StatusCode::SUCCESS;
   MsgStream log(msgSvc(), name());
   log << MSG::DEBUG << "in execute()" << endreq;
   
   // Check for event header
   //const DataHandle<xAOD::EventInfo> evt;
   const DataHandle<EventInfo> evt;
   sc = m_sGevent->retrieve(evt);
   if (sc.isFailure()) {
       log << MSG::FATAL << "Could not find event" << endreq;
       return(StatusCode::FAILURE);
   }
   else {
       log << MSG::INFO << "Found EventInfo in SG" << endreq;
   }
    
   if (!evt.isValid()) {
       log << MSG::FATAL << "Could not find event" << endreq;
       return(StatusCode::FAILURE);
   }
   log << MSG::INFO << "EventInfo event: " << evt->event_ID()->event_number() 
                    << " run: " << evt->event_ID()->run_number() << endreq;
   //
   // Since we have an event, add the dummy trigger object
   // 
   int event = evt->event_ID()->event_number();
   FauxTriggerMap* ftm=0;
   ftm = new FauxTriggerMap();
   // Add some paths
   ftm->addPath(TrigPath(event%3, 2,event%6)); // repeats with period 3
   ftm->addPath(TrigPath(event%16,2,event%11+(event+1)%2)); // repeats with period 16
   if (event==3) {
     delete ftm ; ftm = new FauxTriggerMap();
     ftm->addPath(TrigPath(8,2,1)); // non normal trigger
   }
  
   // Extra map for testing exclude list 
   FauxTriggerMap* ftm2 = new FauxTriggerMap();
   // Add some paths
   ftm2->addPath(TrigPath(event%4, 2,event%6)); // repeats with period 3
   
   std::string keythis = "MultiTestTrigMap";
   if (ftm!=0) {
     sc = m_sGevent->record(ftm,keythis);
     if (sc.isFailure()) {
       log << MSG::ERROR << "could not register FauxTrigger Object " << keythis << endreq;
       return(StatusCode::FAILURE);
     }
   }
   // Now add a copy for exclude list test
   keythis = "ExcludeTestTrigMap";
   if (ftm2!=0) {
     sc = m_sGevent->record(ftm2,keythis);
     if (sc.isFailure()) {
       log << MSG::ERROR << "could not register FauxTrigger Object " << keythis << endreq;
       return(StatusCode::FAILURE);
     }
   }

   log << MSG::INFO << "registered all data" << endreq;
   return(StatusCode::SUCCESS);
}   

StatusCode AddTrigMap::finalize() { return StatusCode::SUCCESS; }
