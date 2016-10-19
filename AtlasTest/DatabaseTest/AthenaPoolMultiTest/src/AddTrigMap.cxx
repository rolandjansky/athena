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

#include <string>
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

    
AddTrigMap::AddTrigMap(const std::string& name, 
			ISvcLocator* pSvcLocator) :
              AthAlgorithm(name, pSvcLocator)
{}

AddTrigMap::~AddTrigMap() 
{}

StatusCode AddTrigMap::initialize() 
{ 
   ATH_MSG_INFO( "in initialize()"  );
   return StatusCode::SUCCESS;
}

StatusCode AddTrigMap::execute() 
{
   ATH_MSG_DEBUG( "in execute()"  );
   
   // Check for event header
   //const DataHandle<xAOD::EventInfo> evt;
   const DataHandle<EventInfo> evt;
   ATH_CHECK( evtStore()->retrieve(evt) );
   ATH_MSG_INFO( "Found EventInfo in SG"  );
    
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
   
   std::string keythis = "MultiTestTrigMap";
   if (ftm!=0) {
     ATH_CHECK( evtStore()->record(ftm,keythis) );
   }
   // Now add a copy for exclude list test
   keythis = "ExcludeTestTrigMap";
   if (ftm2!=0) {
     ATH_CHECK( evtStore()->record(ftm2,keythis) );
   }

   ATH_MSG_INFO( "registered all data"  );
   return(StatusCode::SUCCESS);
}   

StatusCode AddTrigMap::finalize() { return StatusCode::SUCCESS; }
