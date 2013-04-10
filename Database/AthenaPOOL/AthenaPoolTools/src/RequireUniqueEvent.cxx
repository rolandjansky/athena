/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RequireUniqueEvent.cxx 
 * @brief Implementation of class RequireUniqueEvent
*/

#include "RequireUniqueEvent.h"

// the user data-class defintions
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/AlgFactory.h"
                                                          
#include "StoreGate/StoreGateSvc.h"

#include <string>
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
    
RequireUniqueEvent::RequireUniqueEvent(const std::string& name, 
			ISvcLocator* pSvcLocator) :
              Algorithm(name, pSvcLocator), 
	      m_sGevent("StoreGateSvc", name),
	      m_cnt(0)
{}

RequireUniqueEvent::~RequireUniqueEvent() 
{}

StatusCode RequireUniqueEvent::initialize() 
{ 
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in initialize()" << endreq;

   m_evList.clear();
   m_cnt=0;
                                                                                
   // Locate the StoreGateSvc and initialize our local ptr
   StatusCode sc = m_sGevent.retrieve();
   if (!sc.isSuccess() || 0 == m_sGevent) {
            log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
   } 
   return sc;
}

StatusCode RequireUniqueEvent::execute() 
{
   StatusCode sc = StatusCode::SUCCESS;
   MsgStream log(msgSvc(), name());
   log << MSG::DEBUG << "in execute()" << endreq;
   
   // Check for event header
   const DataHandle<EventInfo> evt;
   sc = m_sGevent->retrieve(evt);
   if (sc.isFailure()) {
       log << MSG::FATAL << "Could not find event" << endreq;
       return(StatusCode::FAILURE);
   }
   else {
       log << MSG::DEBUG << "Found EventInfo in SG" << endreq;
   }
    
   if (!evt.isValid()) {
       log << MSG::FATAL << "Could not find event" << endreq;
       return(StatusCode::FAILURE);
   }
   log << MSG::DEBUG << "EventInfo event: " << evt->event_ID()->event_number() 
                    << " run: " << evt->event_ID()->run_number() << endreq;
   //
   // Check if EventID has occurred previously. If so, issue a warning
   //  -- Use brute force, i.e. let the set keep the index and do the search
   //  -- return code on insert is pair with iterator,bool
   //
   EventID ev( evt->event_ID()->run_number(), evt->event_ID()->event_number() ); 
   if (!m_evList.insert(ev).second) {
       log << MSG::WARNING << "Duplicate record " << ev << endreq;
       m_cnt++;
   }

   return(StatusCode::SUCCESS);
}   

StatusCode RequireUniqueEvent::finalize() 
{
   // Just print out the count at the end

   MsgStream log(msgSvc(), name());

   if (m_cnt>0) {
      log << MSG::INFO << "** Found " << m_cnt << " duplicate EventID's **" << endreq; 
   }

   return StatusCode::SUCCESS; 
}
