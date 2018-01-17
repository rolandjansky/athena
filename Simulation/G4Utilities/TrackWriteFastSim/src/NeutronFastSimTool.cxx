/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "NeutronFastSimTool.h"

#include "TrackWriteFastSim/NeutronFastSim.h"

NeutronFastSimTool::NeutronFastSimTool(const std::string& type, const std::string& name, const IInterface *parent)
  : FastSimulationBase(type,name,parent),
    m_trackFastSimSDName("TrackFastSimSDTool"),
    m_etaCut(6.0),
    m_timeCut(150.)
{
  declareProperty("TrackFastSimSDName", m_trackFastSimSDName);
  declareProperty("PrimaryEtaCut",m_etaCut);
  declareProperty("NeutronTimeCut",m_timeCut);
}

G4VFastSimulationModel* NeutronFastSimTool::makeFastSimModel()
{
  ATH_MSG_DEBUG( "Initializing Fast Sim Model" );

  // Create a fresh Fast Sim Model
  return new NeutronFastSim(name(),m_trackFastSimSDName,m_etaCut,m_timeCut);
}
