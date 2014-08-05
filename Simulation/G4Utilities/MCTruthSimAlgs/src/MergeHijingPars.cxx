/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeHijingPars.h"

#include "PileUpTools/IPileUpTool.h"

MergeHijingPars::MergeHijingPars(const std::string& name, ISvcLocator* svcLoc)
  : AthAlgorithm(name, svcLoc),
    m_mergeTool("MergeHijingParsTool", this)
{
  declareProperty("MergeHijingParsTool", m_mergeTool);
}

StatusCode MergeHijingPars::initialize() {
  ATH_MSG_DEBUG ( "Initializing " << name() << " - package version " << PACKAGE_VERSION ); 
  if(m_mergeTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve MergeHijingParsTool!");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Retrieved MergeHijingParsTool (" << m_mergeTool->name() << ").");

  return StatusCode::SUCCESS;
}

StatusCode MergeHijingPars::execute() {
  ATH_MSG_DEBUG("execute()");  
  return m_mergeTool->processAllSubEvents();
}

StatusCode MergeHijingPars::finalize() {
  ATH_MSG_DEBUG("finalize.");
  return StatusCode::SUCCESS;
}
