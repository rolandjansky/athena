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

//<<<<<< INCLUDES                                                       >>>>>>

#include "PileUpEventInfoReader.h"

// Event includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/PileUpEventInfo.h"

// TES include
#include "StoreGate/StoreGateSvc.h"

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

// Constructor with parameters:
PileUpEventInfoReader::PileUpEventInfoReader(const std::string &name, 
				 ISvcLocator *pSvcLocator) :
    Algorithm(name,pSvcLocator),
    m_storeGate(0)
{}

// Initialize method:
StatusCode PileUpEventInfoReader::initialize()
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "PileUpEventInfoReader::initialize()" << endreq;

    // get StoreGate service
    StatusCode sc = service("StoreGateSvc",m_storeGate);
    if (sc.isFailure()) {
	log << MSG::ERROR << "StoreGate service not found !" << endreq;
	return StatusCode::FAILURE;
    } else {}

    return StatusCode::SUCCESS;
}

// Execute method:
StatusCode PileUpEventInfoReader::execute() 
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "PileUpEventInfoReader::execute()" << endreq;

    // Read PileupEventInfo

    // PileupInfo as EventInfo
//    const EventInfo * evt = 0;
    //StatusCode sc = m_storeGate->retrieve( evt, "OverlayEvent" );
    StatusCode sc;
//     StatusCode sc = m_storeGate->retrieve( evt );
//     if ( sc.isFailure() ) {
//  	log << MSG::ERROR << "  Could not get pileup event info as event info" << endreq;      
//  	return StatusCode::FAILURE;
//     }
//     else {
//  	log << MSG::DEBUG << "PileUpEventInfo as EventInfo"
//  	    << endreq;
//  	log << MSG::DEBUG << "Event ID: ["
//  	    << evt->event_ID()->run_number()   << ","
//  	    << evt->event_ID()->event_number() << ":"
//  	    << evt->event_ID()->time_stamp() << "] "
//  	    << endreq;
//  	log << MSG::DEBUG << "Event type: user type "
//  	    << evt->event_type()->user_type()
//  	    << endreq;
//     }


    // PileupEventInfo as itself
    const PileUpEventInfo* pevt = 0;
    //sc = m_storeGate->retrieve( pevt, "OverlayEvent" );
    sc = m_storeGate->retrieve( pevt );
    if ( sc.isFailure() ) {
 	log << MSG::ERROR << "  Could not get pileup event info" << endreq;      
 	return StatusCode::FAILURE;
    }
    else {
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
    }
    
    return StatusCode::SUCCESS;
}


// Finalize method:
StatusCode PileUpEventInfoReader::finalize() 
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "PileUpEventInfoReader::finalize()" << endreq;

    return StatusCode::SUCCESS;
}


