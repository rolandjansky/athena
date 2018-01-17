/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Class header
#include "LUCID_SensitiveDetectorTool.h"

// Package includes
#include "LUCID_SensitiveDetector.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LUCID_SensitiveDetectorTool::LUCID_SensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase(type,name,parent)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* LUCID_SensitiveDetectorTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );
  // Create a fresh SD
  return new LUCID_SensitiveDetector(name(), m_outputCollectionNames[0]);
}
