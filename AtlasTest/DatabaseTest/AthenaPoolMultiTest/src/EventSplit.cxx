/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
//#include "AthenaPoolTestData/FauxTriggerMap.h"

#include "GaudiKernel/MsgStream.h"

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
   ATH_CHECK( m_evt.initialize() );
   ATH_CHECK( m_rftm.initialize() );
   return StatusCode::SUCCESS;
}

StatusCode EventSplit::execute() 
{
   ATH_MSG_DEBUG( "in execute()"  );

   // Get the event header, print out event and run number
   SG::ReadHandle<xAOD::EventInfo> evt (m_evt);
   if (!evt.isValid()) {
      ATH_MSG_FATAL( "Could not find event info"  );
      return(StatusCode::FAILURE);
   }
   ATH_MSG_INFO( "EventInfo event: " << evt->eventNumber() 
                         << " run: " << evt->runNumber()  );
   
   this->setFilterPassed(false);
   // Let's look for the TriggerMap information
   SG::ReadHandle<FauxTriggerMap> eh(m_rftm);
   // Exit if no trigmap info
   if (!eh.isValid()) {
     ATH_MSG_ERROR( "Could not find FauxTriggerMap"  );
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

   return(StatusCode::SUCCESS);
}

StatusCode EventSplit::finalize() 
{
   ATH_MSG_INFO( "in finalize()"  );
   return(StatusCode::SUCCESS);
}
