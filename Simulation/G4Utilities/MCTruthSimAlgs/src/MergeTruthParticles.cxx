/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeTruthParticles.h"

#include "PileUpTools/IPileUpTool.h"

MergeTruthParticles::MergeTruthParticles(const std::string& name,
                               ISvcLocator* svcLoc)
  : AthAlgorithm(name, svcLoc)
{
}

StatusCode MergeTruthParticles::initialize() {
  ATH_MSG_DEBUG ( "Initializing " << name() );
  ATH_CHECK(m_mergeTool.retrieve());
  ATH_MSG_DEBUG("Retrieved MergeTruthParticlesTool (" << m_mergeTool->name() << ").");
 return StatusCode::SUCCESS;
}

StatusCode MergeTruthParticles::execute() {
  ATH_MSG_DEBUG("execute()");
  return m_mergeTool->processAllSubEvents(Gaudi::Hive::currentContext());
}
