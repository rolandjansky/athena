/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "NeutronFastSimTool.h"

#include "TrackWriteFastSim/NeutronFastSim.h"

NeutronFastSimTool::NeutronFastSimTool(const std::string& type, const std::string& name, const IInterface *parent)
  : FastSimulationBase(type,name,parent),
    m_trackFastSimSDTool("TrackFastSimSDTool")
{
  declareProperty("PrimaryEtaCut",m_etaCut);
  declareProperty("NeutronTimeCut",m_timeCut);
  declareInterface<IFastSimulation>(this);
}

StatusCode NeutronFastSimTool::initialize()
{
  CHECK(m_trackFastSimSDTool.retrieve());
  CHECK(m_trackFastSimSDTool->initializeSD());
  return StatusCode::SUCCESS;
}

G4VFastSimulationModel* NeutronFastSimTool::makeFastSimModel()
{
  ATH_MSG_DEBUG( "Initializing Fast Sim Model" );

  // Create a fresh Fast Sim Model
  return new NeutronFastSim(name(),m_trackFastSimSDTool->name(),m_etaCut,m_timeCut);
}
