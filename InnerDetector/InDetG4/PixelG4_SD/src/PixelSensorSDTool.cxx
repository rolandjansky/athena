/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Pixel Sensitive Detector.
// The Hits are processed here. For every hit I get the position and
// an information on the sensor in which the interaction happened

// Class header
#include "PixelSensorSDTool.h"

// For the SD itself
#include "PixelSensorSD.h"

// STL includes
#include <exception>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PixelSensorSDTool::PixelSensorSDTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{
  declareInterface<ISensitiveDetector>(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* PixelSensorSDTool::makeSD()
{
  // Make sure the job has been set up properly
  ATH_MSG_DEBUG( "Initializing SD" );

  // Create a fresh SD
  return new PixelSensorSD(name(), m_outputCollectionNames[0]);
}

