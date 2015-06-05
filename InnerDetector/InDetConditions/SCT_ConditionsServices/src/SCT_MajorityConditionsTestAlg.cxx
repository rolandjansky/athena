/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_MajorityConditionsTestAlg.cxx
 *
 * @brief 
 * 
 *
 * @author gwilliam@mail.cern.ch
 **/

#include "SCT_MajorityConditionsTestAlg.h"

//Gaudi includes
#include "GaudiKernel/StatusCode.h"

//Athena includes
#include "Identifier/Identifier.h"

//local includes
#include "SCT_MajorityConditionsSvc.h"

SCT_MajorityConditionsTestAlg::SCT_MajorityConditionsTestAlg(const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ),
  m_majoritySvc("SCT_MajorityConditionsSvc", name)
{
  declareProperty("MajoritySvc",      m_majoritySvc);
}

SCT_MajorityConditionsTestAlg::~SCT_MajorityConditionsTestAlg()
{ 
  msg(MSG::INFO) << "Calling destructor" << endreq;
}

//Initialize
StatusCode SCT_MajorityConditionsTestAlg::initialize(){
  StatusCode sc(StatusCode::SUCCESS);
  msg(MSG::INFO)<< "Calling initialize" << endreq;
  
  // Retrieve link masking service
  if (m_majoritySvc.retrieve().isFailure()) {
    msg(MSG::ERROR)<<"Could not retrieve the link masking service"<<endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//Execute
StatusCode SCT_MajorityConditionsTestAlg::execute(){
  msg(MSG::INFO)<< "Calling execute" << endreq;

  msg(MSG::INFO)<< "Detector is " << (m_majoritySvc->isGood()   ? "GOOD" : "BAD") << endreq;
  msg(MSG::INFO)<< "ECC is      " << (m_majoritySvc->isGood(-2) ? "GOOD" : "BAD") << endreq;
  msg(MSG::INFO)<< "Barrel is   " << (m_majoritySvc->isGood(0)  ? "GOOD" : "BAD") << endreq;
  msg(MSG::INFO)<< "ECA is      " << (m_majoritySvc->isGood(2)  ? "GOOD" : "BAD") << endreq;

  return StatusCode::SUCCESS;
}


//Finalize
StatusCode SCT_MajorityConditionsTestAlg::finalize(){
  StatusCode sc(StatusCode::SUCCESS);
  msg(MSG::INFO)<< "Calling finalize" << endreq;
  return sc;
}
