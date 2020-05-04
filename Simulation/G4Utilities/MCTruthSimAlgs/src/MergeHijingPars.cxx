/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeHijingPars.h"

#include "PileUpTools/IPileUpTool.h"

MergeHijingPars::MergeHijingPars(const std::string& name, ISvcLocator* svcLoc)
  : AthAlgorithm(name, svcLoc)
{
}

StatusCode MergeHijingPars::initialize() {
  ATH_MSG_DEBUG ( "Initializing " << name() );
  ATH_CHECK(m_mergeTool.retrieve());
  ATH_MSG_DEBUG("Retrieved MergeHijingParsTool (" << m_mergeTool->name() << ").");
  return StatusCode::SUCCESS;
}

StatusCode MergeHijingPars::execute() {
  ATH_MSG_DEBUG("execute()");
  return m_mergeTool->processAllSubEvents(Gaudi::Hive::currentContext());
}
