/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RunEventMetaWriter.cxx
 * @brief Implementation of class RunEventMetaWriter
 * @author K. Karr (Kristo.Karr@cern.ch)
 * @author J. Cranshaw (Jack.Cranshaw@cern.ch)
 * $Id: RunEventMetaWriter.cxx,v 1.4 2009-02-19 22:04:49 cranshaw Exp $ 
 */

#include "RunEventMetaWriter.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/AttributeListSpecification.h"

#include <cassert>
#include <stdlib.h>
#include <exception>


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
  std::string attribName;

  ATH_MSG_DEBUG( "Initializing " << name()  );

  ATH_MSG_DEBUG( "Defining the attribute list specification."  );
  m_attribListSpec = new coral::AttributeListSpecification();

  m_attribListSpec->extend("RunNumber", "unsigned int");
  m_attribListSpec->extend("EventNumber", "unsigned int");

  ATH_MSG_DEBUG( "Printing out attribute list specification:"  );
  {
    ATH_MSG_DEBUG( "Attribute List Specification: "  );
    coral::AttributeListSpecification::const_iterator first = m_attribListSpec->begin();
    coral::AttributeListSpecification::const_iterator last  = m_attribListSpec->end();
    for (; first != last; ++first) 
    {
      ATH_MSG_DEBUG( " name " << (*first).name() 
                     << " type " << (*first).typeName() );
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

  ATH_MSG_DEBUG( "Executing " << name()  );

  AthenaAttributeList* attribList = 0;
  ATH_MSG_DEBUG( "Creating AthenaAttributeList object."  );
  try 
  {
    attribList = new AthenaAttributeList(*m_attribListSpec);
  } 
  catch (const std::exception& e) 
  {
    ATH_MSG_ERROR( "Caught exception during creation of AthenaAttributeList object."
                   << "Message: " << e.what()  );
    return (StatusCode::FAILURE);
  }

  if (!attribList)
  {
    ATH_MSG_ERROR( "Attribute list object is NULL."  );
    return (StatusCode::FAILURE);
  }

  ATH_MSG_DEBUG( "About to assign values to Tag Attrib List"  );
  try
  {
    (*attribList)["RunNumber"].data<unsigned int>() = 88;
    (*attribList)["EventNumber"].data<unsigned int>() = 99;
  } 
  catch (const std::exception& e) 
  {
    ATH_MSG_ERROR( "Caught exception from data() when setting AOD global "
                   << "attributes; Message: " << e.what() );
    return (StatusCode::FAILURE);
  }
  ATH_MSG_DEBUG( "Finished adding Run,Event data to AthenaAttributeList." );

  if ( !detStore()->contains<AthenaAttributeList>("RunEventTag") ) {
     StatusCode sc = detStore()->record(attribList, "RunEventTag");
     ATH_MSG_DEBUG( detStore()->dump()  );
     if (sc.isFailure())
     {
        ATH_MSG_ERROR( "Could not record AthenaAttributeList object in DetectorStore."
                      );
        return (StatusCode::FAILURE);
     }
  }

  ATH_MSG_DEBUG( "Printing out attribute list:"  );
  std::ostringstream attribListStream;
  attribList->toOutputStream(attribListStream);
  ATH_MSG_DEBUG( "Attribute List: " 
                 << attribListStream.str() );

  return (StatusCode::SUCCESS);
}


StatusCode RunEventMetaWriter::finalize() 
{
  ATH_MSG_DEBUG( "Finalizing " << name()  );
  return (StatusCode::SUCCESS);
}
