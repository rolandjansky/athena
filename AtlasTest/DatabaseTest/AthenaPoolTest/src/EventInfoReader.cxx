/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file EventInfoReader.cxx
 *
 * @brief Test Algorithm for checking TagInfoMgr and the storing of
 * geometry and conditions tags in EventInfo, checks tags on reading
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: EventInfoReader.cxx,v 1.10 2007-04-07 17:48:49 schaffer Exp $
 *
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "EventInfoReader.h"

// Event includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TagInfo.h"

// GeoModel
#include "GeoModelInterfaces/IGeoModelSvc.h"

// TES include
#include "StoreGate/StoreGateSvc.h"

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

// Constructor with parameters:
EventInfoReader::EventInfoReader(const std::string &name, 
				 ISvcLocator *pSvcLocator) :
    Algorithm(name,pSvcLocator),
    m_storeGate(0),
    m_detStore(0),
    m_geoModel(0)
{}

// Initialize method:
StatusCode EventInfoReader::initialize()
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "EventInfoReader::initialize()" << endreq;

    // get StoreGate service
    StatusCode sc = service("StoreGateSvc",m_storeGate);
    if (sc.isFailure()) {
	log << MSG::ERROR << "StoreGate service not found !" << endreq;
	return StatusCode::FAILURE;
    } else {}


    // get DetectorStore service
    sc = service("DetectorStore",m_detStore);
    if (sc.isFailure()) {
	log << MSG::ERROR << "DetectorStore service not found !" << endreq;
	return StatusCode::FAILURE;
    } 

    // get GeoModel service
     sc = service ("GeoModelSvc",m_geoModel);
     if (sc.isFailure()) {
 	log << MSG::ERROR << "GeoModelSvc not found !" << endreq;
 	return StatusCode::FAILURE;
     } else {
 	log << MSG::DEBUG << " Found GeoModelSvc " << endreq;
     }

    return StatusCode::SUCCESS;
}

// Execute method:
StatusCode EventInfoReader::execute() 
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "EventInfoReader::execute()" << endreq;

    const EventInfo * evt = 0;
    StatusCode sc = m_storeGate->retrieve( evt );
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
 	    << evt->event_type()->user_type() << " weight "
 	    << evt->event_type()->mc_event_weight() 
 	    << endreq;
    }
 

    // Print out the tags found
    log << MSG::DEBUG << "Tag pairs from EventInfo: "
	<< endreq;
    EventType::NameTagPairVec pairs1;
    evt->event_type()->get_detdescr_tags(pairs1);
    for (unsigned int i = 0; i < pairs1.size(); ++i) {
	std::string name = pairs1[i].first;
	std::string tag  = pairs1[i].second;
	log << MSG::DEBUG << name << " : "
	    << tag
	    << endreq;
    }



    // Get TagInfo and add tags

    const TagInfo* tagInfo = 0;
    // Try to get tagInfo if there, otherwise create
    sc = m_detStore->retrieve( tagInfo );
    if (sc.isFailure()) {
	log << MSG::DEBUG << "No TagInfo in DetectorStore - creating one" << endreq;
	tagInfo = new TagInfo();
    } 
    else {
	log << MSG::DEBUG << "Retrieved TagInfo" << endreq;
    } 

    // Dump out contents of TagInfo
    log << MSG::DEBUG << "Tags from  TagInfo:" << endreq;
    tagInfo->printTags(log);
    
    // Print out current Release version 
    std::string releaseVersion;
    tagInfo->findTag("AtlasRelease", releaseVersion);
    log << MSG::DEBUG << "Found Release version from TagInfo: " 
	<< releaseVersion << endreq;

    // Print out input Release version 
    //tagInfo->findInputTag("AtlasRelease", releaseVersion);
    //log << MSG::DEBUG << "Found input AtlasRelease version from TagInfo: " 
    //<< releaseVersion << endreq;


    return StatusCode::SUCCESS;
}


// Finalize method:
StatusCode EventInfoReader::finalize() 
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "EventInfoReader::finalize()" << endreq;

    return StatusCode::SUCCESS;
}


