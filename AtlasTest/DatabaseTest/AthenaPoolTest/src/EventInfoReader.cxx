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

#include "EventInfoReader.h"

// Event includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TagInfo.h"

// GeoModel
#include "GeoModelInterfaces/IGeoModelSvc.h"

// Constructor with parameters:
EventInfoReader::EventInfoReader(const std::string &name, 
				 ISvcLocator *pSvcLocator) :
    AthAlgorithm(name,pSvcLocator),
    m_geoModel("GeoModelSvc", name)
{}

// Initialize method:
StatusCode EventInfoReader::initialize()
{
    ATH_MSG_INFO( "EventInfoReader::initialize()"  );
    ATH_CHECK( m_geoModel.retrieve() );
    return StatusCode::SUCCESS;
}

// Execute method:
StatusCode EventInfoReader::execute() 
{
    ATH_MSG_DEBUG("EventInfoReader::execute()" );

    const EventInfo * evt = 0;
    ATH_CHECK( evtStore()->retrieve( evt ) );
    ATH_MSG_DEBUG( "Event ID: ["
                   << evt->event_ID()->run_number()   << ","
                   << evt->event_ID()->event_number() << ":"
                   << evt->event_ID()->time_stamp() << "] " );
    ATH_MSG_DEBUG( "Event type: user type "
                   << evt->event_type()->user_type() << " weight "
                   << evt->event_type()->mc_event_weight()  );
 

    // Print out the tags found
    ATH_MSG_DEBUG("Tag pairs from EventInfo: " );
    EventType::NameTagPairVec pairs1;
    evt->event_type()->get_detdescr_tags(pairs1);
    for (unsigned int i = 0; i < pairs1.size(); ++i) {
	std::string name = pairs1[i].first;
	std::string tag  = pairs1[i].second;
	ATH_MSG_DEBUG( name << " : " << tag );
    }



    // Get TagInfo and add tags

    TagInfo tagInfo_local;
    const TagInfo* tagInfo = 0;
    // Try to get tagInfo if there, otherwise create
    if (detStore()->retrieve( tagInfo ).isFailure()) {
	ATH_MSG_DEBUG("No TagInfo in DetectorStore - creating one" );
	tagInfo = &tagInfo_local;
    } 
    else {
	ATH_MSG_DEBUG("Retrieved TagInfo" );
    } 

    // Dump out contents of TagInfo
    ATH_MSG_DEBUG("Tags from  TagInfo:" );
    tagInfo->printTags(msg());
    
    // Print out current Release version 
    std::string releaseVersion;
    tagInfo->findTag("AtlasRelease", releaseVersion);
    ATH_MSG_DEBUG( "Found Release version from TagInfo: "  << releaseVersion  );

    return StatusCode::SUCCESS;
}


// Finalize method:
StatusCode EventInfoReader::finalize() 
{
    ATH_MSG_INFO("EventInfoReader::finalize()" );
    return StatusCode::SUCCESS;
}


