/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class TileGeoG4CalibSDTool.
// AthTool class for holding the Calibration Sensitive Detector for TileCal
//
//************************************************************

#include "TileGeoG4CalibSDTool.h"
#include "TileGeoG4CalibSD.h"
#include "TileG4Interfaces/ITileCalculator.h"

TileGeoG4CalibSDTool::TileGeoG4CalibSDTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase(type, name, parent)
  , m_tileCalculator("TileGeoG4SDCalc", name)
{
  declareProperty("TileCalculator", m_tileCalculator);
}

StatusCode TileGeoG4CalibSDTool::initialize()
{
  ATH_CHECK(m_tileCalculator.retrieve());
  return StatusCode::SUCCESS;
}

G4VSensitiveDetector* TileGeoG4CalibSDTool::makeSD() const {
  if (m_outputCollectionNames.size() < 4) {
    ATH_MSG_ERROR( "Expected 4 output collection names, found " << m_outputCollectionNames.size()
                   << ". Expect the job to crash when the SD is created.");
  }
  return new TileGeoG4CalibSD(name(), m_outputCollectionNames, &*m_tileCalculator, detStore());
}

StatusCode TileGeoG4CalibSDTool::Gather() {
  ATH_MSG_VERBOSE("TileGeoG4SDTool::Gather()");
  if (!getSD()) {
    ATH_MSG_ERROR("Gather: TileGeoG4SD never created!");
    return StatusCode::FAILURE;
  } else {
    TileGeoG4CalibSD *localSD = dynamic_cast<TileGeoG4CalibSD*>(getSD());
    if (!localSD) {
      ATH_MSG_ERROR("Gather: Failed to cast m_SD into TileGeoG4CalibSD.");
      return StatusCode::FAILURE;
    }
    localSD->EndOfAthenaEvent();
  }
  return StatusCode::SUCCESS;
}

