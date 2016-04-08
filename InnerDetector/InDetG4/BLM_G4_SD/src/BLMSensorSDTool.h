/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//###############################################
//   BLM Sensitive Detector tool
//
//###############################################

#ifndef BLM_G4_SD_BLMSENSORSDTOOL_H
#define BLM_G4_SD_BLMSENSORSDTOOL_H

// Base class
#include "G4AtlasTools/SensitiveDetectorBase.h"

class BLMSensorSDTool : public SensitiveDetectorBase
{
 public:
  // Constructor
  BLMSensorSDTool(const std::string& type, const std::string& name, const IInterface *parent);

  // Destructor
  ~BLMSensorSDTool() {}

protected:
  // Make me an SD!
  G4VSensitiveDetector* makeSD() override final;

};

#endif //BLM_G4_SD_BLMSENSORSDTOOL_H
