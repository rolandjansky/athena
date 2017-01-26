/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArFastShowerTool.h"
#include "BarrelFastSimDedicatedSD.h"
#include "EndcapFastSimDedicatedSD.h"
#include "FCALFastSimDedicatedSD.h"
#include "LArFastShower.h"
#include "G4SDManager.hh"

LArFastShowerTool::LArFastShowerTool(const std::string& type, const std::string& name,
                                     const IInterface *parent)
  : FastSimulationBase(type, name, parent),
    m_fastSimDedicatedSD (""), // Empty by default.
    m_showerLibSvc("LArG4ShowerLibSvc", name)
{
  declareProperty("EFlagToShowerLib", m_configuration.m_e_FlagShowerLib = true, "Switch for e+/- frozen showers");
  declareProperty("EMinEneShowerLib", m_configuration.m_e_MinEneShowerLib = 0.0*CLHEP::GeV, "Minimum energy for e+/- frozen showers");
  declareProperty("EMaxEneShowerLib", m_configuration.m_e_MaxEneShowerLib = 1.0*CLHEP::GeV, "Maximum energy for e+/- frozen showers");

  declareProperty("GFlagToShowerLib", m_configuration.m_g_FlagShowerLib = true, "Switch for photon frozen showers");
  declareProperty("GMinEneShowerLib", m_configuration.m_g_MinEneShowerLib = 0.*CLHEP::GeV, "Minimum energy for photon frozen showers");
  declareProperty("GMaxEneShowerLib", m_configuration.m_g_MaxEneShowerLib = 0.010*CLHEP::GeV, "Maximum energy for photon frozen showers");

  declareProperty("NeutFlagToShowerLib", m_configuration.m_Neut_FlagShowerLib = true, "Switch for neutron frozen showers");
  declareProperty("NeutMinEneShowerLib", m_configuration.m_Neut_MinEneShowerLib = 0.0*CLHEP::GeV, "Minimum energy for neutron frozen showers");
  declareProperty("NeutMaxEneShowerLib", m_configuration.m_Neut_MaxEneShowerLib = 0.1*CLHEP::GeV, "Maximum energy for neutron frozen showers");

  declareProperty("PionFlagToShowerLib", m_configuration.m_Pion_FlagShowerLib = true, "Switch for neutron frozen showers");
  declareProperty("PionMinEneShowerLib", m_configuration.m_Pion_MinEneShowerLib = 0.0*CLHEP::GeV, "Minimum energy for neutron frozen showers");
  declareProperty("PionMaxEneShowerLib", m_configuration.m_Pion_MaxEneShowerLib = 2.0*CLHEP::GeV, "Maximum energy for neutron frozen showers");

  declareProperty("ContainLow",   m_configuration.m_containLow = true, "Switch for containment at low eta");
  declareProperty("AbsLowEta",    m_configuration.m_absLowEta, "");
  declareProperty("ContainHigh",  m_configuration.m_containHigh = true, "Switch for containment at high eta");
  declareProperty("AbsHighEta",   m_configuration.m_absHighEta, "");
  declareProperty("ContainCrack", m_configuration.m_containCrack = true, "Switch for containment in the crack region");
  declareProperty("AbsCrackEta1", m_configuration.m_absCrackEta1, "");
  declareProperty("AbsCrackEta2", m_configuration.m_absCrackEta2, "");

  declareProperty("GeneratedStartingPointsFile", m_configuration.m_generated_starting_points_file = "",
                  "Name of file for generated SPs. Do not touch until you want to produce a new library");
  declareProperty("GeneratedStartingPointsRatio", m_configuration.m_generated_starting_points_ratio = 0.02, "Ratio of SPs that goes to output");
  declareProperty("DetectorTag", m_configuration.m_detector_tag, "Which detector is this?");
  declareProperty("SensitiveDetector" , m_fastSimDedicatedSD , "Fast sim dedicated SD for this setup");
  declareProperty("ShowerLibSvc" , m_showerLibSvc, "Handle on the shower library service");
  m_configuration.m_showerLibSvcName = m_showerLibSvc.name();

  declareInterface<IFastSimulation>(this);
}

StatusCode LArFastShowerTool::initialize()
{
  ATH_MSG_VERBOSE( name() << "::initialize()" );
  CHECK( m_showerLibSvc.retrieve() );
  return FastSimulationBase::initialize();
}

G4VFastSimulationModel* LArFastShowerTool::makeFastSimModel()
{
  ATH_MSG_DEBUG( "Initializing Fast Sim Model" );
  IFastSimDedicatedSD* fastSD = dynamic_cast<IFastSimDedicatedSD*>(
    G4SDManager::GetSDMpointer()->FindSensitiveDetector(m_fastSimDedicatedSD, false) );
  if (fastSD){
    ATH_MSG_INFO( "SD " << m_fastSimDedicatedSD << " already created." );
  } else if ("BarrelFastSimDedicatedSD" == m_fastSimDedicatedSD){
    fastSD = new BarrelFastSimDedicatedSD( &*detStore() );
  } else if ("EndcapFastSimDedicatedSD" == m_fastSimDedicatedSD){
    fastSD = new EndcapFastSimDedicatedSD( &*detStore() );
  } else if ("FCALFastSimDedicatedSD" == m_fastSimDedicatedSD){
    fastSD = new FCALFastSimDedicatedSD( &*detStore() );
  } else {
    ATH_MSG_FATAL( "Fast sim SD type " << m_fastSimDedicatedSD << " not found!" );
    throw std::runtime_error("Bad SD name");
  }
  G4SDManager::GetSDMpointer()->AddNewDetector(fastSD);

  // Create a fresh Fast Sim Model
  return new LArFastShower(name(), m_configuration, fastSD);
}

StatusCode LArFastShowerTool::EndOfAthenaEvent()
{
  // SD is taken care of by the SD tools
  return StatusCode::SUCCESS;
}
