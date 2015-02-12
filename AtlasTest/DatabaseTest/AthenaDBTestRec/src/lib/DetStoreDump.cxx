/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DetStoreDump.cxx
// Athena algorithm to dump detector store status at end of init 
// and at first event execute
// started Richard Hawkings 24/1/06

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "AthenaDBTestRec/DetStoreDump.h"

DetStoreDump::DetStoreDump(const std::string& name, 
  ISvcLocator* pSvcLocator) :
   AthAlgorithm(name,pSvcLocator),
   m_first(true)
{
  declareProperty("Mode",m_mode); 
}

DetStoreDump::~DetStoreDump() {}

StatusCode DetStoreDump::initialize() {

  ATH_MSG_INFO("Initialise - mode " << m_mode);

  msg(MSG::INFO) << "Dump of complete detector store at initialize"
	<< detStore()->dump() << endreq;
  msg(MSG::INFO) << "Dump finished" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode DetStoreDump::execute() {
  if (m_first && m_mode>0) {
    msg(MSG::INFO) << 
     "Dump of complete detector store at first event execute"
	  << detStore()->dump() << endreq;
    msg(MSG::INFO) << "Dump finished" << endreq;
  }
  m_first=false;
  return StatusCode::SUCCESS;
}

StatusCode DetStoreDump::finalize() {
  return StatusCode::SUCCESS;
}
