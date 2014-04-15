/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RunEventTagWriter.cxx
 * @brief Implementation of class RunEventTagWriter
 * @author K. Karr (Kristo.Karr@cern.ch)
 * @author J. Cranshaw (Jack.Cranshaw@cern.ch)
 * $Id: RunEventTagWriter.cxx,v 1.4 2009-02-19 22:04:49 cranshaw Exp $ 
 */

#include "RunEventTagWriter.h"

#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "AthenaPoolUtilities/TagAthenaAttributeList.h"
#include "AthenaPoolUtilities/AthenaAttributeListSpecification.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"

#include "POOLCore/Exception.h"

#include <cassert>
#include <stdlib.h>


RunEventTagWriter::RunEventTagWriter(const std::string& name, 
                                     ISvcLocator* pSvcLocator) 
    : Algorithm(name, pSvcLocator),
      m_storeGateSvc(0),
      m_attribListSpec(0)
{}

 
RunEventTagWriter::~RunEventTagWriter() 
{
  if ( m_attribListSpec )
  {
    m_attribListSpec->release();
  }
}


StatusCode RunEventTagWriter::initialize() 
{
  StatusCode sc = StatusCode::SUCCESS;
  std::string attribName;

  MsgStream log(messageService(), name());

  log << MSG::DEBUG << "Initializing " << name() << endreq;

  sc = service("StoreGateSvc", m_storeGateSvc);
  if (sc.isFailure()) 
  {
    log << MSG::ERROR << "Unable to get pointer to StoreGate service."
         << endreq;
    return sc;
  }

  log << MSG::DEBUG << "Defining the attribute list specification." << endreq;
  m_attribListSpec = new AthenaAttributeListSpecification();

  m_attribListSpec->extend("RunNumber", "unsigned int");
  m_attribListSpec->extend("EventNumber", "unsigned int");

  log << MSG::DEBUG << "Printing out attribute list specification:" << endreq;
  IMessageSvc* msgSvc;
  sc = Gaudi::svcLocator()->service("MessageSvc", msgSvc);
  //if (sc.isSuccess() && msgSvc && (msgSvc->outputLevel() <= MSG::DEBUG))
  if (sc.isSuccess())
  {
    log << MSG::DEBUG << "Attribute List Specification: " << endreq;
    AthenaAttributeListSpecification::const_iterator first = m_attribListSpec->begin();
    AthenaAttributeListSpecification::const_iterator last  = m_attribListSpec->end();
    for (; first != last; ++first) 
    {
      log << MSG::DEBUG << " name " << (*first).name() 
                        << " type " << (*first).typeName() 
                        << endreq;
    }
  }

  return (StatusCode::SUCCESS);
}


StatusCode RunEventTagWriter::execute() 
{
  StatusCode sc = StatusCode::SUCCESS;
  std::string attribName;

  MsgStream log(messageService(), name());

  log << MSG::DEBUG << "Executing " << name() << endreq;

  TagAthenaAttributeList* attribList = 0;
  log << MSG::DEBUG << "Creating TagAthenaAttributeList object." << endreq;
  try 
  {
    attribList = new TagAthenaAttributeList(*m_attribListSpec);
  } 
  catch (pool::Exception e) 
  {
    log << MSG::ERROR
        << "Caught exception during creation of TagAthenaAttributeList object."
        << "Message: " << e.what() << endreq;
    return (StatusCode::FAILURE);
  }

  log << MSG::DEBUG << "Retrieving event info from TDS." << endreq;
  const DataHandle<EventInfo> eventInfo;
  sc = m_storeGateSvc->retrieve(eventInfo);
  if (sc.isFailure()) 
  {
    log << MSG::ERROR << "Could not retrieve event info from TDS." << endreq;
  }

  unsigned int runNumber = eventInfo->event_ID()->run_number();
  unsigned int eventNumber = eventInfo->event_ID()->event_number();

  if (attribList == false)
  {
    log << MSG::ERROR << "Attribute list object is NULL." << endreq;
    return (StatusCode::FAILURE);
  }

  log << MSG::DEBUG << "About to assign values to Tag Attrib List" << endreq;
  try
  {
    (*attribList)["RunNumber"].data<unsigned int>() = runNumber;
    (*attribList)["EventNumber"].data<unsigned int>() = eventNumber;
  } 
  catch (pool::Exception e) 
  {
    log << MSG::ERROR
        << "Caught exception from data() when setting AOD global "
        << "attributes; Message: " << e.what()
        << endreq;
    return (StatusCode::FAILURE);
  }
  log << MSG::DEBUG << "Finished adding Run,Event data to TagAthenaAttributeList."
      << endreq;

  sc = m_storeGateSvc->record(attribList, "RunEventTag");
  if (sc.isFailure()) 
  {
    log << MSG::ERROR << "Could not record TagAthenaAttributeList object."
         << endreq;
    return (StatusCode::FAILURE);
  }

  log << MSG::DEBUG << "Printing out attribute list:" << endreq;
  std::ostringstream attribListStream;
  attribList->toOutputStream(attribListStream);
  log << MSG::DEBUG << "Attribute List: " 
                    << attribListStream.str() 
                    << endreq;

  return (StatusCode::SUCCESS);
}


StatusCode RunEventTagWriter::finalize() 
{
  MsgStream log(messageService(), name());

  log << MSG::DEBUG << "Finalizing " << name() << endreq;

  return (StatusCode::SUCCESS);
}
