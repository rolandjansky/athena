/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************
   SCT Sensitive Detector (GMX) class
****************************************************************/

#ifndef SCT_G4_SD_SCTSENSORGMXSD_H
#define SCT_G4_SD_SCTSENSORGMXSD_H

// Base class
#include "SctSensorSD.h"

// G4 needed classes
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class SctSensorGmxSD : public SctSensorSD
{
public:
  // Constructor
  SctSensorGmxSD(const std::string& name, const std::string& hitCollectionName);

  // Destructor
  ~SctSensorGmxSD();

  // Deal with each G4 hit
  G4bool ProcessHits(G4Step*, G4TouchableHistory*) override final;
};

#endif //SCT_G4_SD_SCTSENSORGMXSD_H
