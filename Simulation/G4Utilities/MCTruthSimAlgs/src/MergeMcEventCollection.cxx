/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeMcEventCollection.h"
#include "PileUpTools/IPileUpTool.h"

MergeMcEventCollection::MergeMcEventCollection(const std::string& name,
                                               ISvcLocator *svcLoc):
  AthAlgorithm(name, svcLoc)
{
}

StatusCode MergeMcEventCollection::initialize()
{
  ATH_MSG_DEBUG ( "Initializing " << name() );
  ATH_CHECK(m_mergeTool.retrieve());
  ATH_MSG_DEBUG("Retrieved MergeMcEventCollTool (" << m_mergeTool->name() << ").");
  return StatusCode::SUCCESS;
}

StatusCode MergeMcEventCollection::execute()
{
  ATH_MSG_DEBUG("execute()");
  return m_mergeTool->processAllSubEvents(Gaudi::Hive::currentContext());
}
