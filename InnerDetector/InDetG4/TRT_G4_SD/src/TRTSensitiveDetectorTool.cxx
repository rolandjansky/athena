/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Class header
#include "TRTSensitiveDetectorTool.h"

// Package headers
#include "TRTSensitiveDetector.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TRTSensitiveDetectorTool::TRTSensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{
  declareInterface<ISensitiveDetector>(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* TRTSensitiveDetectorTool::makeSD()
{

  ATH_MSG_DEBUG( "Initializing SD" );
  // Create a fresh SD
  //Need to set message levels here as the settings are needed in the constructor...
  int verboseLevel = 3;
  if(msgLvl(MSG::VERBOSE))    { verboseLevel = 10; }
  else if(msgLvl(MSG::DEBUG)) { verboseLevel = 5;  }
  return new TRTSensitiveDetector(name(), m_outputCollectionNames[0], verboseLevel);
}
