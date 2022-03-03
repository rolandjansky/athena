/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// HGTD Sensitive Detector.
// The Hits are processed here. For every hit, the position and information
// on the sensor in which the interaction happened are obtained.

// Class header
#include "HGTDSensorSDTool.h"

// For the SD itself
#include "HGTDSensorSD.h"
#include "HGTDSensorGmxSD.h"
// STL includes
#include <exception>

HGTDSensorSDTool::HGTDSensorSDTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{
}

G4VSensitiveDetector* HGTDSensorSDTool::makeSD() const
{
  // Make sure the job has been set up properly
  ATH_MSG_DEBUG( "Initializing SD" );
  // Create a fresh SD
  if (!m_gmxSensor){
    return new HGTDSensorSD(name(), m_outputCollectionNames[0]);
  } else {
    return new HGTDSensorGmxSD(name(), m_outputCollectionNames[0]);
  }
  
}
