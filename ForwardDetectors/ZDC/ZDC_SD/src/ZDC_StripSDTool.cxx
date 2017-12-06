/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Class header
#include "ZDC_StripSDTool.h"

// For the SD itself
#include "ZDC_StripSD.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ZDC_StripSDTool::ZDC_StripSDTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase(type,name,parent)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StatusCode ZDC_StripSDTool::SetupEvent()
{
  ATH_MSG_VERBOSE( "ZDC_StripSDTool::SetupEvent()" );
  if(!getSD())
    {
      ATH_MSG_ERROR ("SetupEvent: ZDC_StripSD never created!");
      return StatusCode::FAILURE;
    }
  else
    {
      ZDC_StripSD *localSD = dynamic_cast<ZDC_StripSD*>(getSD());
      if(!localSD)
        {
          ATH_MSG_ERROR ("SetupEvent: Failed to cast m_SD into ZDC_StripSD.");
          return StatusCode::FAILURE;
        }
      localSD->StartOfAthenaEvent();
    }
  return StatusCode::SUCCESS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StatusCode ZDC_StripSDTool::Gather()
{
  ATH_MSG_VERBOSE( "ZDC_StripSDTool::Gather()" );
  if(!getSD())
    {
      ATH_MSG_ERROR ("Gather: ZDC_StripSD never created!");
      return StatusCode::FAILURE;
    }
  else
    {
      ZDC_StripSD *localSD = dynamic_cast<ZDC_StripSD*>(getSD());
      if(!localSD)
        {
          ATH_MSG_ERROR ("Gather: Failed to cast m_SD into ZDC_StripSD.");
          return StatusCode::FAILURE;
        }
      localSD->EndOfAthenaEvent();
    }
  return StatusCode::SUCCESS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* ZDC_StripSDTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );
  // Create a fresh SD
  return new ZDC_StripSD(name(), m_outputCollectionNames[0]);
}
