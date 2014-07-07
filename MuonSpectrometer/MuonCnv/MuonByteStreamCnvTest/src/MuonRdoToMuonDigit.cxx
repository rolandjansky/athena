/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"

#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"

#include "MuonByteStreamCnvTest/MuonRdoToMuonDigit.h"

MuonRdoToMuonDigit::MuonRdoToMuonDigit(const std::string& name, 
				 ISvcLocator* pSvcLocator)
  : Algorithm(name, pSvcLocator),
    m_digTool("MuonRdoToMuonDigitTool", this )
{
   declareProperty("MuonRdoToMuonDigitTool", m_digTool);
}

MuonRdoToMuonDigit::~MuonRdoToMuonDigit()  {

}

StatusCode MuonRdoToMuonDigit::initialize() {

  m_log = new MsgStream(msgSvc(),name());
  m_debug = m_log->level() <= MSG::DEBUG;
  m_verbose = m_log->level() <= MSG::VERBOSE;

// intitialize store gate active store
  if (m_digTool.retrieve().isFailure()) {
    *m_log << MSG::FATAL << "Could not retrieve MuonRdoToMuonDigit Tool!" 
	   << endreq;
    return StatusCode::FAILURE;
  }
  if ( m_debug ) *m_log << MSG::DEBUG << "Retrieved MuonRdoToMuonDigit Tool." << endreq;

  return StatusCode::SUCCESS;
    
}

StatusCode MuonRdoToMuonDigit::execute() {

  if ( m_debug ) *m_log << MSG::DEBUG << "in execute()" << endreq;

  return m_digTool->digitize();
}

 StatusCode MuonRdoToMuonDigit::finalize() {
    
   if ( m_debug ) *m_log << MSG::DEBUG << "finalize." << endreq;
   delete m_log;

   return StatusCode::SUCCESS;
}


