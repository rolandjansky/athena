/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Class header
#include "AFP_SensitiveDetectorTool.h"

// For the SD itself
#include "AFP_SensitiveDetector.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

AFP_SensitiveDetectorTool::AFP_SensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase(type,name,parent)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StatusCode AFP_SensitiveDetectorTool::SetupEvent()
{
  ATH_MSG_VERBOSE( "AFP_SensitiveDetectorTool::SetupEvent()" );
  if(!getSD())
    {
      ATH_MSG_ERROR ("SetupEvent: AFP_SensitiveDetector never created!");
      return StatusCode::FAILURE;
    }
  else
    {
      AFP_SensitiveDetector *localSD = dynamic_cast<AFP_SensitiveDetector*>(getSD());
      if(!localSD)
        {
          ATH_MSG_ERROR ("SetupEvent: Failed to cast m_SD into AFP_SensitiveDetector.");
          return StatusCode::FAILURE;
        }
      localSD->StartOfAthenaEvent();
    }
  return StatusCode::SUCCESS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StatusCode AFP_SensitiveDetectorTool::Gather()
{
  ATH_MSG_VERBOSE( "AFP_SensitiveDetectorTool::Gather()" );
  if(!getSD())
    {
      ATH_MSG_ERROR ("Gather: AFP_SensitiveDetector never created!");
      return StatusCode::FAILURE;
    }
  else
    {
      AFP_SensitiveDetector *localSD = dynamic_cast<AFP_SensitiveDetector*>(getSD());
      if(!localSD)
        {
          ATH_MSG_ERROR ("Gather: Failed to cast m_SD into AFP_SensitiveDetector.");
          return StatusCode::FAILURE;
        }
      localSD->EndOfAthenaEvent();
    }
  return StatusCode::SUCCESS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* AFP_SensitiveDetectorTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );
  // Create a fresh SD
  return new AFP_SensitiveDetector(name(), m_outputCollectionNames[0], m_outputCollectionNames[1]);
}

