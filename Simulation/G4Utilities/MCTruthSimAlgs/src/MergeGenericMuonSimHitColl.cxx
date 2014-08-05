/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeGenericMuonSimHitColl.h"

#include "PileUpTools/IPileUpTool.h"

MergeGenericMuonSimHitColl::MergeGenericMuonSimHitColl(const std::string& name,
			       ISvcLocator* svcLoc)
  : AthAlgorithm(name, svcLoc),
    m_mergeTool("MergeGenericMuonSimHitCollTool", this)
{
  declareProperty("MergeGenericMuonSimHitCollTool", m_mergeTool);
}
StatusCode MergeGenericMuonSimHitColl::initialize() {
  ATH_MSG_DEBUG ( "Initializing " << name() << " - package version " << PACKAGE_VERSION ); 
  if(m_mergeTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve MergeGenericMuonSimHitCollTool!");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Retrieved MergeGenericMuonSimHitCollTool (" << m_mergeTool->name() << ").");
 
 return StatusCode::SUCCESS;
}
StatusCode MergeGenericMuonSimHitColl::execute() {
  ATH_MSG_DEBUG("execute()");  
  return m_mergeTool->processAllSubEvents();
}
StatusCode MergeGenericMuonSimHitColl::finalize() {
  ATH_MSG_DEBUG("finalize.");
  return StatusCode::SUCCESS;
}
