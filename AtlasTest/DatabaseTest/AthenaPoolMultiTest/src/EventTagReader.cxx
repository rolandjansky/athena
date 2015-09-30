/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file EventTagReader.cxx
 * @brief Implementation of class EventTagReader
 * @author K. Karr <Kristo.Karr@cern.ch>
 * $Id: EventTagReader.cxx,v 1.3 2009-02-19 22:04:49 cranshaw Exp $ 
 */

#include "EventTagReader.h"

#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ISvcLocator.h"

#include "CoralBase/AttributeListSpecification.h"
#include "CoralBase/Attribute.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include <fstream>


EventTagReader::EventTagReader(const std::string& name,
                               ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_storeGateSvc(0)
{
}


EventTagReader::~EventTagReader()
{}


StatusCode EventTagReader::initialize()
{
  StatusCode sc = StatusCode::SUCCESS;

  MsgStream log(messageService(), name());

  log << MSG::DEBUG << "Initializing " << name() << endreq;

  sc = service("StoreGateSvc", m_storeGateSvc);
  if (sc.isFailure())
  {
    log << MSG::ERROR << "Cannot get StoreGate service." << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;      
}


StatusCode EventTagReader::execute()
{ 
  MsgStream log(messageService(), name());   
  
  log << MSG::DEBUG << "Executing " << name() << endreq;

  const AthenaAttributeList* attribList;
  StatusCode sc = m_storeGateSvc->retrieve(attribList, "Input");
  //log << MSG::INFO << "BLARG " << m_storeGateSvc->dump() << endreq;

  if (sc.isSuccess()) {
  
     //const coral::AttributeList& attribList = cursor.currentRow().attributeList();
     log << MSG::DEBUG << "Attribute list is: " << endreq;
     for (coral::AttributeList::const_iterator first = attribList->begin(); 
                                               first != attribList->end(); 
                                             ++first)
     {
       std::ostringstream value;
       (*first).toOutputStream(value);
       log << MSG::DEBUG << "NAME " << (*first).specification().name() << " TYPE " 
           << (*first).specification().typeName() << " VALUE "
           << value.str() << endreq;
     }
  }

  return StatusCode::SUCCESS;
}


StatusCode EventTagReader::finalize() 
{
  MsgStream log(messageService(), name());

  log << MSG::DEBUG << "Finalizing " << name() << endreq;

  return StatusCode::SUCCESS;
}

