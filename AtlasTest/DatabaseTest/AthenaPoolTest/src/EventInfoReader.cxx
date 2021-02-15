/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file EventInfoReader.cxx
 *
 * @brief Test Algorithm for checking TagInfoMgr and the storing of
 * geometry and conditions tags in EventInfo, checks tags on reading
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "EventInfoReader.h"

// Event includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"

// Constructor with parameters:
EventInfoReader::EventInfoReader(const std::string &name, 
				 ISvcLocator *pSvcLocator) :
    AthAlgorithm(name,pSvcLocator),
    m_geoModel("GeoModelSvc", name),
    m_tagInfoMgr("TagInfoMgr", name)
{}

// Initialize method:
StatusCode EventInfoReader::initialize()
{
    ATH_MSG_INFO( "EventInfoReader::initialize()"  );
    ATH_CHECK( m_geoModel.retrieve() );
    ATH_CHECK( m_tagInfoMgr.retrieve() );
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

    // Dump out Tags
    ATH_MSG_DEBUG("Tags from TagInfoMgr:");
    ATH_MSG_DEBUG( m_tagInfoMgr->dumpTagInfoToStr() );
    
    // Print out current Release version 
    std::string releaseVersion = m_tagInfoMgr->findTag("AtlasRelease");
    ATH_MSG_DEBUG("Found Release version from TagInfoMgr: " << releaseVersion);

    return StatusCode::SUCCESS;
}


// Finalize method:
StatusCode EventInfoReader::finalize() 
{
    ATH_MSG_INFO("EventInfoReader::finalize()" );
    return StatusCode::SUCCESS;
}


