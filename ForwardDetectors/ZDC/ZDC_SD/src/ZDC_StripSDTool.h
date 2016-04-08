/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_SD_ZDC_STRIP_SDTOOL_H
#define ZDC_SD_ZDC_STRIP_SDTOOL_H

#include "G4AtlasTools/SensitiveDetectorBase.h"

// STL headers
#include <string>

class G4VSensitiveDetector;

class ZDC_StripSDTool : public SensitiveDetectorBase
{

 public:
  // Constructor
  ZDC_StripSDTool(const std::string& type, const std::string& name, const IInterface* parent);
  // Destructor
  ~ZDC_StripSDTool() {};
  /** Beginning of an athena event.  This is where collection initialization should happen.*/
  StatusCode SetupEvent() override final; //FIXME would be good to be able to avoid this.
  /** End of an athena event */
  StatusCode Gather() override final; //FIXME would be good to be able to avoid this.

protected:
  // Make me an SD!
  G4VSensitiveDetector* makeSD() override final;
};

#endif //ZDC_SD_ZDC_STRIP_SDTOOL_H
