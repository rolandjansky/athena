/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file EventInfoWriter.cxx
 *
 * @brief Test Algorithm for checking TagInfoMgr and the storing of
 * geometry and conditions tags in EventInfo, writes extra tags.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: EventInfoWriter.cxx,v 1.9 2009-04-29 07:49:45 schaffer Exp $
 *
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "EventInfoWriter.h"

// Event includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TagInfo.h"
#include "EventInfoMgt/ITagInfoMgr.h"

// TES include
#include "StoreGate/StoreGateSvc.h"

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

// Constructor with parameters:
EventInfoWriter::EventInfoWriter(const std::string &name, 
				 ISvcLocator *pSvcLocator) :
        AthAlgorithm(name,pSvcLocator),
        m_createDummyTags(false),
        m_removeDummyTags(false),
        m_createDummyOverrideTags(false),
        m_tagInfoMgr(0)
{
    declareProperty("CreateDummyTags", m_createDummyTags);
    declareProperty("RemoveDummyTags", m_removeDummyTags);
    declareProperty("CreateDummyOverrideTags", m_createDummyOverrideTags);
}

// Initialize method:
StatusCode EventInfoWriter::initialize()
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "EventInfoWriter::initialize()" << endreq;

    log << MSG::DEBUG << "CreateDummyTags         " << m_createDummyTags
	<< endreq;
    log << MSG::DEBUG << "RemoveDummyTags         " << m_removeDummyTags
	<< endreq;
    log << MSG::DEBUG << "CreateDummyOverrideTags " << m_createDummyOverrideTags
	<< endreq;


    // get TagInfoMgr service
    if (service("TagInfoMgr", m_tagInfoMgr).isFailure()) {
	log << MSG::FATAL << "TagInfoMgr service not found !" << endreq;
	return StatusCode::FAILURE;
    } 

//     // Register callback to test when TagInfo has changed
//     const DataHandle<TagInfo> tagInfoH;
//     std::string key = m_tagInfoMgr->tagInfoKey();
//     if (m_detStore->regFcn(&EventInfoWriter::checkTagInfo,
// 			     this, tagInfoH, key).isFailure()) {
// 	log << MSG::ERROR << "Cannot register checkTagInfo function for key " 
// 	    << key
// 	    << endreq;
// 	return sc;
//     } else {
// 	log << MSG::DEBUG << "Registered checkTagInfo callback for key: " 
// 	    << key
// 	    << endreq;
//     }

    // Insert a dummy tag into the TagInfoMgr
    if (m_tagInfoMgr->addTag("EventInfoWriterTag", "EIW-00-00-01").isFailure()) {
	log << MSG::FATAL << "Unable to add tag to TagInfoMgr !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << "Added tag to TagInfoMgr" << endreq;
    }

    if (fillTagInfo().isFailure()) {
	log << MSG::FATAL << "Unable to to call fillTagInfo !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << "Called fillTagInfo" << endreq;
    }

    return StatusCode::SUCCESS;
}

// Execute method:
StatusCode EventInfoWriter::execute() 
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "EventInfoWriter::execute()" << endreq;

    const EventInfo * evt = 0;
    if (evtStore()->retrieve( evt ).isFailure() ) {
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
 
    // Set new dummy tags only at the first event of run 2
    static bool setDummyTags = false; // flag to set tags only once
    if (!setDummyTags && evt->event_ID()->run_number() == 2 && m_createDummyOverrideTags) {
        setDummyTags = true;
	msg(MSG::DEBUG) << "Create dummy tags, A2-D2" << endreq;
	// New/different tags
        if (m_tagInfoMgr->addTag("TagA1", "A2").isFailure()) {
            msg(MSG::WARNING) << "TagA1 NOT added" << endreq;
        } 
        if (m_tagInfoMgr->addTag("TagB1", "B2").isFailure()) {
	    msg(MSG::WARNING) << "TagB1 NOT added" << endreq;
	} 
        if (m_tagInfoMgr->addTag("TagC1", "C2").isFailure()) {
	    msg(MSG::WARNING) << "TagC1 NOT added" << endreq;
	} 
        if (m_tagInfoMgr->addTag("TagD1", "D2").isFailure()) {
	    msg(MSG::WARNING) << "TagD1 NOT added" << endreq;
	} 

        msg (MSG::DEBUG) << "execute: reset TagInfo proxy" << endreq;

        SG::DataProxy* tagInfoProxy = 
            detStore()->proxy( ClassID_traits<TagInfo>::ID() );

        if (0 == tagInfoProxy) {
            msg(MSG::ERROR) << "execute: Unable to retrieve TagInfo object with clid/key: " 
                            << ClassID_traits<TagInfo>::ID() << endreq;
            return StatusCode::FAILURE;
        }
        // reset and retrieve
        tagInfoProxy->reset();
        const TagInfo* tagInfo = 0;
        if (detStore()->retrieve( tagInfo ).isFailure() ) {
            msg(MSG::ERROR) << "execute: Could not retrieve TagInfo object from the detector store" << endreq;      
            return StatusCode::FAILURE;
        }
        else {
            msg(MSG::DEBUG) << "execute: retrieved TagInfo" << endreq;
        }

    }


    // Get TagInfo and add tags

    const TagInfo* tagInfo = 0;
    // Try to get tagInfo if there, otherwise create
    if (detStore()->retrieve( tagInfo ).isFailure()) {
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

    return StatusCode::SUCCESS;
}


// Finalize method:
StatusCode EventInfoWriter::finalize() 
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "EventInfoWriter::finalize()" << endreq;

    return StatusCode::SUCCESS;
}

StatusCode 
EventInfoWriter::fillTagInfo    () const
{

    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "EventInfoWriter::fillTagInfo()" << endreq;

    //
    //  For tests, add in dummy tags
    //
    if(m_createDummyTags) {
	log << MSG::DEBUG << "Create dummy tags, A1-D1" << endreq;
	// New/different tags
        if (m_tagInfoMgr->addTag("TagA1", "A1").isFailure()) {
	    log << MSG::WARNING << "TagA1 NOT added" << endreq;
        } 
        if (m_tagInfoMgr->addTag("TagB1", "B1").isFailure()) {
	    log << MSG::WARNING << "TagB1 NOT added" << endreq;
	} 
        if (m_tagInfoMgr->addTag("TagC1", "C1").isFailure()) {
	    log << MSG::WARNING << "TagC1 NOT added" << endreq;
	} 
        if (m_tagInfoMgr->addTag("TagD1", "D1").isFailure()) {
	    log << MSG::WARNING << "TagD1 NOT added" << endreq;
	} 
    }

    // Remove incoming dummy tag
    if (m_removeDummyTags) {
	log << MSG::DEBUG << "Create dummy tags, A1-D1" << endreq;
        if (m_tagInfoMgr->removeTagFromInput("TagC1").isFailure()) {
	    log << MSG::WARNING << "TagC1 NOT removed" << endreq;
	} 
    }
        
    return StatusCode::SUCCESS;

}



StatusCode
EventInfoWriter::checkTagInfo(IOVSVC_CALLBACK_ARGS)
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "EventInfoWriter::checkTagInfo - called by IOVSvc" << endreq;
    return (fillTagInfo());
}

