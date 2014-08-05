/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeTrackRecordCollection.h"

#include "PileUpTools/IPileUpTool.h"

MergeTrackRecordCollection::MergeTrackRecordCollection(const std::string& name,
			       ISvcLocator* svcLoc)
  : AthAlgorithm(name, svcLoc),
    m_mergeTool("MergeTrackRecordCollTool", this)
{
  declareProperty("MergeTrackRecordCollTool", m_mergeTool);
}
StatusCode MergeTrackRecordCollection::initialize() {
  ATH_MSG_DEBUG ( "Initializing " << name() << " - package version " << PACKAGE_VERSION ); 
  if(m_mergeTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve MergeTrackRecordCollTool!");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Retrieved MergeTrackRecordCollTool (" << m_mergeTool->name() << ").");
 
 return StatusCode::SUCCESS;
}
StatusCode MergeTrackRecordCollection::execute() {
  ATH_MSG_DEBUG("execute()");  
  return m_mergeTool->processAllSubEvents();
}
StatusCode MergeTrackRecordCollection::finalize() {
  ATH_MSG_DEBUG("finalize.");
  return StatusCode::SUCCESS;
}
