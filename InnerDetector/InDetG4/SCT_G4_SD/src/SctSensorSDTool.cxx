/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SCT Sensitive Detector Tool.
//

// class header
#include "SctSensorSDTool.h"

// package includes
#include "SctSensorSD.h"
#include "SctSensorGmxSD.h"

// STL includes
#include <exception>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SctSensorSDTool::SctSensorSDTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent ),m_isGmxSensor(false)
{
  declareProperty("GmxSensor",m_isGmxSensor);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* SctSensorSDTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );
  if(m_isGmxSensor)
    {
      return new SctSensorGmxSD(name(), m_outputCollectionNames[0]);
    }
  else
    {
      return new SctSensorSD(name(), m_outputCollectionNames[0]);
    }
}

