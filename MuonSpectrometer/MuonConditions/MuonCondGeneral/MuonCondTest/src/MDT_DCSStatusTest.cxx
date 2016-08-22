/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondTest/MDT_DCSStatusTest.h"
#include "MuonCondSvc/MDT_DCSConditionsSvc.h"


MDT_DCSStatusTest::MDT_DCSStatusTest(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_mdt_DCSConditionsSvc(NULL),
  m_log(NULL),
  m_chronoSvc(NULL)
{ 
}

MDT_DCSStatusTest::~MDT_DCSStatusTest() { }


StatusCode MDT_DCSStatusTest::initialize()
{

  // initialize the chrono service
  StatusCode sc = service("ChronoStatSvc",m_chronoSvc);
  if (sc != StatusCode::SUCCESS) {
    *m_log << MSG::ERROR << "Could not find the ChronoSvc" << endmsg;
    return sc;
  }

  // initialize the  service
  sc = service("MDT_DCSConditionsSvc",m_mdt_DCSConditionsSvc);
  if (sc != StatusCode::SUCCESS) {
    *m_log << MSG::ERROR << "Could not find the MDT_DCSConditionsSvc "<< endmsg;
    return sc;
  }



  m_chrono1 = "first test";

  return StatusCode::SUCCESS;
}


StatusCode MDT_DCSStatusTest::execute()
{


  return StatusCode::SUCCESS;
}


StatusCode MDT_DCSStatusTest::finalize()
{
  return StatusCode::SUCCESS;
}

