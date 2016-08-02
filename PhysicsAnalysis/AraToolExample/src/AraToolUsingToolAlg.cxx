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
  log << MSG::INFO << "initialize()" << endmsg;
  // verify that our tool handle is pointing to an accessible tool

  if ( m_tool.retrieve().isFailure() ) {
    log << MSG::FATAL << "Failed to retrieve " << m_tool << endmsg;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "Retrieved " << m_tool << endmsg;
  }

  if ( m_toolusingtool.retrieve().isFailure() ) {
    log << MSG::FATAL << "Failed to retrieve " << m_toolusingtool << endmsg;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "Retrieved " << m_toolusingtool << endmsg;
  }

   
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode AraToolUsingToolAlg::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "execute()" << endmsg;

  static float a=1,b=2;


  log<<MSG::INFO<<"Input to m_tool::PerformTask:"<<" "<<a<<" "<<b<<" "<<endmsg;
  m_tool->getTool()->performTask(a,b);
  log<<MSG::INFO<<"Output of m_tool::PerformTask and input of m_toolusingtool::alsoPerformTask"<<" "<<a<<" "<<b<<" "<<endmsg;
  m_toolusingtool->getTool()->alsoPerformTask(a,b);
  log<<MSG::INFO<<"Output of m_toolusingtool::alsoPerformTask:"<<" "<<a<<" "<<b<<" "<<endmsg;

  a=b;

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode AraToolUsingToolAlg::finalize() {

  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "finalize()" << endmsg;
  m_tool.release();
  m_toolusingtool.release();

  return StatusCode::SUCCESS;
}
 
