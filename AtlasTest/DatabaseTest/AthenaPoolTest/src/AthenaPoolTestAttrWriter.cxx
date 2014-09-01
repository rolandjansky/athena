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

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ISvcLocator.h"
//#include "GaudiKernel/ListItem.h"
#include "StoreGate/StoreGateSvc.h"


AthenaPoolTestAttrWriter::AthenaPoolTestAttrWriter(const std::string& name,
						   ISvcLocator* pSvcLocator)
    :
    Algorithm(name, pSvcLocator)
{}

AthenaPoolTestAttrWriter::~AthenaPoolTestAttrWriter()
{}

StatusCode AthenaPoolTestAttrWriter::initialize()
{
  MsgStream log( messageService(), name());

  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure())
    {
      log << MSG::ERROR
          << "Unable to get pointer to StoreGate Service"
          << endreq;
      return sc;
    }

    // Create AttrList
    try {
 	log << MSG::DEBUG << "Try new attlist" << endreq;
 	m_attributes = new AthenaAttributeList();
	m_attributes->extend("Run",        "unsigned int" );
	m_attributes->extend("Event",      "unsigned int" );
	m_attributes->extend("IEvent",     "unsigned int");
	m_attributes->extend("MissingET",  "float");
	m_attributes->extend("electronPT", "float");
 	log << MSG::DEBUG << "new attlist" << endreq;
    } catch (std::exception e) {
 	log << MSG::ERROR 
 	    << "Caught exception from creation of AthenaAttributeList. Message: " 
 	    << e.what() 
 	    << endreq;
 	return (StatusCode::FAILURE);
    }
    m_attributes->addRef();

    log << MSG::DEBUG << "After new att list" << endreq;

    return StatusCode::SUCCESS; 
}

StatusCode AthenaPoolTestAttrWriter::execute()
{ 

    StatusCode sc;

    MsgStream log( messageService(), name() );   
    log << MSG::DEBUG << "Executing AthenaPoolTestAttrWriter" << endreq;
   

    const EventInfo* eventInfo;
    sc = m_storeGate->retrieve(eventInfo);
    if (sc.isFailure()) {
	log << MSG::ERROR << "Could not retrieve event info" << endreq;
    }

    log << MSG::DEBUG << "Retrieved EventInfo" << endreq;

    EventID* myEventID=eventInfo->event_ID();
 
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
	log << MSG::ERROR 
	    << "Caught exception from setValue for attributes. Message: " 
	    << e.what() 
	    << endreq;
	return (StatusCode::FAILURE);
    }

    log << MSG::DEBUG << "After create att" << endreq;

    coral::AttributeList::const_iterator first = attributes->begin();
    coral::AttributeList::const_iterator last  = attributes->end();
    for (; first != last; ++first) {
	log << MSG::DEBUG << " name " << (*first).specification().name() 
	    << " type " << (*first).specification().typeName() 
	    << " value ";
	std::stringstream str;
	(*first).toOutputStream(str);
	log << MSG::DEBUG << str << endreq;
    }

    log << MSG::DEBUG << "After iteration" << endreq;

    sc = m_storeGate->record(attributes, "SimpleTag");
    if (sc.isFailure()) {
	log << MSG::ERROR << "Could not record attributes" << endreq;
	return  StatusCode::FAILURE;
    }

    log << MSG::DEBUG << "End" << endreq;

    return StatusCode::SUCCESS;
   
}

StatusCode AthenaPoolTestAttrWriter::finalize() 
{
  return StatusCode::SUCCESS;
}


