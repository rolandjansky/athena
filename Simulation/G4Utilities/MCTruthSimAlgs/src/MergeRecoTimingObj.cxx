/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeRecoTimingObj.h"

#include "PileUpTools/IPileUpTool.h"

MergeRecoTimingObj::MergeRecoTimingObj(const std::string& name,
                               ISvcLocator* svcLoc)
  : AthAlgorithm(name, svcLoc)
{
}

StatusCode MergeRecoTimingObj::initialize() {
  ATH_MSG_DEBUG ( "Initializing " << name() );
  ATH_CHECK(m_mergeTool.retrieve());
  ATH_MSG_DEBUG("Retrieved MergeRecoTimingObjTool (" << m_mergeTool->name() << ").");

 return StatusCode::SUCCESS;
}

StatusCode MergeRecoTimingObj::execute() {
  ATH_MSG_DEBUG("execute()");
  return m_mergeTool->processAllSubEvents(Gaudi::Hive::currentContext());
}
