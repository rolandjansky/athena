/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class PhantomBarrelSD.
// Tool for configuring the sensitive detector for the phantom calorimeter in combined 2004
//
//************************************************************

#include "PhantomBarrelSDTool.hh"
#include "PhantomBarrelSD.hh"

PhantomBarrelSDTool::PhantomBarrelSDTool(const std::string& type, const std::string& name, const IInterface* parent)
    : SensitiveDetectorBase(type, name, parent) {
  declareInterface<ISensitiveDetector>(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhantomBarrelSDTool::~PhantomBarrelSDTool() {

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StatusCode PhantomBarrelSDTool::SetupEvent() {
  ATH_MSG_VERBOSE("PhantomBarrelSDTool::SetupEvent()");
  if (!getSD()) {
    ATH_MSG_ERROR("SetupEvent: PhantomBarrelSD never created!");
    return StatusCode::FAILURE;
  } else {
    PhantomBarrelSD *localSD = dynamic_cast<PhantomBarrelSD*>(getSD());
    if (!localSD) {
      ATH_MSG_ERROR("SetupEvent: Failed to cast m_SD into PhantomBarrelSD.");
      return StatusCode::FAILURE;
    }
    localSD->StartOfAthenaEvent();
  }
  return StatusCode::SUCCESS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StatusCode PhantomBarrelSDTool::Gather() {
  ATH_MSG_VERBOSE("PhantomBarrelSDTool::Gather()");
  if (!getSD()) {
    ATH_MSG_ERROR("Gather: PhantomBarrelSD never created!");
    return StatusCode::FAILURE;
  } else {
    PhantomBarrelSD *localSD = dynamic_cast<PhantomBarrelSD*>(getSD());
    if (!localSD) {
      ATH_MSG_ERROR("Gather: Failed to cast m_SD into PhantomBarrelSD.");
      return StatusCode::FAILURE;
    }
    localSD->EndOfAthenaEvent();
  }
  return StatusCode::SUCCESS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* PhantomBarrelSDTool::makeSD() {
  ATH_MSG_DEBUG("Initializing SD");
  // Create a fresh SD
  return new PhantomBarrelSD(name(), m_outputCollectionNames[0]);
}
