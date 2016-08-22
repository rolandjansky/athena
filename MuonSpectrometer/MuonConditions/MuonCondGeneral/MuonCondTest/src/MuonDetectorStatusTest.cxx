/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondTest/MuonDetectorStatusTest.h"
#include "MuonCondSvc/MuonDetectorStatusDbSvc.h"


MuonDetectorStatusTest::MuonDetectorStatusTest(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_muonDetectorStatusDbSvc(NULL),
  m_log(NULL),
  m_chronoSvc(NULL)
{ 
//  m_muonDetectorStatusDbSvc=0; 
}

MuonDetectorStatusTest::~MuonDetectorStatusTest() { }


StatusCode MuonDetectorStatusTest::initialize()
{

  // initialize the chrono service
  StatusCode sc = service("ChronoStatSvc",m_chronoSvc);
  if (sc != StatusCode::SUCCESS) {
    *m_log << MSG::ERROR << "Could not find the ChronoSvc" << endmsg;
    return sc;
  }

  // initialize the  service
  sc = service("MuonDetectorStatusDbSvc",m_muonDetectorStatusDbSvc);
  if (sc != StatusCode::SUCCESS) {
    *m_log << MSG::ERROR << "Could not find the muonDetectorStatusDbSvc" << endmsg;
    return sc;
  }



  m_chrono1 = "first test";

  return StatusCode::SUCCESS;
}


StatusCode MuonDetectorStatusTest::execute()
{
  

  return StatusCode::SUCCESS;
}


StatusCode MuonDetectorStatusTest::finalize()
{
  return StatusCode::SUCCESS;
}

