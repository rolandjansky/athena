/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoppedParticleFastSimTool.h"
#include "TrackWriteFastSim/StoppedParticleFastSim.h"

StoppedParticleFastSimTool::StoppedParticleFastSimTool(const std::string& type, const std::string& name, const IInterface *parent)
  : FastSimulationBase(type,name,parent),
    m_trackFastSimSDTool("TrackFastSimSDTool")
{
}

StatusCode StoppedParticleFastSimTool::initialize()
{
  CHECK(m_trackFastSimSDTool.retrieve());
  CHECK(m_trackFastSimSDTool->initializeSD());
  return StatusCode::SUCCESS;
}

G4VFastSimulationModel* StoppedParticleFastSimTool::makeFastSimModel()
{
  ATH_MSG_DEBUG( "Initializing Fast Sim Model" );

  // Create a fresh Fast Sim Model
  return new StoppedParticleFastSim(name(),m_trackFastSimSDTool->name());
}
