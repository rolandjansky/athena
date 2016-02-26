/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//###############################################
//   BCM Sensitive Detector Tool
//
//###############################################

// Class header
#include "BCMSensorSDTool.h"

// For the SD itself
#include "BCMSensorSD.h"

BCMSensorSDTool::BCMSensorSDTool(const std::string& type, const std::string& name, const IInterface *parent) :
  SensitiveDetectorBase(type,name,parent)
{
  declareInterface<ISensitiveDetector>(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* BCMSensorSDTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );

  // Create a fresh SD
  return new BCMSensorSD(name(), m_outputCollectionNames[0]);
}


