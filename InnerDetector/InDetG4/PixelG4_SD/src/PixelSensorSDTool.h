/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************
   Pixel Sensitive Detector class
   Davide Costanzo Sep 4th 2002.
 ****************************************************************/

#ifndef PIXEL_G4_SD_PIXELSENSORSDTOOL_H
#define PIXEL_G4_SD_PIXELSENSORSDTOOL_H

// Base class
#include "G4AtlasTools/SensitiveDetectorBase.h"

// STL headers
#include <string>

class G4VSensitiveDetector;

class PixelSensorSDTool : public SensitiveDetectorBase
{
 public:
  // Constructor
  PixelSensorSDTool(const std::string& type, const std::string& name, const IInterface *parent);

  // Destructor
  virtual ~PixelSensorSDTool() {}

protected:
  // Make me an SD!
  G4VSensitiveDetector* makeSD() override final;

};

#endif //PIXEL_G4_SD_PIXELSENSORSDTOOL_H
