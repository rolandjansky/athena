/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_G4_SD_LUCID_SensitiveDetectorTool_h
#define LUCID_G4_SD_LUCID_SensitiveDetectorTool_h

// Base class
#include "G4AtlasTools/SensitiveDetectorBase.h"

// STL headers
#include <string>

class G4VSensitiveDetector;

class LUCID_SensitiveDetectorTool : public SensitiveDetectorBase {
public:
  // Constructor
  LUCID_SensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface* parent);

  // Destructor
  ~LUCID_SensitiveDetectorTool() {}

protected:
  // Make me an SD!
  G4VSensitiveDetector* makeSD() override final;

};

#endif //LUCID_G4_SD_LUCID_SensitiveDetectorTool_h
