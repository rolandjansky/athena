/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Class header
#include "ALFA_SensitiveDetectorTool.h"

// Package includes
#include "ALFA_SensitiveDetector.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ALFA_SensitiveDetectorTool::ALFA_SensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase(type,name,parent)
{
  declareInterface<ISensitiveDetector>(this);
  // m_volumeNames = { "ALFA::logALFA_FiberU" , "ALFA::logALFA_FiberV" , "ALFA::logOD_FiberActive" ,
  //                   "ALFA::B7L1_GVS" , "ALFA::A7L1_GVS" , "ALFA::A7R1_GVS" , "ALFA::B7R1_GVS" };
  // for (unsigned int i=0;i<64;++i){
  //   m_volumeNames.push_back( "ALFA::logALFA_FiberU["+std::to_string(i)+"]" );
  //   m_volumeNames.push_back( "ALFA::logALFA_FiberV["+std::to_string(i)+"]" );
  // }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StatusCode ALFA_SensitiveDetectorTool::SetupEvent()
{
  ATH_MSG_VERBOSE( "ALFA_SensitiveDetectorTool::SetupEvent()" );
  if(!getSD())
    {
      ATH_MSG_ERROR ("SetupEvent: ALFA_SensitiveDetector never created!");
      return StatusCode::FAILURE;
    }
  else
    {
      ALFA_SensitiveDetector *localSD = dynamic_cast<ALFA_SensitiveDetector*>(getSD());
      if(!localSD)
        {
          ATH_MSG_ERROR ("SetupEvent: Failed to cast m_SD into ALFA_SensitiveDetector.");
          return StatusCode::FAILURE;
        }
      localSD->StartOfAthenaEvent();
    }
  return StatusCode::SUCCESS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StatusCode ALFA_SensitiveDetectorTool::Gather()
{
  ATH_MSG_VERBOSE( "ALFA_SensitiveDetectorTool::Gather()" );
  if(!getSD())
    {
      ATH_MSG_ERROR ("Gather: ALFA_SensitiveDetector never created!");
      return StatusCode::FAILURE;
    }
  else
    {
      ALFA_SensitiveDetector *localSD = dynamic_cast<ALFA_SensitiveDetector*>(getSD());
      if(!localSD)
        {
          ATH_MSG_ERROR ("Gather: Failed to cast m_SD into ALFA_SensitiveDetector.");
          return StatusCode::FAILURE;
        }
      localSD->EndOfAthenaEvent();
    }
  return StatusCode::SUCCESS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* ALFA_SensitiveDetectorTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );
  // Create a fresh SD
  return new ALFA_SensitiveDetector(name(), m_outputCollectionNames[0], m_outputCollectionNames[1]);
}
