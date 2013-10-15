/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigExMTHelloWorld/MTHelloWorld.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Service.h"

// static const AlgFactory<MTHelloWorld>    Factory;
// const IAlgFactory& MTHelloWorldFactory = Factory;

/////////////////////////////////////////////////////////////////////////////

MTHelloWorld::MTHelloWorld(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  m_log(msgSvc(), name),
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
  m_log.setLevel(outputLevel());
  
  m_log << MSG::INFO << "initialize()" << endreq;


#ifdef ATLAS_GAUDI_V21
  SmartIF<IService> tmp_msgSvc(msgSvc()); 
  if(tmp_msgSvc.isValid()) {
    m_log << MSG::INFO << " Algorithm = " << name() << " is connected to Message Service = "
          << tmp_msgSvc->name() << endreq;
  }
#else
  Service* tmp_msgSvc = dynamic_cast<Service*> (msgSvc()); 
  if(tmp_msgSvc != 0) {
    m_log << MSG::INFO << " Algorithm = " << name() << " is connected to Message Service = "
          << tmp_msgSvc->name() << endreq;
  }
#endif

  // Part 2: Print out the property values
  m_log << MSG::INFO << "  MyInt =    " << m_myInt << endreq;
  m_log << MSG::INFO << "  MyBool =   " << m_myBool << endreq;
  m_log << MSG::INFO << "  MyDouble = " << m_myDouble << endreq;
  
  for (unsigned int i=0; i<m_myStringVec.value().size(); i++) {
    m_log << MSG::INFO << "  MyStringVec[" << i << "] = " << (m_myStringVec.value())[i] 
          << endreq;
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTHelloWorld::execute()
{
  
  m_log << MSG::INFO << "execute()" << endreq;

  // Part 1: Print out the different levels of messages
  m_log << MSG::DEBUG << "A DEBUG message" << endreq;
  m_log << MSG::INFO << "An INFO message" << endreq;
  m_log << MSG::WARNING << "A WARNING message" << endreq;
  m_log << MSG::ERROR << "An ERROR message" << endreq;
  m_log << MSG::FATAL << "A FATAL error message" << endreq;

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTHelloWorld::finalize()
{
  m_log << MSG::INFO << "finalize()" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode MTHelloWorld::beginRun()
{
  m_log << MSG::INFO << "beginRun()" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode MTHelloWorld::endRun()
{
  m_log << MSG::INFO << "endRun()" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode MTHelloWorld::start()
{
  m_log << MSG::INFO << "start()" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode MTHelloWorld::stop()
{
  m_log << MSG::INFO << "stop()" << endreq;
  return StatusCode::SUCCESS;
}
