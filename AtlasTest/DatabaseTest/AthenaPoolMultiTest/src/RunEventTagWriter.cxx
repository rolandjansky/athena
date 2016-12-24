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
//#include "EventInfo/EventInfo.h"
//#include "xAODEventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
//#include "AthenaPoolUtilities/AthenaAttributeList.h"
//#include "AthenaPoolUtilities/AthenaAttributeListSpecification.h"
#include "CoralBase/AttributeListSpecification.h"
//#include "AthenaPoolUtilities/MinimalEventInfoAttributeList.h"

#include "POOLCore/Exception.h"
#include "CxxUtils/make_unique.h"

#include <cassert>
#include <stdlib.h>


RunEventTagWriter::RunEventTagWriter(const std::string& name, 
                                     ISvcLocator* pSvcLocator) 
    : AthAlgorithm(name, pSvcLocator),
      m_evt("McEventInfo"),
      m_evtx("McEventInfo"),
      m_attribList("RunEventTag"),
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
  std::string attribName;

  ATH_CHECK( m_evt.initialize() );
  ATH_CHECK( m_evtx.initialize() );
  ATH_CHECK( m_attribList.initialize() );

  ATH_MSG_DEBUG( "Initializing " << name()  );

  ATH_MSG_DEBUG( "Defining the attribute list specification."  );
  //m_attribListSpec = new AthenaAttributeListSpecification();
  m_attribListSpec = new coral::AttributeListSpecification();

  m_attribListSpec->extend("RunNumber", "unsigned int");
  m_attribListSpec->extend("EventNumber", "unsigned int");

  ATH_MSG_DEBUG( "Printing out attribute list specification:"  );
  ATH_MSG_DEBUG( "Attribute List Specification: "  );
  //AthenaAttributeListSpecification::const_iterator first = m_attribListSpec->begin();
  //AthenaAttributeListSpecification::const_iterator last  = m_attribListSpec->end();
  coral::AttributeListSpecification::const_iterator first = m_attribListSpec->begin();
  coral::AttributeListSpecification::const_iterator last  = m_attribListSpec->end();
  for (; first != last; ++first) 
  {
    ATH_MSG_DEBUG( " name " << (*first).name() 
                   << " type " << (*first).typeName() );
  }

  return (StatusCode::SUCCESS);
}


StatusCode RunEventTagWriter::execute() 
{
  std::string attribName;

  ATH_MSG_DEBUG( "Executing " << name()  );

  AthenaAttributeList* attribList = 0;
  ATH_MSG_DEBUG( "Creating AthenaAttributeList object."  );
  try 
  {
    attribList = new AthenaAttributeList(*m_attribListSpec);
  } 
  catch (pool::Exception e) 
  {
    ATH_MSG_ERROR( "Caught exception during creation of AthenaAttributeList object."
                   << "Message: " << e.what()  );
    return (StatusCode::FAILURE);
  }

  ATH_MSG_DEBUG( "Retrieving event info from TDS."  );
  SG::ReadHandle<xAOD::EventInfo> evtx (m_evtx);
  SG::ReadHandle<EventInfo> evt (m_evt);
  if (!evtx.isValid()) 
  {
    if (!evt.isValid()) {
      ATH_MSG_ERROR( "Could not get event info from TDS."  );
    }
    else {
      if(fillTag(evt.cptr(),attribList).isFailure()) {
        ATH_MSG_ERROR( "Could not build tag from old event info."  );
      }
    }
  }
  else {
    if(fillTag(evtx.cptr(),attribList).isFailure()) {
      ATH_MSG_ERROR( "Could not build tag from xaod event info."  );
    }
  }

  //ATH_CHECK( evtStore()->record(attribList, "RunEventTag") );
  SG::WriteHandle<AthenaAttributeList> attwrite(m_attribList);
  ATH_CHECK( attwrite.record (std::make_unique<AthenaAttributeList>(*attribList)) );

  ATH_MSG_DEBUG( "Printing out attribute list:"  );
  std::ostringstream attribListStream;
  attribList->toOutputStream(attribListStream);
  ATH_MSG_DEBUG( "Attribute List: " << attribListStream.str() );

  return (StatusCode::SUCCESS);
}

StatusCode RunEventTagWriter::fillTag(const EventInfo* eInfo, AthenaAttributeList* attribList)
{
  unsigned int runNumber = eInfo->event_ID()->run_number();
  unsigned int eventNumber = eInfo->event_ID()->event_number();

  if (!attribList)
  {
    ATH_MSG_ERROR( "Attribute list object is NULL."  );
    return (StatusCode::FAILURE);
  }

  ATH_MSG_DEBUG( "About to assign values to Tag Attrib List"  );
  try
  {
    (*attribList)["RunNumber"].data<unsigned int>() = runNumber;
    (*attribList)["EventNumber"].data<unsigned int>() = eventNumber;
  } 
  catch (pool::Exception e) 
  {
    ATH_MSG_ERROR( "Caught exception from data() when setting AOD global "
                   << "attributes; Message: " << e.what() );
    return (StatusCode::FAILURE);
  }
  ATH_MSG_DEBUG( "Finished adding Run,Event data to AthenaAttributeList." );

  return StatusCode::SUCCESS;

}

StatusCode RunEventTagWriter::fillTag(const xAOD::EventInfo* eventInfo, AthenaAttributeList* attribList)
{
  unsigned int runNumber = eventInfo->runNumber();
  unsigned int eventNumber = eventInfo->eventNumber();

  if (!attribList)
  {
    ATH_MSG_ERROR( "Attribute list object is NULL."  );
    return (StatusCode::FAILURE);
  }

  ATH_MSG_DEBUG( "About to assign values to Tag Attrib List"  );
  try
  {
    (*attribList)["RunNumber"].data<unsigned int>() = runNumber;
    (*attribList)["EventNumber"].data<unsigned int>() = eventNumber;
  } 
  catch (pool::Exception e) 
  {
    ATH_MSG_ERROR( "Caught exception from data() when setting AOD global "
                   << "attributes; Message: " << e.what() );
    return (StatusCode::FAILURE);
  }
  ATH_MSG_DEBUG( "Finished adding Run,Event data to AthenaAttributeList." );

  return StatusCode::SUCCESS;

}


StatusCode RunEventTagWriter::finalize() 
{
  ATH_MSG_DEBUG( "Finalizing " << name()  );
  return (StatusCode::SUCCESS);
}
