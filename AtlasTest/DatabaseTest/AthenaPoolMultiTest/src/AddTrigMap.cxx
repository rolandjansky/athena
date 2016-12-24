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
//#include "AthenaPoolTestData/FauxTriggerMap.h"

#include <string>
//#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "CxxUtils/make_unique.h"
    
AddTrigMap::AddTrigMap(const std::string& name, 
			ISvcLocator* pSvcLocator) :
              AthAlgorithm(name, pSvcLocator),
              m_evt("McEventInfo"),
              m_wftm("MultiTestTrigMap"),
              m_wftm2("ExcludeTestTrigMap")
{}

AddTrigMap::~AddTrigMap() 
{}

StatusCode AddTrigMap::initialize() 
{ 
   ATH_MSG_INFO( "in initialize()"  );
   ATH_CHECK( m_evt.initialize() );
   ATH_CHECK( m_wftm.initialize() );
   ATH_CHECK( m_wftm2.initialize() );
   return StatusCode::SUCCESS;
}

StatusCode AddTrigMap::execute() 
{
   ATH_MSG_DEBUG( "in execute()"  );
   
   // Check for event header
   SG::ReadHandle<EventInfo> evt (m_evt);
   if (!evt.isValid()) {
       ATH_MSG_FATAL( "Could not find event"  );
       return(StatusCode::FAILURE);
   }

   ATH_MSG_INFO( "EventInfo event: " << evt->event_ID()->event_number() 
                         << " run: " << evt->event_ID()->run_number()  );
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
   
   // Set up the writing
   if (ftm!=0) {
     SG::WriteHandle<FauxTriggerMap> wftm(m_wftm);
     ATH_CHECK( wftm.record (std::make_unique<FauxTriggerMap>(*ftm)) );
   }
   // Now add a copy for exclude list test
   if (ftm2!=0) {
     SG::WriteHandle<FauxTriggerMap> wftm2(m_wftm2);
     ATH_CHECK( wftm2.record (std::make_unique<FauxTriggerMap>(*ftm2)) );
   }

   ATH_MSG_INFO( "registered all data"  );
   return(StatusCode::SUCCESS);
}   

StatusCode AddTrigMap::finalize() { return StatusCode::SUCCESS; }
