/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_G4_SD_ALFA_SensitiveDetectorTool_h
#define ALFA_G4_SD_ALFA_SensitiveDetectorTool_h

// Base class header
#include "G4AtlasTools/SensitiveDetectorBase.h"

// STL headers
#include <string>

class G4VSensitiveDetector;

class ALFA_SensitiveDetectorTool : public SensitiveDetectorBase {

public:
  // Constructor
  ALFA_SensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface* parent);
  // Destructor
  ~ALFA_SensitiveDetectorTool() {}
  /** Beginning of an athena event.  This is where collection initialization should happen.*/
  StatusCode SetupEvent() override final; //FIXME would be good to be able to avoid this.
  /** End of an athena event */
  StatusCode Gather() override final; //FIXME would be good to be able to avoid this.
protected:
  // Make me an SD!
  G4VSensitiveDetector* makeSD() override final;
};

#endif //ALFA_G4_SD_ALFA_SensitiveDetectorTool_h
