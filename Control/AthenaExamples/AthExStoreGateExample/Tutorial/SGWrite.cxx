/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SGWrite.h"

#include "MyDataObj.h"
#include "LinkObj.h"
#include "Tutorial_ClassDEF.h"

#include "GaudiKernel/ISvcLocator.h"

#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

#include "StoreGate/StoreGateSvc.h"

using namespace SGTutorial;

/////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:

SGWrite::SGWrite(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), m_DataObjKey("foo")
{
  
  // Declare the properties
  // Get key to be given to Data Objects in TDS

  declareProperty("DataObjKey", m_DataObjKey);

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SGWrite::initialize()
{

  ATH_MSG_INFO ("in initialize()");

  // Print out the key of the data objects

  ATH_MSG_INFO ("MyDataObj key: " << m_DataObjKey);

  StatusCode sc = evtStore().retrieve();
  if (sc.isFailure())
  {
    ATH_MSG_ERROR ("Unable to retrieve pointer to StoreGate Service");
    return sc;
  }

  return StatusCode::SUCCESS;


}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SGWrite::execute() {

  ATH_MSG_INFO ("in execute()");

  /////////////////////////////////////////////////////////////////////
  // PART1  Objective: 
  // Create a Data Object "MyDataObj" and record it in the TDS
  /////////////////////////////////////////////////////////////////////

  // Register "MyDataObj" 

  // set it as constant so that nobody can modify it anymore.
  // This will also allow the object to be persistified later on
  

  /////////////////////////////////////////////////////////////////////
  // PART2  Objective: 
  // Create a collection of objects : "MyElement".  Note that "MyElement"
  // does not inherits from ContainedObject.  Push MyElement into a
  // collection:  std::vector<MyElement>.
  // Register the std::vector<MyElement> into the StoreGate:
  /////////////////////////////////////////////////////////////////////

 

  // Register vector of "MyElement" 

  // set it as constant so that nobody can modify it anymore.
  // This will also allow the object to be persistified later on

  /////////////////////////////////////////////////////////////////////
  // PART3  Objective: 
  // Set up DataLinks .  Each instance of LinkObj contains a link
  // to a MyDataObj and one to a MyElement; 
  // Create data links for both and set them in a LinkObj object 
  // using the setObjLink and setContLink methods.
  // We will use the DataLinks in LinkObj later in SGRead to ensure that it
  // does return the correct objects they point to.
  /////////////////////////////////////////////////////////////////////

  //Instantiate a LinkObj

  //set the DataLink to a MyDataObj

  //set the ElementLink to the first element of the collection

  //Register the LinkObj in TDS:


  // set it as constant so that nobody can modify it anymore
  // This will also allow the object to be persistified later on

  /////////////////////////////////////////////////////////////////////
  // We are DONE!
  /////////////////////////////////////////////////////////////////////

  ATH_MSG_INFO ("registered all data");
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SGWrite::finalize() 
{
  ATH_MSG_INFO ("in finalize()");
  return StatusCode::SUCCESS;
}


