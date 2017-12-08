/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header include
#include "SimpleFastKillerTool.h"

// the Fast Sim Model itself
#include "SimpleFastKiller.h"

SimpleFastKillerTool::SimpleFastKillerTool(const std::string& type, const std::string& name, const IInterface *parent)
  : FastSimulationBase(type,name,parent)
{
  m_regionNames = { "BeampipeFwdCut" };
  declareInterface<IFastSimulation>(this);
}

G4VFastSimulationModel* SimpleFastKillerTool::makeFastSimModel()
{
  ATH_MSG_DEBUG( "Initializing Fast Sim Model" );

  // Create a fresh Fast Sim Model
  return new SimpleFastKiller(name());
}
