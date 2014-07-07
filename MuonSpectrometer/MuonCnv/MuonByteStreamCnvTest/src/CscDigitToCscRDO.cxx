/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Author: Ketevi A. Assamagan

#include "GaudiKernel/MsgStream.h"

#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"

#include "MuonByteStreamCnvTest/CscDigitToCscRDO.h"

CscDigitToCscRDO::CscDigitToCscRDO(const std::string& name, 
				 ISvcLocator* pSvcLocator)
  : Algorithm(name, pSvcLocator),
    m_digTool("CscDigitToCscRDOTool", this )
{
   declareProperty("CscDigitToRDOTool", m_digTool);
}

CscDigitToCscRDO::~CscDigitToCscRDO()  {

}

StatusCode CscDigitToCscRDO::initialize() {

  m_log = new MsgStream(msgSvc(),name());
  m_debug = m_log->level() <= MSG::DEBUG;
  m_verbose = m_log->level() <= MSG::VERBOSE;

// intitialize store gate active store
  if (m_digTool.retrieve().isFailure()) {
    *m_log << MSG::FATAL << "Could not retrieve CSC Digitization Tool!" 
	   << endreq;
    return StatusCode::FAILURE;
  }
  if ( m_debug ) *m_log << MSG::DEBUG << "Retrieved CSC Digit -> RDO Tool." << endreq;

  return StatusCode::SUCCESS;
    
}

StatusCode CscDigitToCscRDO::execute() {

  if ( m_debug ) *m_log << MSG::DEBUG << "in execute()" << endreq;

  return m_digTool->digitize();
}

StatusCode CscDigitToCscRDO::finalize() {
    
   if ( m_debug ) *m_log << MSG::DEBUG << "finalize." << endreq;
   delete m_log;

   return StatusCode::SUCCESS;
}



