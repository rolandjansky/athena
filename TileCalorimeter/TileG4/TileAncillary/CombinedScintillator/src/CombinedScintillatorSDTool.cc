/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class CombinedScintillatorSDTool
// Sensitive detector for the Scintillator between LAR & Tile
//
//************************************************************

#include "CombinedScintillatorSDTool.hh"
#include "CombinedScintillatorSD.hh"

CombinedScintillatorSDTool::CombinedScintillatorSDTool(const std::string& type, const std::string& name,
                                                       const IInterface* parent)
    : SensitiveDetectorBase(type, name, parent)
{
  declareInterface<ISensitiveDetector>(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CombinedScintillatorSDTool::~CombinedScintillatorSDTool() {

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StatusCode CombinedScintillatorSDTool::SetupEvent() {
  ATH_MSG_VERBOSE("CombinedScintillatorSDTool::SetupEvent()");
  if (!getSD()) {
    ATH_MSG_ERROR("SetupEvent: CombinedScintillatorSD never created!");
    return StatusCode::FAILURE;
  } else {
    CombinedScintillatorSD *localSD = dynamic_cast<CombinedScintillatorSD*>(getSD());
    if (!localSD) {
      ATH_MSG_ERROR("SetupEvent: Failed to cast m_SD into CombinedScintillatorSD.");
      return StatusCode::FAILURE;
    }
    localSD->StartOfAthenaEvent();
  }
  return StatusCode::SUCCESS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StatusCode CombinedScintillatorSDTool::Gather() {
  ATH_MSG_VERBOSE("CombinedScintillatorSDTool::Gather()");
  if (!getSD()) {
    ATH_MSG_ERROR("Gather: CombinedScintillatorSD never created!");
    return StatusCode::FAILURE;
  } else {
    CombinedScintillatorSD *localSD = dynamic_cast<CombinedScintillatorSD*>(getSD());
    if (!localSD) {
      ATH_MSG_ERROR("Gather: Failed to cast m_SD into CombinedScintillatorSD.");
      return StatusCode::FAILURE;
    }
    localSD->EndOfAthenaEvent();
  }
  return StatusCode::SUCCESS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* CombinedScintillatorSDTool::makeSD() {
  ATH_MSG_DEBUG("Initializing SD");
  // Create a fresh SD
  return new CombinedScintillatorSD(name(), m_outputCollectionNames[0]);
}
