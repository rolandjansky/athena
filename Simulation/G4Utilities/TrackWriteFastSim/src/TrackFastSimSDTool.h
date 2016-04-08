/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKWRITEFASTSIM_TRACKFASTSIMSDTOOL_H
#define TRACKWRITEFASTSIM_TRACKFASTSIMSDTOOL_H

// Base class header
#include "G4AtlasTools/SensitiveDetectorBase.h"

// STL headers
#include <string>

class G4VSensitiveDetector;

class TrackFastSimSDTool : public SensitiveDetectorBase
{
public:
  // Constructor
  TrackFastSimSDTool(const std::string& type, const std::string& name, const IInterface* parent);
  // Destructor
  ~TrackFastSimSDTool() {}
protected:
  // Make me an SD!
  G4VSensitiveDetector* makeSD() override final;
};

#endif //TRACKWRITEFASTSIM_TRACKFASTSIMSDTOOL_H
