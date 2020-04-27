/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeTruthJets.h"

#include "PileUpTools/IPileUpTool.h"

MergeTruthJets::MergeTruthJets(const std::string& name,
                               ISvcLocator* svcLoc)
  : AthAlgorithm(name, svcLoc)
{
}

StatusCode MergeTruthJets::initialize() {
  ATH_MSG_DEBUG ( "Initializing " << name() );
  ATH_CHECK(m_mergeTool.retrieve());
  ATH_MSG_DEBUG("Retrieved MergeTruthJetsTool (" << m_mergeTool->name() << ").");
 return StatusCode::SUCCESS;
}

StatusCode MergeTruthJets::execute() {
  ATH_MSG_DEBUG("execute()");
  return m_mergeTool->processAllSubEvents(Gaudi::Hive::currentContext());
}
