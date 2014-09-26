/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "TrigNavigationSlimming.h"

using namespace HLT;

HLT::TrigNavigationSlimming::TrigNavigationSlimming(const std::string& name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_tool("HLT::TrigNavigationThinningTool/TrigNavigationThinningTool") {
  declareProperty("ThinningTool", m_tool, "Tool instance which forwards the slimming request");
}

StatusCode HLT::TrigNavigationSlimming::initialize() {
  CHECK( m_tool.retrieve() ); 
  // here we shoudl check if the tool is well configured
  return StatusCode::SUCCESS;
}


StatusCode HLT::TrigNavigationSlimming::execute() {
  CHECK( m_tool->doThinning() );
  return StatusCode::SUCCESS;
}

StatusCode HLT::TrigNavigationSlimming::finalize() {
  return StatusCode::SUCCESS;
}
