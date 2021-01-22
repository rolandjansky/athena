/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// HGTD Sensitive Detector.
// The Hits are processed here. For every hit, the position and information
// on the sensor in which the interaction happened are obtained.

// Class header
#include "HGTDSensorSDTool.h"

// For the identifiers
#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"
#include "HGTD_Identifier/HGTD_ID.h"

// G4 stuff
#include "G4LogicalVolumeStore.hh"
#include "G4SDManager.hh"

// STL includes
#include <exception>

HGTDSensorSDTool::HGTDSensorSDTool(const std::string& type, const std::string& name, const IInterface* parent)
    : SensitiveDetectorBase( type , name , parent )
{
    declareInterface<ISensitiveDetector>(this);
}

StatusCode HGTDSensorSDTool::initialize()
{

    // initialize the sensor
    m_hgtdSD = CxxUtils::make_unique<HGTDSensorSD>(name());

    return StatusCode::SUCCESS;
}

G4VSensitiveDetector* HGTDSensorSDTool::makeSD()
{
    return m_hgtdSD.get();
}
