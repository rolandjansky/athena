/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// HGTDSensorSDTool.h, (c) ATLAS Detector software
//////////////////////////////////////////////////////////

#ifndef HGTD_G4_SD_HGTDSENSORSDTOOL_H
#define HGTD_G4_SD_HGTDSENSORSDTOOL_H

// Base class
#include "G4AtlasTools/SensitiveDetectorBase.h"

// STL headers
#include <string>

class G4VSensitiveDetector;

class HGTDSensorSDTool : public SensitiveDetectorBase
{
public:
  // Constructor
  HGTDSensorSDTool(const std::string& type, const std::string& name, const IInterface *parent);
    
  // Destructor
  virtual ~HGTDSensorSDTool() {}

protected:
  // Make me an SD!
  virtual G4VSensitiveDetector* makeSD() const override final;
  // property to toggle GeoModelXML 
  Gaudi::Property<bool> m_gmxSensor{this, "GmxSensor", false};
};

#endif // HGTD_G4_SD_HGTDSENSORSDTOOL_H
