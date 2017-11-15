/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class MuonWallSD.
// Sensitive detector for the muon wall
//
// Author: franck Martin <Franck.Martin@cern.ch>
// january 12, 2004
//
//************************************************************

#include "MuonWallSDTool.h"
#include "MuonWallSD.h"

MuonWallSDTool::MuonWallSDTool(const std::string& type, const std::string& name, const IInterface* parent)
    : SensitiveDetectorBase(type, name, parent) {
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MuonWallSDTool::~MuonWallSDTool() {

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StatusCode MuonWallSDTool::SetupEvent() {
  ATH_MSG_VERBOSE("MuonWallSDTool::SetupEvent()");
  if (!getSD()) {
    ATH_MSG_ERROR("SetupEvent: MuonWallSD never created!");
    return StatusCode::FAILURE;
  } else {
    MuonWallSD *localSD = dynamic_cast<MuonWallSD*>(getSD());
    if (!localSD) {
      ATH_MSG_ERROR("SetupEvent: Failed to cast m_SD into MuonWallSD.");
      return StatusCode::FAILURE;
    }
    localSD->StartOfAthenaEvent();
  }
  return StatusCode::SUCCESS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StatusCode MuonWallSDTool::Gather() {
  ATH_MSG_VERBOSE("MuonWallSDTool::Gather()");
  if (!getSD()) {
    ATH_MSG_ERROR("Gather: MuonWallSD never created!");
    return StatusCode::FAILURE;
  } else {
    MuonWallSD *localSD = dynamic_cast<MuonWallSD*>(getSD());
    if (!localSD) {
      ATH_MSG_ERROR("Gather: Failed to cast m_SD into MuonWallSD.");
      return StatusCode::FAILURE;
    }
    localSD->EndOfAthenaEvent();
  }
  return StatusCode::SUCCESS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* MuonWallSDTool::makeSD() {
  ATH_MSG_DEBUG("Initializing SD");
  // Create a fresh SD
  return new MuonWallSD(name(), m_outputCollectionNames[0]);
}
