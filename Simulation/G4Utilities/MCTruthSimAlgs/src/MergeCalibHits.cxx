/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeCalibHits.h"

#include "PileUpTools/IPileUpTool.h"

MergeCalibHits::MergeCalibHits(const std::string& name,
                               ISvcLocator* svcLoc)
  : AthAlgorithm(name, svcLoc)
{
}

StatusCode MergeCalibHits::initialize() {
  ATH_MSG_DEBUG ( "Initializing " << name() );
  ATH_CHECK(m_mergeTool.retrieve());
  ATH_MSG_DEBUG("Retrieved MergeCalibHitsTool (" << m_mergeTool->name() << ").");
 return StatusCode::SUCCESS;
}

StatusCode MergeCalibHits::execute() {
  ATH_MSG_DEBUG("execute()");
  return m_mergeTool->processAllSubEvents(Gaudi::Hive::currentContext());
}
