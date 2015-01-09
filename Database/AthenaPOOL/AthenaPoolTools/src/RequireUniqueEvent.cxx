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
              AthAlgorithm(name, pSvcLocator), 
	      m_cnt(0)
{}

RequireUniqueEvent::~RequireUniqueEvent() 
{}

StatusCode RequireUniqueEvent::initialize() 
{ 
   ATH_MSG_INFO ( "in initialize()" );
   m_evList.clear();
   m_cnt=0;
   return StatusCode::SUCCESS;
}

StatusCode RequireUniqueEvent::execute() 
{
   ATH_MSG_DEBUG ( "in execute()" );
   
   // Check for event header
   const DataHandle<EventInfo> evt;
   ATH_CHECK( evtStore()->retrieve(evt) );
   ATH_MSG_DEBUG ( "Found EventInfo in SG" );
    
   if (!evt.isValid()) {
       ATH_MSG_FATAL ( "Could not find event" );
       return(StatusCode::FAILURE);
   }
   ATH_MSG_DEBUG ( "EventInfo event: " << evt->event_ID()->event_number() 
                   << " run: " << evt->event_ID()->run_number() );
   //
   // Check if EventID has occurred previously. If so, issue a warning
   //  -- Use brute force, i.e. let the set keep the index and do the search
   //  -- return code on insert is pair with iterator,bool
   //
   EventID ev( evt->event_ID()->run_number(), evt->event_ID()->event_number() ); 
   if (!m_evList.insert(ev).second) {
       ATH_MSG_WARNING ( "Duplicate record " << ev );
       m_cnt++;
   }

   return(StatusCode::SUCCESS);
}   

StatusCode RequireUniqueEvent::finalize() 
{
   // Just print out the count at the end

   if (m_cnt>0) {
     ATH_MSG_INFO ( "** Found " << m_cnt << " duplicate EventID's **" );
   }

   return StatusCode::SUCCESS; 
}
