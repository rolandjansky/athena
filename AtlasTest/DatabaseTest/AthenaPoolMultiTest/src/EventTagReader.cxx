/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file EventTagReader.cxx
 * @brief Implementation of class EventTagReader
 * @author K. Karr <Kristo.Karr@cern.ch>
 * $Id: EventTagReader.cxx,v 1.3 2009-02-19 22:04:49 cranshaw Exp $ 
 */

#include "EventTagReader.h"

#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/ISvcLocator.h"

#include "CoralBase/AttributeListSpecification.h"
#include "CoralBase/Attribute.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include <fstream>


EventTagReader::EventTagReader(const std::string& name,
                               ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
}


EventTagReader::~EventTagReader()
{}


StatusCode EventTagReader::initialize()
{
  ATH_MSG_DEBUG( "Initializing " << name()  );
  return StatusCode::SUCCESS;      
}


StatusCode EventTagReader::execute()
{ 
  ATH_MSG_DEBUG( "Executing " << name()  );

  const AthenaAttributeList* attribList;
  StatusCode sc = evtStore()->retrieve(attribList, "Input");
  //log << MSG::INFO << "BLARG " << m_storeGateSvc->dump() << endmsg;

  if (sc.isSuccess()) {
  
     //const coral::AttributeList& attribList = cursor.currentRow().attributeList();
     ATH_MSG_DEBUG( "Attribute list is: "  );
     for (coral::AttributeList::const_iterator first = attribList->begin(); 
                                               first != attribList->end(); 
                                             ++first)
     {
       std::ostringstream value;
       (*first).toOutputStream(value);
       ATH_MSG_DEBUG( "NAME " << (*first).specification().name() << " TYPE " 
                      << (*first).specification().typeName() << " VALUE "
                      << value.str()  );
     }
  }

  return StatusCode::SUCCESS;
}


StatusCode EventTagReader::finalize() 
{
  ATH_MSG_DEBUG( "Finalizing " << name()  );
  return StatusCode::SUCCESS;
}

