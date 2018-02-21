/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Class header
#include "ZDC_PixelSDTool.h"

// Package headers
#include "ZDC_PixelSD.h"

ZDC_PixelSDTool::ZDC_PixelSDTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase(type,name,parent)
{
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StatusCode ZDC_PixelSDTool::SetupEvent()
{
  ATH_MSG_VERBOSE( "ZDC_PixelSDTool::SetupEvent()" );
  if(!getSD())
    {
      ATH_MSG_ERROR ("SetupEvent: ZDC_PixelSD never created!");
      return StatusCode::FAILURE;
    }
  else
    {
      ZDC_PixelSD *localSD = dynamic_cast<ZDC_PixelSD*>(getSD());
      if(!localSD)
        {
          ATH_MSG_ERROR ("SetupEvent: Failed to cast m_SD into ZDC_PixelSD.");
          return StatusCode::FAILURE;
        }
      localSD->StartOfAthenaEvent();
    }
  return StatusCode::SUCCESS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StatusCode ZDC_PixelSDTool::Gather()
{
  ATH_MSG_VERBOSE( "ZDC_PixelSDTool::Gather()" );
  if(!getSD())
    {
      ATH_MSG_ERROR ("Gather: ZDC_PixelSD never created!");
      return StatusCode::FAILURE;
    }
  else
    {
      ZDC_PixelSD *localSD = dynamic_cast<ZDC_PixelSD*>(getSD());
      if(!localSD)
        {
          ATH_MSG_ERROR ("Gather: Failed to cast m_SD into ZDC_PixelSD.");
          return StatusCode::FAILURE;
        }
      localSD->EndOfAthenaEvent();
    }
  return StatusCode::SUCCESS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector*  ZDC_PixelSDTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );
  // Create a fresh SD
  return new ZDC_PixelSD(name(), m_outputCollectionNames[0]);
}
