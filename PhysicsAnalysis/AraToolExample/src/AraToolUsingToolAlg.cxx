/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AraToolUsingToolAlg.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

/////////////////////////////////////////////////////////////////////////////

AraToolUsingToolAlg::AraToolUsingToolAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), m_tool( "AnExampleAraToolWrapper", this ), m_toolusingtool( "AraToolUsingToolWrapper", this )
{
  declareProperty( "TheTool",       m_tool,       "a Tool" );
  declareProperty( "TheToolUsingTool",       m_toolusingtool,       "A Tool Using another Tool" );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode AraToolUsingToolAlg::initialize() {

  // Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initialize()" << endreq;
  // verify that our tool handle is pointing to an accessible tool

  if ( m_tool.retrieve().isFailure() ) {
    log << MSG::FATAL << "Failed to retrieve " << m_tool << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "Retrieved " << m_tool << endreq;
  }

  if ( m_toolusingtool.retrieve().isFailure() ) {
    log << MSG::FATAL << "Failed to retrieve " << m_toolusingtool << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "Retrieved " << m_toolusingtool << endreq;
  }

   
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode AraToolUsingToolAlg::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "execute()" << endreq;

  static float a=1,b=2;


  log<<MSG::INFO<<"Input to m_tool::PerformTask:"<<" "<<a<<" "<<b<<" "<<endreq;
  m_tool->getTool()->performTask(a,b);
  log<<MSG::INFO<<"Output of m_tool::PerformTask and input of m_toolusingtool::alsoPerformTask"<<" "<<a<<" "<<b<<" "<<endreq;
  m_toolusingtool->getTool()->alsoPerformTask(a,b);
  log<<MSG::INFO<<"Output of m_toolusingtool::alsoPerformTask:"<<" "<<a<<" "<<b<<" "<<endreq;

  a=b;

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode AraToolUsingToolAlg::finalize() {

  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "finalize()" << endreq;
  m_tool.release();
  m_toolusingtool.release();

  return StatusCode::SUCCESS;
}
 
