/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
    ATH_CHECK( m_eventInfoKey.initialize() );
    return StatusCode::SUCCESS;
}

// Execute method:
StatusCode EventInfoReader::execute() 
{
    ATH_MSG_DEBUG("EventInfoReader::execute()" );

    SG::ReadHandle<xAOD::EventInfo> eventInfo (m_eventInfoKey, getContext());
    if(!eventInfo.isValid()) {
      ATH_MSG_ERROR("  Could not get event info");      
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG( "Event ID: ["
                   << eventInfo->runNumber()   << ","
                   << eventInfo->eventNumber() << ":"
                   << eventInfo->timeStamp() << "] " );
    ATH_MSG_DEBUG( "Event type: IS_SIMULATION " << std::boolalpha
		   << eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION) << " weight "
                   << eventInfo->mcEventWeight()  );
 

    // Print out the tags found
    ATH_MSG_DEBUG("Tag pairs from EventInfo: " );
    for (const auto& ddTag : eventInfo->detDescrTags() ) {
      ATH_MSG_DEBUG( ddTag.first << " : " << ddTag.second);
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


