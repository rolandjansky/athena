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

//<<<<<< INCLUDES                                                       >>>>>>

#include "PileUpEventInfoWriter.h"

// Event includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/PileUpEventInfo.h"
#include "EventInfo/PileUpTimeEventIndex.h"

// TES include
#include "StoreGate/StoreGateSvc.h"

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IIncidentSvc.h"

// Constructor with parameters:
PileUpEventInfoWriter::PileUpEventInfoWriter(const std::string &name, 
				 ISvcLocator *pSvcLocator) :
    Algorithm(name,pSvcLocator),
    m_storeGate(0)
{}

// Initialize method:
StatusCode PileUpEventInfoWriter::initialize()
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());

    // get StoreGate service
    StatusCode sc = service("StoreGateSvc",m_storeGate);
    if (sc.isFailure()) {
	log << MSG::ERROR << "StoreGate service not found !" << endreq;
	return StatusCode::FAILURE;
    } 

    // Set to be listener for end of event
    IIncidentSvc* incSvc;
    sc = service("IncidentSvc",incSvc);
    if (sc.isFailure()) {
	log << MSG::ERROR << "Unable to get the IncidentSvc" << endreq;
	return sc;
    }
    long int pri=100;
    incSvc->addListener( this, "EndEvent", pri);

    return StatusCode::SUCCESS;
}

// Execute method:
StatusCode PileUpEventInfoWriter::execute() 
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "PileUpEventInfoWriter::execute() - do nothing" << endreq;

    // Write a PileupEventInfo

    const EventInfo * evt = 0;
    StatusCode sc = m_storeGate->retrieve( evt, "McEventInfo" );
    if ( sc.isFailure() ) {
 	log << MSG::ERROR << "  Could not get event info" << endreq;      
 	return StatusCode::FAILURE;
    }
    else {
 	log << MSG::DEBUG << "Event ID: ["
 	    << evt->event_ID()->run_number()   << ","
            << evt->event_ID()->event_number() << ":"
 	    << evt->event_ID()->time_stamp() << "] "
 	    << endreq;
 	log << MSG::DEBUG << "Event type: user type "
 	    << evt->event_type()->user_type()
 	    << endreq;
    }

    EventID* pOvrID = new EventID(evt->event_ID()->run_number(),
				  evt->event_ID()->event_number());
    EventType* pOvrEt = new EventType(); //FIXME
    pOvrEt->set_user_type("Overlaid"); //FIXME

    PileUpEventInfo* pOverEvent = new PileUpEventInfo(pOvrID, pOvrEt);
//    EventInfo* pOverEventSym = pOverEvent;
    //  register as sub event of the overlaid
    //pOverEvent->addSubEvt(0.0, evt, m_origStream.store());
//    pOverEvent->addSubEvt(0.0, evt, m_storeGate);
//    pOverEvent->addSubEvt(0.0, 0, m_storeGate);
    pOverEvent->addSubEvt(0, PileUpTimeEventIndex::Signal, evt, m_storeGate);
    //FIXME (Davide) root 4 does not link a symlink, need to copy
//     const EventInfo* pDeepCopy = new const EventInfo(*evt);
//     if( !(p_SGOver->record(pDeepCopy, "MyEvent")).isSuccess() )  {
// 	log << MSG::ERROR
// 	    << "Error recording overlayed event object" << endreq;
// 	return StatusCode::FAILURE;
//     } 

    PileUpEventInfo* pevt = pOverEvent;
    
    log << MSG::DEBUG << "PileUpEventInfo"
	<< endreq;
    log << MSG::DEBUG << "Event ID: ["
	<< pevt->event_ID()->run_number()   << ","
        << pevt->event_ID()->event_number() << ":"
	<< pevt->event_ID()->time_stamp() << "] "
	<< endreq;
    log << MSG::DEBUG << "Event type: user type "
	<< pevt->event_type()->user_type()
	<< endreq;


    // Get normal event info as a sub-event info
    log << MSG::DEBUG << "SubEventInfos"
	<< endreq;
    PileUpEventInfo::SubEvent::const_iterator it  = pevt->beginSubEvt();
    PileUpEventInfo::SubEvent::const_iterator end = pevt->endSubEvt();
    if (it == end) {
	log << MSG::DEBUG << "None found" << endreq;
    }
    for (; it != end; ++it) {
	const EventInfo* sevt = (*it).pSubEvt;
	log << MSG::DEBUG << "Time, index " 
	    << (*it).time() << " " << (*it).index()
	    << endreq;
	if (sevt) {
	    log << MSG::DEBUG << "Event ID: ["
		<< sevt->event_ID()->run_number()   << ","
                << sevt->event_ID()->event_number() << ":"
		<< sevt->event_ID()->time_stamp() << "] "
		<< endreq;
	    log << MSG::DEBUG << "Event type: user type "
		<< sevt->event_type()->user_type()
		<< endreq;
	}
	else {
	    log << MSG::DEBUG << "Subevent is null ptr "
		<< endreq;
	}
    }

    if( !(m_storeGate->record(pOverEvent, "OverlayEvent")).isSuccess() )  {
 	log << MSG::ERROR
 	    << "Error recording overlayed event object" << endreq;
 	return StatusCode::FAILURE;
    } 
    else {
 	log << MSG::DEBUG << "Wrote PileUpEventInfo "
 	    << endreq;
    }
     
    //FIXME (Davide) root 4 does not link a symlink, need to copy
//     if( !(m_storeGate->symLink(pOverEvent, pOverEventSym)).isSuccess() )  {
// 	log << MSG::ERROR
// 	    << "Error symlinking overlayed event object" << endreq;
// 	return StatusCode::FAILURE;
//     } 
//     else {
//  	log << MSG::DEBUG << "Symlinked PileUpEventInfo to EventInfo "
//  	    << endreq;
//     }
    
    
    return StatusCode::SUCCESS;
}


/// incident service handle for EndEvent
void PileUpEventInfoWriter::handle(const Incident& inc) {


    // Get the messaging service, print where you are
    MsgStream log(messageService(), name());

    log << MSG::DEBUG << "entering handle(), incidence type " << inc.type()
	<< " from " << inc.source() << endreq;

    // Only call fillIOV for EndEvent incident
    if (inc.type() != "EndEvent") return;

    // Get proxy for PileUpEventInfo and remove it 

    // PileupEventInfo as itself
    const PileUpEventInfo* pevt = 0;
    StatusCode sc = m_storeGate->retrieve( pevt, "OverlayEvent" );
    if ( sc.isFailure() ) {
 	log << MSG::ERROR << "  Could not get pileup event info" << endreq;      
	throw GaudiException("PileUpEventInfoWriter::handle Could not get pileup event info", 
			     "PileUpEventInfoWriter", 0);
    }
    else {
	sc = m_storeGate->remove(pevt);
	if ( sc.isFailure() ) {
	    log << MSG::ERROR << "  Could not remove pileup event info" << endreq;      
	throw GaudiException("PileUpEventInfoWriter::handle Could not remove pileup event info", 
			     "PileUpEventInfoWriter", 0);
	}
	else {
	    log << MSG::DEBUG << "Removed PileUpEventInfo/OverlayEvent: "
		<< endreq;
	}
    }
    log << MSG::DEBUG << "end event handle" << endreq;

}


// Finalize method:
StatusCode PileUpEventInfoWriter::finalize() 
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "PileUpEventInfoWriter::finalize()" << endreq;

    return StatusCode::SUCCESS;
}

