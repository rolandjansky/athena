/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file PileUpEventInfoReader.cxx
 *
 * @brief Test reading of PileUpEventInfo
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: PileUpEventInfoReader.cxx,v 1.3 2006-09-17 15:21:22 schaffer Exp $
 *
 */

#include "PileUpEventInfoReader.h"

// Event includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/PileUpEventInfo.h"

// Constructor with parameters:
PileUpEventInfoReader::PileUpEventInfoReader(const std::string &name, 
				 ISvcLocator *pSvcLocator) :
    AthAlgorithm(name,pSvcLocator)
{}

// Initialize method:
StatusCode PileUpEventInfoReader::initialize()
{
    ATH_MSG_INFO("PileUpEventInfoReader::initialize()" );
    return StatusCode::SUCCESS;
}

// Execute method:
StatusCode PileUpEventInfoReader::execute() 
{
    MsgStream log(msgSvc(), name());
    ATH_MSG_DEBUG("PileUpEventInfoReader::execute()" );

    // PileupEventInfo as itself
    const PileUpEventInfo* pevt = nullptr;
    //sc = m_storeGate->retrieve( pevt, "OverlayEvent" );
    ATH_CHECK( evtStore()->retrieve( pevt ) );

    ATH_MSG_DEBUG( "PileUpEventInfo" );
    ATH_MSG_DEBUG( "Event ID: ["
                   << pevt->event_ID()->run_number()   << ","
                   << pevt->event_ID()->event_number() << ":"
                   << pevt->event_ID()->time_stamp() << "] " );
    ATH_MSG_DEBUG( "Event type: user type "
                   << pevt->event_type()->user_type() );


    // Get normal event info as a sub-event info
    ATH_MSG_DEBUG( "SubEventInfos" );
    PileUpEventInfo::SubEvent::const_iterator it  = pevt->beginSubEvt();
    PileUpEventInfo::SubEvent::const_iterator end = pevt->endSubEvt();
    if (it == end) {
      ATH_MSG_DEBUG( "None found"  );
    }
    for (; it != end; ++it) {
      const EventInfo* sevt = (*it).pSubEvt;
      ATH_MSG_DEBUG( "Time, index " 
                     << (*it).time() << " " << (*it).index() );
      if (sevt) {
        ATH_MSG_DEBUG( "Event ID: ["
                       << sevt->event_ID()->run_number()   << ","
                       << sevt->event_ID()->event_number() << ":"
                       << sevt->event_ID()->time_stamp() << "] " );
        ATH_MSG_DEBUG( "Event type: user type "
                       << sevt->event_type()->user_type() );
      }
      else {
        ATH_MSG_DEBUG( "Subevent is null ptr " );
      }
    }
    
    return StatusCode::SUCCESS;
}


// Finalize method:
StatusCode PileUpEventInfoReader::finalize() 
{
    ATH_MSG_INFO("PileUpEventInfoReader::finalize()" );

    return StatusCode::SUCCESS;
}


