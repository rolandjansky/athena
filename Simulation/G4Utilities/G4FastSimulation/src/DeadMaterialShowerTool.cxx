/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DeadMaterialShowerTool.h"

#include "DeadMaterialShower.h"

#include "CLHEP/Units/SystemOfUnits.h"

DeadMaterialShowerTool::DeadMaterialShowerTool(const std::string& type, const std::string& name, const IInterface *parent)
  : FastSimulationBase(type,name,parent)
{
  declareProperty( "HighEnergy" , m_highEnergy = 1000.0*CLHEP::GeV , "Kill everything but muons up to this energy" );
  declareProperty( "LowEnergy" , m_lowEnergy = 0.0*CLHEP::GeV , "Kill only electrons and positrons up to this energy" );
  declareProperty( "ZCutoff" , m_zcutoff = 5698.*CLHEP::mm , "Z-position to start killing everything but muons" );
}

G4VFastSimulationModel* DeadMaterialShowerTool::makeFastSimModel()
{
  ATH_MSG_DEBUG( "Initializing Fast Sim Model" );

  // Create a fresh Fast Sim Model
  return new DeadMaterialShower(name(), m_highEnergy, m_lowEnergy, m_zcutoff);
}
