/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//###############################################
//   BCM Sensitive Detector class
//   Bostjan Macek 17.may.2007
//###############################################

#ifndef BCM_G4_SD_BCMSENSORSDTOOL_H
#define BCM_G4_SD_BCMSENSORSDTOOL_H

// Base class
#include "G4AtlasTools/SensitiveDetectorBase.h"

class BCMSensorSDTool : public SensitiveDetectorBase
{
 public:
  // Constructor
  BCMSensorSDTool(const std::string& type, const std::string& name, const IInterface *parent);

  // Destructor
  ~BCMSensorSDTool() {}

protected:
  // Make me an SD!
  G4VSensitiveDetector* makeSD() override final;

};

#endif //BCM_G4_SD_BCMSENSORSDTOOL_H
