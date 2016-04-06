/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file PileUpEventInfoWriter.cxx
 *
 * @brief Test reading of PileUpEventInfo
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: PileUpEventInfoWriter.cxx,v 1.5 2009-03-30 11:18:27 schaffer Exp $
 *
 */


#include "PileUpEventInfoWriter.h"

// Event includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/PileUpEventInfo.h"
#include "EventInfo/PileUpTimeEventIndex.h"

#include "GaudiKernel/IIncidentSvc.h"

// Constructor with parameters:
PileUpEventInfoWriter::PileUpEventInfoWriter(const std::string &name, 
                                             ISvcLocator *pSvcLocator) :
    AthAlgorithm(name,pSvcLocator)
{}

// Initialize method:
StatusCode PileUpEventInfoWriter::initialize()
{
    // Set to be listener for end of event
    IIncidentSvc* incSvc = nullptr;
    ATH_CHECK( service("IncidentSvc",incSvc) );
    long int pri=100;
    incSvc->addListener( this, "EndEvent", pri);

    return StatusCode::SUCCESS;
}

// Execute method:
StatusCode PileUpEventInfoWriter::execute() 
{
    ATH_MSG_DEBUG("PileUpEventInfoWriter::execute() - do nothing" );

    // Write a PileupEventInfo

    const EventInfo * evt = 0;
    ATH_CHECK( evtStore()->retrieve( evt, "McEventInfo" ) );
    ATH_MSG_DEBUG( "Event ID: ["
                   << evt->event_ID()->run_number()   << ","
                   << evt->event_ID()->event_number() << ":"
                   << evt->event_ID()->time_stamp() << "] " );
    ATH_MSG_DEBUG( "Event type: user type "
                   << evt->event_type()->user_type() );

    EventID* pOvrID = new EventID(evt->event_ID()->run_number(),
				  evt->event_ID()->event_number());
    EventType* pOvrEt = new EventType(); //FIXME
    pOvrEt->set_user_type("Overlaid"); //FIXME

    PileUpEventInfo* pOverEvent = new PileUpEventInfo(pOvrID, pOvrEt);
    pOverEvent->addSubEvt(0, PileUpTimeEventIndex::Signal, evt, &*evtStore());

    PileUpEventInfo* pevt = pOverEvent;
    
    ATH_MSG_DEBUG("PileUpEventInfo");
    ATH_MSG_DEBUG("Event ID: ["
                  << pevt->event_ID()->run_number()   << ","
                  << pevt->event_ID()->event_number() << ":"
                  << pevt->event_ID()->time_stamp() << "] ");
    ATH_MSG_DEBUG("Event type: user type "
                  << pevt->event_type()->user_type());

    // Get normal event info as a sub-event info
    ATH_MSG_DEBUG("SubEventInfos");
    PileUpEventInfo::SubEvent::const_iterator it  = pevt->beginSubEvt();
    PileUpEventInfo::SubEvent::const_iterator end = pevt->endSubEvt();
    if (it == end) {
	ATH_MSG_DEBUG("None found" );
    }
    for (; it != end; ++it) {
	const EventInfo* sevt = (*it).pSubEvt;
	ATH_MSG_DEBUG("Time, index " 
                      << (*it).time() << " " << (*it).index());
	if (sevt) {
	    ATH_MSG_DEBUG("Event ID: ["
                          << sevt->event_ID()->run_number()   << ","
                          << sevt->event_ID()->event_number() << ":"
                          << sevt->event_ID()->time_stamp() << "] ");
	    ATH_MSG_DEBUG("Event type: user type "
                          << sevt->event_type()->user_type());
	}
	else {
	    ATH_MSG_DEBUG("Subevent is null ptr ");
	}
    }

    ATH_CHECK( evtStore()->record(pOverEvent, "OverlayEvent") );
    ATH_MSG_DEBUG( "Wrote PileUpEventInfo " );
     
    return StatusCode::SUCCESS;
}


/// incident service handle for EndEvent
void PileUpEventInfoWriter::handle(const Incident& inc) {

    ATH_MSG_DEBUG("entering handle(), incidence type " << inc.type()
                  << " from " << inc.source() );

    // Only call fillIOV for EndEvent incident
    if (inc.type() != "EndEvent") return;

    // Get proxy for PileUpEventInfo and remove it 

    // PileupEventInfo as itself
    const PileUpEventInfo* pevt = 0;
    if (evtStore()->retrieve( pevt ).isFailure() ) {
 	ATH_MSG_ERROR("  Could not get pileup event info" );      
	throw GaudiException("PileUpEventInfoWriter::handle Could not get pileup event info", 
			     "PileUpEventInfoWriter", 0);
    }
    else {
        if (evtStore()->remove(pevt).isFailure() ) {
	    ATH_MSG_ERROR("  Could not remove pileup event info" );      
            throw GaudiException("PileUpEventInfoWriter::handle Could not remove pileup event info", 
                                 "PileUpEventInfoWriter", 0);
	}
	else {
	    ATH_MSG_DEBUG("Removed PileUpEventInfo/OverlayEvent: " );
	}
    }
    ATH_MSG_DEBUG("end event handle" );

}


// Finalize method:
StatusCode PileUpEventInfoWriter::finalize() 
{
    ATH_MSG_DEBUG("PileUpEventInfoWriter::finalize()" );
    return StatusCode::SUCCESS;
}

