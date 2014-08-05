/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeMcEventCollection.h"
#include "PileUpTools/IPileUpTool.h"

MergeMcEventCollection::MergeMcEventCollection(const std::string& name,
					       ISvcLocator *svcLoc):
  AthAlgorithm(name, svcLoc),
  m_mergeTool("MergeMcEventCollTool", this)
{
  declareProperty("MergeMcEventCollTool",   m_mergeTool);
}

StatusCode MergeMcEventCollection::initialize() 
{
  ATH_MSG_DEBUG ( "Initializing " << name() << " - package version " << PACKAGE_VERSION ); 
  if(m_mergeTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve MergeMcEventCollTool!");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Retrieved MergeMcEventCollTool (" << m_mergeTool->name() << ").");
  return StatusCode::SUCCESS;
}

StatusCode MergeMcEventCollection::execute() 
{
  ATH_MSG_DEBUG("execute()");  
  return m_mergeTool->processAllSubEvents();
}

StatusCode MergeMcEventCollection::finalize() 
{
  ATH_MSG_DEBUG("finalize.");
  return StatusCode::SUCCESS;
}

