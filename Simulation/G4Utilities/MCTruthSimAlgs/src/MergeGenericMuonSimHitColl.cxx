/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeGenericMuonSimHitColl.h"

#include "PileUpTools/IPileUpTool.h"

MergeGenericMuonSimHitColl::MergeGenericMuonSimHitColl(const std::string& name,
                               ISvcLocator* svcLoc)
  : AthAlgorithm(name, svcLoc)
{
}

StatusCode MergeGenericMuonSimHitColl::initialize() {
  ATH_MSG_DEBUG ( "Initializing " << name() );
  ATH_CHECK(m_mergeTool.retrieve());
  ATH_MSG_DEBUG("Retrieved MergeGenericMuonSimHitCollTool (" << m_mergeTool->name() << ").");

 return StatusCode::SUCCESS;
}

StatusCode MergeGenericMuonSimHitColl::execute() {
  ATH_MSG_DEBUG("execute()");
  return m_mergeTool->processAllSubEvents(Gaudi::Hive::currentContext());
}
