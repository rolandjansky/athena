/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SCT Sensitive Detector Tool.
//

//class header
#include "SctSensor_CTBTool.h"

//package includes
#include "SctSensor_CTB.h"

// STL includes
#include <exception>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SctSensor_CTBTool::SctSensor_CTBTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* SctSensor_CTBTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );

  return new SctSensor_CTB(name(), m_outputCollectionNames[0]);
}

