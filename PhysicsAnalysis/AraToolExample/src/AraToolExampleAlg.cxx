/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AraToolExampleAlg.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

/////////////////////////////////////////////////////////////////////////////

AraToolExampleAlg::AraToolExampleAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), m_tool( "AnExampleAraToolWrapper", this )
{
  declareProperty( "TheTool",       m_tool,       "helper to offload some tasks to" );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode AraToolExampleAlg::initialize() {

  // Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initialize()" << endmsg;
  // verify that our tool handle is pointing to an accessible tool

  if ( m_tool.retrieve().isFailure() ) {
    log << MSG::FATAL << "Failed to retrieve " << m_tool << endmsg;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "Retrieved " << m_tool << endmsg;
  }

   
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode AraToolExampleAlg::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "execute()" << endmsg;

  static float a=1,b=2;

  log<<MSG::INFO<<"M_TOOL:"<<m_tool<<" "
     <<endmsg;

  log<<MSG::INFO<<"PerformTask:"<<" "<<a<<" "<<b<<" "<<endmsg;
  m_tool->getTool()->performTask(a,b);
  log<<MSG::INFO<<"PerformTask:"<<" "<<a<<" "<<b<<" "<<endmsg;

  a=b;

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode AraToolExampleAlg::finalize() {

  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "finalize()" << endmsg;
  m_tool.release();

  return StatusCode::SUCCESS;
}
 
