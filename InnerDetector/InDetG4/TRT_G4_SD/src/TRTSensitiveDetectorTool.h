/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_G4_SD_TRTSENSITIVEDETECTORTOOL_H
#define TRT_G4_SD_TRTSENSITIVEDETECTORTOOL_H

// Base class
#include "G4AtlasTools/SensitiveDetectorBase.h"

// STL headers
#include <string>

class G4VSensitiveDetector;

class TRTSensitiveDetectorTool : public SensitiveDetectorBase
{
 public:
  // Constructor
  TRTSensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface *parent);

  // Destructor
  ~TRTSensitiveDetectorTool() { /* don't own any of the pointers... */ }

protected:
  // Make me an SD!
  G4VSensitiveDetector* makeSD() override final;

};

#endif //TRT_G4_SD_TRTSENSITIVEDETECTORTOOL_H
