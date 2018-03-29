/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Class header
#include "AFP_SiDSensitiveDetectorTool.h"

// For the SD itself
#include "AFP_SiDSensitiveDetector.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

AFP_SiDSensitiveDetectorTool::AFP_SiDSensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase(type,name,parent)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StatusCode AFP_SiDSensitiveDetectorTool::SetupEvent()
{
  ATH_MSG_VERBOSE( "AFP_SiDSensitiveDetectorTool::SetupEvent()" );
  if(!getSD())
    {
      ATH_MSG_ERROR ("SetupEvent: AFP_SiDSensitiveDetector never created!");
      return StatusCode::FAILURE;
    }
  else
    {
      AFP_SiDSensitiveDetector *localSD = dynamic_cast<AFP_SiDSensitiveDetector*>(getSD());
      if(!localSD)
        {
          ATH_MSG_ERROR ("SetupEvent: Failed to cast m_SD into AFP_SiDSensitiveDetector.");
          return StatusCode::FAILURE;
        }
      localSD->StartOfAthenaEvent();
    }
  return StatusCode::SUCCESS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StatusCode AFP_SiDSensitiveDetectorTool::Gather()
{
  ATH_MSG_VERBOSE( "AFP_SiDSensitiveDetectorTool::Gather()" );
  if(!getSD())
    {
      ATH_MSG_ERROR ("Gather: AFP_SiDSensitiveDetector never created!");
      return StatusCode::FAILURE;
    }
  else
    {
      AFP_SiDSensitiveDetector *localSD = dynamic_cast<AFP_SiDSensitiveDetector*>(getSD());
      if(!localSD)
        {
          ATH_MSG_ERROR ("Gather: Failed to cast m_SD into AFP_SiDSensitiveDetector.");
          return StatusCode::FAILURE;
        }
      localSD->EndOfAthenaEvent();
    }
  return StatusCode::SUCCESS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* AFP_SiDSensitiveDetectorTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );
  // Create a fresh SD
  return new AFP_SiDSensitiveDetector(name(), m_outputCollectionNames[0]);
}

