/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolTestAttrWriter.cxx
 *
 * @brief Test Algorithm for testing creation of explicit collections,
 * writes an AttributeList to the transient store each event.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: AthenaPoolTestAttrWriter.cxx,v 1.6 2006-03-26 18:16:09 schaffer Exp $
 *
 */

// INCLUDE HEADER FILES:

#include "AthenaPoolTestAttrWriter.h"


#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// AttributeList:
#include "AthenaPoolUtilities/AthenaAttributeList.h"

AthenaPoolTestAttrWriter::AthenaPoolTestAttrWriter(const std::string& name,
						   ISvcLocator* pSvcLocator)
    :
    AthAlgorithm(name, pSvcLocator), m_attributes(0)
{}

AthenaPoolTestAttrWriter::~AthenaPoolTestAttrWriter()
{}

StatusCode AthenaPoolTestAttrWriter::initialize()
{

    // Create AttrList
    try {
 	ATH_MSG_DEBUG("Try new attlist" );
 	m_attributes = new AthenaAttributeList();
	m_attributes->extend("Run",        "unsigned int" );
	m_attributes->extend("Event",      "unsigned int" );
	m_attributes->extend("IEvent",     "unsigned int");
	m_attributes->extend("MissingET",  "float");
	m_attributes->extend("electronPT", "float");
 	ATH_MSG_DEBUG("new attlist" );
    } catch (std::exception e) {
 	ATH_MSG_ERROR("Caught exception from creation of AthenaAttributeList. Message: " 
 	    << e.what() 
 	    );
 	return (StatusCode::FAILURE);
    }
    m_attributes->addRef();

    ATH_MSG_DEBUG("After new att list" );

    return StatusCode::SUCCESS; 
}

StatusCode AthenaPoolTestAttrWriter::execute()
{ 
    ATH_MSG_DEBUG("Executing AthenaPoolTestAttrWriter" );
   
    const EventInfo* eventInfo = nullptr;
    ATH_CHECK(  evtStore()->retrieve(eventInfo) );
    ATH_MSG_DEBUG( "Retrieved EventInfo"  );

    const EventID* myEventID=eventInfo->event_ID();
 
    // Create AttrList
    static unsigned int iEvent = 0;
    AthenaAttributeList* attributes = m_attributes;
    
    try {
	(*attributes)["Run"].setValue(myEventID->run_number());
	(*attributes)["Event"].setValue(myEventID->event_number());
	++iEvent;
	(*attributes)["IEvent"].setValue(iEvent);
	// Must cast double to float, otherwise exception!!
	(*attributes)["MissingET"].setValue(7.0f*iEvent);
	(*attributes)["electronPT"].setValue(5.0f*iEvent);
    } catch (std::exception e) {
	ATH_MSG_ERROR("Caught exception from setValue for attributes. Message: " 
	    << e.what() 
	    );
	return (StatusCode::FAILURE);
    }

    ATH_MSG_DEBUG("After create att" );

    coral::AttributeList::const_iterator first = attributes->begin();
    coral::AttributeList::const_iterator last  = attributes->end();
    for (; first != last; ++first) {
	std::stringstream str;
	(*first).toOutputStream(str);
	ATH_MSG_DEBUG(" name " << (*first).specification().name() 
                      << " type " << (*first).specification().typeName() 
                      << " value ");
    }

    ATH_MSG_DEBUG("After iteration" );

    ATH_CHECK(  evtStore()->record(attributes, "SimpleTag") );
    ATH_MSG_DEBUG("End" );
    return StatusCode::SUCCESS;
   
}

StatusCode AthenaPoolTestAttrWriter::finalize() 
{
  return StatusCode::SUCCESS;
}


