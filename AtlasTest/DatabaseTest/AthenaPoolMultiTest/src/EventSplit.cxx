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
   ATH_MSG_INFO( "in initialize()"  );
   return StatusCode::SUCCESS;
}

StatusCode EventSplit::execute() 
{
   ATH_MSG_DEBUG( "in execute()"  );

   // Get the event header, print out event and run number
   //const DataHandle<xAOD::EventInfo> evt;
   const DataHandle<EventInfo> evt;
   ATH_CHECK( evtStore()->retrieve(evt) );
   if (!evt.isValid()) {
      ATH_MSG_FATAL( "Could not find event"  );
      return(StatusCode::FAILURE);
   }
   ATH_MSG_INFO( "EventInfo event: " << evt->event_ID()->event_number() 
                 << " run: " << evt->event_ID()->run_number()  );
   // Get the DataObject, print out its contents
   ATH_MSG_INFO( "Get Smart data ptr 1"  );
   
   this->setFilterPassed(false);
   // Let's look for the TriggerMap information
   if (evtStore()->contains<FauxTriggerMap>("MultiTestTrigMap")) {
      const DataHandle<FauxTriggerMap> eh;
      StatusCode sc = evtStore()->retrieve(eh, "MultiTestTrigMap");
      if (!sc.isSuccess()) {
         ATH_MSG_ERROR( "Could not find FauxTriggerMap"  );
         //return(StatusCode::FAILURE);
         return(StatusCode::SUCCESS);
      }
      // Now check for requested bits in L1 
      ATH_MSG_DEBUG( "Trigmask = " << static_cast<int>(eh->bits())  );
      if (m_l1bits & eh->bits()) {
	 setFilterPassed(true);
	 ATH_MSG_INFO( "L1 passed"  );
      } else {
        ATH_MSG_INFO( "L1 failed"  );
      }
   }
   else {
     ATH_MSG_WARNING( "Could not find TrigMap object!"  );
   }
   // Let's look for the TriggerInfo object
   return(StatusCode::SUCCESS);
}

StatusCode EventSplit::finalize() 
{
   ATH_MSG_INFO( "in finalize()"  );
   return(StatusCode::SUCCESS);
}
