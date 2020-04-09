/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeTrackRecordCollection.h"

#include "PileUpTools/IPileUpTool.h"

MergeTrackRecordCollection::MergeTrackRecordCollection(const std::string& name,
                               ISvcLocator* svcLoc)
  : AthAlgorithm(name, svcLoc)
{
}

StatusCode MergeTrackRecordCollection::initialize() {
  ATH_MSG_DEBUG ( "Initializing " << name() );
  ATH_CHECK(m_mergeTool.retrieve());
  ATH_MSG_DEBUG("Retrieved MergeTrackRecordCollTool (" << m_mergeTool->name() << ").");

 return StatusCode::SUCCESS;
}

StatusCode MergeTrackRecordCollection::execute() {
  ATH_MSG_DEBUG("execute()");
  return m_mergeTool->processAllSubEvents(Gaudi::Hive::currentContext());
}
