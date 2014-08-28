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
   Algorithm(name,pSvcLocator),  m_log (msgSvc(),name), 
   p_detstore(0), m_first(true)
{
  declareProperty("Mode",m_mode); 
}

DetStoreDump::~DetStoreDump() {}

StatusCode DetStoreDump::initialize() {

  if (StatusCode::SUCCESS!=service("DetectorStore",p_detstore)) {
    m_log << MSG::FATAL << "Detector store not found" << endreq;
    return StatusCode::FAILURE;
  }
  m_log << MSG::INFO << "Initialise - mode " << m_mode << endreq;

  m_log << MSG::INFO << "Dump of complete detector store at initialize"
	<< p_detstore->dump() << endreq;
  m_log << MSG::INFO << "Dump finished" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode DetStoreDump::execute() {
  if (m_first && m_mode>0) {
    m_log << MSG::INFO << 
     "Dump of complete detector store at first event execute"
	  << p_detstore->dump() << endreq;
    m_log << MSG::INFO << "Dump finished" << endreq;
  }
  m_first=false;
  return StatusCode::SUCCESS;
}

StatusCode DetStoreDump::finalize() {
  return StatusCode::SUCCESS;
}
