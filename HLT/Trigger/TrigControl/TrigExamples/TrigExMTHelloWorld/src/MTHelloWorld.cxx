/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigExMTHelloWorld/MTHelloWorld.h"

MTHelloWorld::MTHelloWorld(const std::string& name, ISvcLocator* pSvcLocator) :
   AthAlgorithm(name, pSvcLocator),
   m_myInt(0), m_myBool(0), m_myDouble(0)
{
  
  // Part 2: Declare the properties
  declareProperty("MyInt", m_myInt);
  declareProperty("MyBool", m_myBool);
  declareProperty("MyDouble", m_myDouble);

  declareProperty("MyStringVec",m_myStringVec);

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTHelloWorld::initialize()
{
   ATH_MSG_INFO("initialize()");


#ifdef ATLAS_GAUDI_V21
  SmartIF<IService> tmp_msgSvc(msgSvc()); 
  if(tmp_msgSvc.isValid()) {
     ATH_MSG_INFO(" Algorithm = " << name() << " is connected to Message Service = " << tmp_msgSvc->name());
  }
#else
  Service* tmp_msgSvc = dynamic_cast<Service*> (msgSvc()); 
  if(tmp_msgSvc != 0) {
    ATH_MSG_INFO(" Algorithm = " << name() << " is connected to Message Service = "
                 << tmp_msgSvc->name());
  }
#endif

  // Part 2: Print out the property values
  ATH_MSG_INFO("  MyInt =    " << m_myInt);
  ATH_MSG_INFO("  MyBool =   " << m_myBool);
  ATH_MSG_INFO("  MyDouble = " << m_myDouble);
  
  for (unsigned int i=0; i<m_myStringVec.value().size(); i++) {
     ATH_MSG_INFO("  MyStringVec[" << i << "] = " << (m_myStringVec.value())[i]);
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTHelloWorld::execute()
{
  
   ATH_MSG_INFO("execute()");

   // Part 1: Print out the different levels of messages
   ATH_MSG_DEBUG( "A DEBUG message");
   ATH_MSG_INFO( "An INFO message");
   ATH_MSG_WARNING( "A WARNING message");
   ATH_MSG_ERROR( "An ERROR message");
   ATH_MSG_FATAL( "A FATAL error message");
   
   return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTHelloWorld::finalize()
{
   ATH_MSG_INFO( "finalize()");
   return StatusCode::SUCCESS;
}

StatusCode MTHelloWorld::beginRun()
{
   ATH_MSG_INFO( "beginRun()");
   return StatusCode::SUCCESS;
}

StatusCode MTHelloWorld::endRun()
{
   ATH_MSG_INFO( "endRun()");
   return StatusCode::SUCCESS;
}

StatusCode MTHelloWorld::start()
{
   ATH_MSG_INFO( "start()");
   return StatusCode::SUCCESS;
}

StatusCode MTHelloWorld::stop()
{
   ATH_MSG_INFO( "stop()");
   return StatusCode::SUCCESS;
}
