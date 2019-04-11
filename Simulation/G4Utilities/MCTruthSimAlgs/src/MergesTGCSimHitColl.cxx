/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MergesTGCSimHitColl.h"

#include "PileUpTools/IPileUpTool.h"

MergesTGCSimHitColl::MergesTGCSimHitColl(const std::string& name,
			       ISvcLocator* svcLoc)
  : AthAlgorithm(name, svcLoc),
    m_mergeTool("MergesTGCSimHitCollTool", this)
{
  declareProperty("MergesTGCSimHitCollTool", m_mergeTool);
}
StatusCode MergesTGCSimHitColl::initialize() {
  ATH_MSG_DEBUG ( "Initializing " << name() << " - package version " << PACKAGE_VERSION ); 
  if(m_mergeTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve MergesTGCSimHitCollTool!");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Retrieved MergesTGCSimHitCollTool (" << m_mergeTool->name() << ").");
 
 return StatusCode::SUCCESS;
}
StatusCode MergesTGCSimHitColl::execute() {
  ATH_MSG_DEBUG("execute()");  
  return m_mergeTool->processAllSubEvents();
}
StatusCode MergesTGCSimHitColl::finalize() {
  ATH_MSG_DEBUG("finalize.");
  return StatusCode::SUCCESS;
}
