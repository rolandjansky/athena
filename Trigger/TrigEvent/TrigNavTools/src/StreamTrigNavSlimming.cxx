/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrigNavigationThinningTool.h"
#include "StreamTrigNavSlimming.h"

HLT::StreamTrigNavSlimming::StreamTrigNavSlimming(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_tool("HLT::TrigNavigationThinningTool/TrigNavigationThinningTool") {
  declareProperty("ThinningTool", m_tool, "Tool instance which forwards the slimming request");
}

HLT::StreamTrigNavSlimming::~StreamTrigNavSlimming() {
}

StatusCode HLT::StreamTrigNavSlimming::initialize() {  
  ATH_MSG_DEBUG( "StreamTrigNavSlimming::initialize()" );
  CHECK( m_tool.retrieve() ); 
  /*  check is the tool is configured ok, but sgetProperty of AlgTool does not work
  if ( bool(m_tool->getProperty("ActInPlace")) == true ) {
  ATH_MSG_DEBUG( "StreamTrigNavSlimming::initialize() the thinning tool configured to act in place" );
    StatusCode::FAILURE;
  }
  */
  return  StatusCode::SUCCESS;
}

StatusCode HLT::StreamTrigNavSlimming::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode HLT::StreamTrigNavSlimming::execute() {
  ATH_MSG_DEBUG( "StreamTrigNavSlimming::execute()" );
  CHECK(m_tool->doThinning());
  return StatusCode::SUCCESS;
}

