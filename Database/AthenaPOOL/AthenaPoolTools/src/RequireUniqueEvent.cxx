/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RequireUniqueEvent.cxx 
 * @brief Implementation of class RequireUniqueEvent
*/

#include "RequireUniqueEvent.h"

// the user data-class defintions
#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
                                                          
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

   const EventIDBase& eid = getContext().eventID();
   ATH_MSG_DEBUG ( "EventInfo event: " << eid.event_number() 
                   << " run: " << eid.run_number() );
   //
   // Check if EventID has occurred previously. If so, issue a warning
   //  -- Use brute force, i.e. let the set keep the index and do the search
   //  -- return code on insert is pair with iterator,bool
   //
   EventID ev( eid.run_number(), eid.event_number() ); 
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
