/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeTruthJets.h"

#include "PileUpTools/IPileUpTool.h"

MergeTruthJets::MergeTruthJets(const std::string& name,
			       ISvcLocator* svcLoc)
  : AthAlgorithm(name, svcLoc),
    m_mergeTool("MergeTruthJetsTool", this)
{
  declareProperty("MergeTruthJetsTool", m_mergeTool);
}
StatusCode MergeTruthJets::initialize() {
  ATH_MSG_DEBUG ( "Initializing " << name() << " - package version " << PACKAGE_VERSION ); 
  if(m_mergeTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve MergeTruthJetsTool!");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Retrieved MergeTruthJetsTool (" << m_mergeTool->name() << ").");
 
 return StatusCode::SUCCESS;
}
StatusCode MergeTruthJets::execute() {
  ATH_MSG_DEBUG("execute()");  
  return m_mergeTool->processAllSubEvents();
}
StatusCode MergeTruthJets::finalize() {
  ATH_MSG_DEBUG("finalize.");
  return StatusCode::SUCCESS;
}
