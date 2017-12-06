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
    ATH_MSG_DEBUG("EventInfoWriter::initialize()");

    ATH_MSG_DEBUG("CreateDummyTags         " << m_createDummyTags);
    ATH_MSG_DEBUG("RemoveDummyTags         " << m_removeDummyTags);
    ATH_MSG_DEBUG("CreateDummyOverrideTags " << m_createDummyOverrideTags);


    // get TagInfoMgr service
    if (service("TagInfoMgr", m_tagInfoMgr).isFailure()) {
	ATH_MSG_FATAL("TagInfoMgr service not found !");
	return StatusCode::FAILURE;
    } 

//     // Register callback to test when TagInfo has changed
//     const DataHandle<TagInfo> tagInfoH;
//     std::string key = m_tagInfoMgr->tagInfoKey();
//     if (m_detStore->regFcn(&EventInfoWriter::checkTagInfo,
// 			     this, tagInfoH, key).isFailure()) {
// 	ATH_MSG_ERROR("Cannot register checkTagInfo function for key " 
// 	    << key
// 	   );
// 	return sc;
//     } else {
// 	ATH_MSG_DEBUG("Registered checkTagInfo callback for key: " 
// 	    << key
// 	   );
//     }

    // Insert a dummy tag into the TagInfoMgr
    if (m_tagInfoMgr->addTag("EventInfoWriterTag", "EIW-00-00-01").isFailure()) {
	ATH_MSG_FATAL("Unable to add tag to TagInfoMgr !");
	return StatusCode::FAILURE;
    } 
    else {
	ATH_MSG_DEBUG("Added tag to TagInfoMgr");
    }

    if (fillTagInfo().isFailure()) {
	ATH_MSG_FATAL("Unable to to call fillTagInfo !");
	return StatusCode::FAILURE;
    } 
    else {
	ATH_MSG_DEBUG("Called fillTagInfo");
    }

    return StatusCode::SUCCESS;
}

// Execute method:
StatusCode EventInfoWriter::execute() 
{
    // Get the messaging service, print where you are
    ATH_MSG_DEBUG("EventInfoWriter::execute()");

    const EventInfo * evt = 0;
    if (evtStore()->retrieve( evt ).isFailure() ) {
 	ATH_MSG_ERROR("  Could not get event info");      
 	return StatusCode::FAILURE;
    }
    else {
 	ATH_MSG_DEBUG("Event ID: ["
 	    << evt->event_ID()->run_number()   << ","
            << evt->event_ID()->event_number() << ":"
 	    << evt->event_ID()->time_stamp() << "] "
 	   );
 	ATH_MSG_DEBUG("Event type: user type "
 	    << evt->event_type()->user_type()
 	   );
    }
 
    // Set new dummy tags only at the first event of run 2
    static bool setDummyTags = false; // flag to set tags only once
    if (!setDummyTags && evt->event_ID()->run_number() == 2 && m_createDummyOverrideTags) {
        setDummyTags = true;
        ATH_MSG_DEBUG("Create dummy tags, A2-D2");
	// New/different tags
        if (m_tagInfoMgr->addTag("TagA1", "A2").isFailure()) {
            ATH_MSG_WARNING("TagA1 NOT added");
        } 
        if (m_tagInfoMgr->addTag("TagB1", "B2").isFailure()) {
            ATH_MSG_WARNING("TagB1 NOT added");
	} 
        if (m_tagInfoMgr->addTag("TagC1", "C2").isFailure()) {
            ATH_MSG_WARNING("TagC1 NOT added");
	} 
        if (m_tagInfoMgr->addTag("TagD1", "D2").isFailure()) {
            ATH_MSG_WARNING("TagD1 NOT added");
	} 

ATH_MSG_DEBUG("execute: reset TagInfo proxy");

        SG::DataProxy* tagInfoProxy = 
            detStore()->proxy( ClassID_traits<TagInfo>::ID() );

        if (0 == tagInfoProxy) {
            ATH_MSG_ERROR("execute: Unable to retrieve TagInfo object with clid/key: " 
                            << ClassID_traits<TagInfo>::ID());
            return StatusCode::FAILURE;
        }
        // reset and retrieve
        tagInfoProxy->reset();
        const TagInfo* tagInfo = 0;
        if (detStore()->retrieve( tagInfo ).isFailure() ) {
            ATH_MSG_ERROR("execute: Could not retrieve TagInfo object from the detector store");      
            return StatusCode::FAILURE;
        }
        else {
            ATH_MSG_DEBUG("execute: retrieved TagInfo");
        }

    }


    // Get TagInfo and add tags

    TagInfo tagInfo_local;
    const TagInfo* tagInfo = 0;
    // Try to get tagInfo if there, otherwise create
    if (detStore()->retrieve( tagInfo ).isFailure()) {
	ATH_MSG_DEBUG("No TagInfo in DetectorStore - creating one");
	tagInfo = &tagInfo_local;
    } 
    else {
	ATH_MSG_DEBUG("Retrieved TagInfo");
    } 

    // Dump out contents of TagInfo
    ATH_MSG_DEBUG("Tags from  TagInfo:");
    MsgStream log(msgSvc(), name());
    tagInfo->printTags(log);
    
    // Print out current Release version 
    std::string releaseVersion;
    tagInfo->findTag("AtlasRelease", releaseVersion);
    ATH_MSG_DEBUG("Found Release version from TagInfo: " 
	<< releaseVersion);

    return StatusCode::SUCCESS;
}


// Finalize method:
StatusCode EventInfoWriter::finalize() 
{
    // Get the messaging service, print where you are
    ATH_MSG_DEBUG("EventInfoWriter::finalize()");

    return StatusCode::SUCCESS;
}

StatusCode 
EventInfoWriter::fillTagInfo    () const
{

    // Get the messaging service, print where you are
    ATH_MSG_DEBUG("EventInfoWriter::fillTagInfo()");

    //
    //  For tests, add in dummy tags
    //
    if(m_createDummyTags) {
	ATH_MSG_DEBUG("Create dummy tags, A1-D1");
	// New/different tags
        if (m_tagInfoMgr->addTag("TagA1", "A1").isFailure()) {
	    ATH_MSG_WARNING("TagA1 NOT added");
        } 
        if (m_tagInfoMgr->addTag("TagB1", "B1").isFailure()) {
	    ATH_MSG_WARNING("TagB1 NOT added");
	} 
        if (m_tagInfoMgr->addTag("TagC1", "C1").isFailure()) {
	    ATH_MSG_WARNING("TagC1 NOT added");
	} 
        if (m_tagInfoMgr->addTag("TagD1", "D1").isFailure()) {
	    ATH_MSG_WARNING("TagD1 NOT added");
	} 
    }

    // Remove incoming dummy tag
    if (m_removeDummyTags) {
	ATH_MSG_DEBUG("Create dummy tags, A1-D1");
        if (m_tagInfoMgr->removeTagFromInput("TagC1").isFailure()) {
	    ATH_MSG_WARNING("TagC1 NOT removed");
	} 
    }
        
    return StatusCode::SUCCESS;

}



StatusCode
EventInfoWriter::checkTagInfo(IOVSVC_CALLBACK_ARGS)
{
    // Get the messaging service, print where you are
    ATH_MSG_DEBUG("EventInfoWriter::checkTagInfo - called by IOVSvc");
    return (fillTagInfo());
}

