/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RunEventMetaWriter.cxx
 * @brief Implementation of class RunEventMetaWriter
 * @author K. Karr (Kristo.Karr@cern.ch)
 * @author J. Cranshaw (Jack.Cranshaw@cern.ch)
 * $Id: RunEventMetaWriter.cxx,v 1.4 2009-02-19 22:04:49 cranshaw Exp $ 
 */

#include "RunEventMetaWriter.h"

#include "StoreGate/StoreGateSvc.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/AttributeListSpecification.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"

#include "POOLCore/Exception.h"

#include <cassert>
#include <stdlib.h>


RunEventMetaWriter::RunEventMetaWriter(const std::string& name, 
                                     ISvcLocator* pSvcLocator) 
    : AthAlgorithm(name, pSvcLocator),
      m_attribListSpec(0)
{
   // Declare the properties
}

 
RunEventMetaWriter::~RunEventMetaWriter() 
{
  if ( m_attribListSpec )
  {
    m_attribListSpec->release();
  }
}


StatusCode RunEventMetaWriter::initialize() 
{
  StatusCode sc = StatusCode::SUCCESS;
  std::string attribName;

  MsgStream log(messageService(), name());

  log << MSG::DEBUG << "Initializing " << name() << endreq;

  log << MSG::DEBUG << "Defining the attribute list specification." << endreq;
  m_attribListSpec = new coral::AttributeListSpecification();

  m_attribListSpec->extend("RunNumber", "unsigned int");
  m_attribListSpec->extend("EventNumber", "unsigned int");

  log << MSG::DEBUG << "Printing out attribute list specification:" << endreq;
  IMessageSvc* msgSvc;
  sc = Gaudi::svcLocator()->service("MessageSvc", msgSvc);
  if (sc.isSuccess())
  {
    log << MSG::DEBUG << "Attribute List Specification: " << endreq;
    coral::AttributeListSpecification::const_iterator first = m_attribListSpec->begin();
    coral::AttributeListSpecification::const_iterator last  = m_attribListSpec->end();
    for (; first != last; ++first) 
    {
      log << MSG::DEBUG << " name " << (*first).name() 
                        << " type " << (*first).typeName() 
                        << endreq;
    }
  }

  return (StatusCode::SUCCESS);
}

StatusCode RunEventMetaWriter::execute() 
{
  return StatusCode::SUCCESS;
}

StatusCode RunEventMetaWriter::start() 
{
  std::string attribName;

  MsgStream log(messageService(), name());

  log << MSG::DEBUG << "Executing " << name() << endreq;

  AthenaAttributeList* attribList = 0;
  log << MSG::DEBUG << "Creating AthenaAttributeList object." << endreq;
  try 
  {
    attribList = new AthenaAttributeList(*m_attribListSpec);
  } 
  catch (pool::Exception e) 
  {
    log << MSG::ERROR
        << "Caught exception during creation of AthenaAttributeList object."
        << "Message: " << e.what() << endreq;
    return (StatusCode::FAILURE);
  }

  if (!attribList)
  {
    log << MSG::ERROR << "Attribute list object is NULL." << endreq;
    return (StatusCode::FAILURE);
  }

  log << MSG::DEBUG << "About to assign values to Tag Attrib List" << endreq;
  try
  {
    (*attribList)["RunNumber"].data<unsigned int>() = 88;
    (*attribList)["EventNumber"].data<unsigned int>() = 99;
  } 
  catch (pool::Exception e) 
  {
    log << MSG::ERROR
        << "Caught exception from data() when setting AOD global "
        << "attributes; Message: " << e.what()
        << endreq;
    return (StatusCode::FAILURE);
  }
  log << MSG::DEBUG << "Finished adding Run,Event data to AthenaAttributeList."
      << endreq;

  if ( !detStore()->contains<AthenaAttributeList>("RunEventTag") ) {
     StatusCode sc = detStore()->record(attribList, "RunEventTag");
     log << MSG::DEBUG << detStore()->dump() << endreq;
     if (sc.isFailure())
     {
        log << MSG::ERROR << "Could not record AthenaAttributeList object in DetectorStore."
            << endreq;
        return (StatusCode::FAILURE);
     }
  }

  log << MSG::DEBUG << "Printing out attribute list:" << endreq;
  std::ostringstream attribListStream;
  attribList->toOutputStream(attribListStream);
  log << MSG::DEBUG << "Attribute List: " 
                    << attribListStream.str() 
                    << endreq;

  return (StatusCode::SUCCESS);
}


StatusCode RunEventMetaWriter::finalize() 
{
  MsgStream log(messageService(), name());

  log << MSG::DEBUG << "Finalizing " << name() << endreq;

  return (StatusCode::SUCCESS);
}
