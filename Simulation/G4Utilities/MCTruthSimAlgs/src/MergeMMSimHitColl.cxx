/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeMMSimHitColl.h"

#include "PileUpTools/IPileUpTool.h"

MergeMMSimHitColl::MergeMMSimHitColl(const std::string& name,
			       ISvcLocator* svcLoc)
  : AthAlgorithm(name, svcLoc),
    m_mergeTool("MergeMMSimHitCollTool", this)
{
  declareProperty("MergeMMSimHitCollTool", m_mergeTool);
}

MergeMMSimHitColl::~MergeMMSimHitColl() {
}

StatusCode MergeMMSimHitColl::initialize() {
  ATH_MSG_DEBUG ( "Initializing " << name() << " - package version " << PACKAGE_VERSION ); 
  if(m_mergeTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve MergeMMSimHitCollTool!");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Retrieved MergeMMSimHitCollTool (" << m_mergeTool->name() << ").");
 
 return StatusCode::SUCCESS;
}
StatusCode MergeMMSimHitColl::execute() {
  ATH_MSG_DEBUG("execute()");  
  return m_mergeTool->processAllSubEvents();
}
StatusCode MergeMMSimHitColl::finalize() {
  ATH_MSG_DEBUG("finalize.");
  return StatusCode::SUCCESS;
}
