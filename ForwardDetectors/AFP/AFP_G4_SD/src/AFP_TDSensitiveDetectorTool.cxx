/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Class header
#include "AFP_TDSensitiveDetectorTool.h"

// For the SD itself
#include "AFP_TDSensitiveDetector.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

AFP_TDSensitiveDetectorTool::AFP_TDSensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase(type,name,parent)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StatusCode AFP_TDSensitiveDetectorTool::SetupEvent()
{
  if(!getSD())
    {
      ATH_MSG_ERROR ("SetupEvent: AFP_TDSensitiveDetector never created!");
      return StatusCode::FAILURE;
    }
  else
    {
      AFP_TDSensitiveDetector *localSD = dynamic_cast<AFP_TDSensitiveDetector*>(getSD());
      if(!localSD)
        {
          ATH_MSG_ERROR ("SetupEvent: Failed to cast m_SD into AFP_TDSensitiveDetector.");
          return StatusCode::FAILURE;
        }
      localSD->StartOfAthenaEvent();
    }
  return StatusCode::SUCCESS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StatusCode AFP_TDSensitiveDetectorTool::Gather()
{
  ATH_MSG_VERBOSE( "AFP_TDSensitiveDetectorTool::Gather()" );
  if(!getSD())
    {
      ATH_MSG_ERROR ("Gather: AFP_TDSensitiveDetector never created!");
      return StatusCode::FAILURE;
    }
  else
    {
      AFP_TDSensitiveDetector *localSD = dynamic_cast<AFP_TDSensitiveDetector*>(getSD());
      if(!localSD)
        {
          ATH_MSG_ERROR ("Gather: Failed to cast m_SD into AFP_TDSensitiveDetector.");
          return StatusCode::FAILURE;
        }
      localSD->EndOfAthenaEvent();
    }
  return StatusCode::SUCCESS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* AFP_TDSensitiveDetectorTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );
  // Create a fresh SD
  return new AFP_TDSensitiveDetector(name(), m_outputCollectionNames[0]);
}

