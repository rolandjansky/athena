/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_SD_ZDC_PIXEL_SDTOOL_H
#define ZDC_SD_ZDC_PIXEL_SDTOOL_H

// Base class
#include "G4AtlasTools/SensitiveDetectorBase.h"

// STL headers
#include <string>

class G4VSensitiveDetector;

class ZDC_PixelSDTool : public SensitiveDetectorBase
{

 public:
  // Constructor
  ZDC_PixelSDTool(const std::string& type, const std::string& name, const IInterface* parent);

  // Destructor
  ~ZDC_PixelSDTool() {};
  /** Beginning of an athena event.  This is where collection initialization should happen.*/
  StatusCode SetupEvent() override final; //FIXME would be good to be able to avoid this.
  /** End of an athena event */
  StatusCode Gather() override final; //FIXME would be good to be able to avoid this.

  // Make me an SD!
  G4VSensitiveDetector* makeSD() override final;

};

#endif //ZDC_SD_ZDC_PIXEL_SDTOOL_H
