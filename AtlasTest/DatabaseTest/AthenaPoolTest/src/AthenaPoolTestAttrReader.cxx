/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolTestAttrReader.cxx
 *
 * @brief Test Algorithm for reading an AttributeList from the
 * transient store
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: AthenaPoolTestAttrReader.cxx,v 1.4 2005-06-01 07:56:30 schaffer Exp $
 *
 */


// INCLUDE HEADER FILES:


#include "AthenaPoolTestAttrReader.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ISvcLocator.h"

// Athena
#include "StoreGate/StoreGateSvc.h"
//#include "EventSelectorAthenaPool/EventSelectorAthenaPool.h"

// AttributeList:
#include "AthenaPoolUtilities/AthenaAttributeList.h"


AthenaPoolTestAttrReader::AthenaPoolTestAttrReader(const std::string& name,
					 ISvcLocator* pSvcLocator):
    Algorithm(name, pSvcLocator)
{}

AthenaPoolTestAttrReader::~AthenaPoolTestAttrReader()
{}

StatusCode AthenaPoolTestAttrReader::initialize()
{
    MsgStream log( messageService(), name());

//     StatusCode sc = service("EventSelector", m_evtSel);
//     if (sc.isFailure())
// 	{
// 	    log << MSG::ERROR
// 		<< "Unable to get pointer to AthenaPoolEventSelector"
// 		<< endreq;
// 	    return sc;
// 	}


//     IService* svc ;
//     StatusCode sc = serviceLocator()->getService("EventSelector", svc);
//     if (sc != StatusCode::SUCCESS ) {
// 	log << MSG::ERROR 
// 	    << " Cannot get EventSelector " 
// 	    << endreq;
// 	return sc ;
//     }
 

//     m_evtSel = dynamic_cast<EventSelectorAthenaPool*> (svc);
//     if(m_evtSel == 0 ) {
// 	log << MSG::ERROR
// 	    << "  Cannot cast to EventSelectorAthenaPool " 
// 	    << endreq;
// 	return StatusCode::FAILURE ;
//      }
//     else {
// 	log << MSG::DEBUG << "Got EventSelectorAthenaPool " << endreq;
//     }
    
 

    return StatusCode::SUCCESS; 
      
}

StatusCode AthenaPoolTestAttrReader::execute()
{ 

    StatusCode sc;

    MsgStream log( messageService(), name() );   
    log << MSG::DEBUG << "Executing AthenaPoolTestAttrReader" << endreq;
   

    // Create AttrList
//     const AthenaAttributeList& attributes = m_evtSel->attributeList();
//     pool::AttributeList::const_iterator first = attributes.begin();
//     pool::AttributeList::const_iterator last  = attributes.end();
//     for (; first != last; ++first) {
// 	log << MSG::DEBUG << " name " << (*first).spec().name() 
// 	    << " type " << (*first).spec().type_name() 
// 	    << " value " << (*first).getValueAsString() << endreq;
//     }

				    
    return StatusCode::SUCCESS;
   
}

StatusCode AthenaPoolTestAttrReader::finalize() 
{
    return StatusCode::SUCCESS;
}


