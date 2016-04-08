/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SGRead.h"

#include "MyDataObj.h"
#include "LinkObj.h"
#include "Tutorial_ClassDEF.h"

#include "GaudiKernel/ISvcLocator.h"

#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

#include "StoreGate/StoreGateSvc.h"

using namespace SGTutorial;

/////////////////////////////////////////////////////////////////////////////

SGRead::SGRead(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
  
  // Declare the properties
  // Key of the Data Object in the TDS:
  declareProperty("DataObjKey", m_DataObjKey);

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SGRead::initialize()
{

  ATH_MSG_INFO ("in initialize()");

  // Print out the key of the data objects

  ATH_MSG_INFO ("Data Object key: " << m_DataObjKey);

  StatusCode sc = evtStore().retrieve();
  if (sc.isFailure())
    {
      ATH_MSG_ERROR ("Unable to retrieve pointer to StoreGate Service");
      return sc;
    }


  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SGRead::execute() 
{

  ATH_MSG_INFO ("in execute()");

  /////////////////////////////////////////////////////////////////////
  // PART1  Objective: 
  // Retrieve the Data Object "MyDataObj" from SG (which was
  // registered in SG by the SGWrite algorithm.
  // Print out the value set in the MyDataObj
  /////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////
  // PART2  Objective: 
  // Retrieve the Collection of MyElement objects from the SG which was
  // registered in SG by the SGWrite algorithm.
  // Iterate over the collection to gain access to MyElement.
  // Print out each MyElement.
  /////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////
  // PART3  Objective: 
  // Retrieve LinkObj and dump its content.
  // LinkObj contains a DataLink to MyDataObj and a DataLink to a MyElement
  // in a vector. It provides an accessor method myLinkedObj() that returns
  // a reference to the linked MyDataObj.
  // Another accessor myLinkedElement() returns a reference to the linked
  // MyElement.
  // Verify that the contents (time and id) match with the 
  // direct-dump of MyDataObj in PART1 and MyElement[0] in PART2.
  /////////////////////////////////////////////////////////////////////

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SGRead::finalize() 
{
  ATH_MSG_INFO ("in finalize()");
  return StatusCode::SUCCESS;
}




