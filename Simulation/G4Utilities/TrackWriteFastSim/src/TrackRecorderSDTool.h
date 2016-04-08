/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************
   Track Recorder Sensitive Detector Tool
 ****************************************************************/

#ifndef TRACKWRITEFASTSIM_TRACKRECORDERSDTOOL_H
#define TRACKWRITEFASTSIM_TRACKRECORDERSDTOOL_H

// Base class
#include "G4AtlasTools/SensitiveDetectorBase.h"

// STL headers
#include <string>

class G4VSensitiveDetector;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

class TrackRecorderSDTool : public SensitiveDetectorBase
{
 public:
  // Constructor
  TrackRecorderSDTool(const std::string& type, const std::string& name, const IInterface *parent);

  // Destructor
  ~TrackRecorderSDTool() { /* If all goes well we do not own myHitColl here */ }

protected:
  // Make me an SD!
  G4VSensitiveDetector* makeSD() override final;

  int m_SD_type; //TrackRecorderSD=1, CosmicTRSD=2
};

#endif //TRACKWRITEFASTSIM_TRACKRECORDERSDTOOL_H
