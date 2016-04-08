/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class TileGeoG4CalibSDTool.
// AthTool class for holding the Calibration Sensitive Detector for TileCal simulation
//
//************************************************************

#ifndef TILEGEOG4CALIB_TILEGEOG4CALIBSDTOOL_H
#define TILEGEOG4CALIB_TILEGEOG4CALIBSDTOOL_H

#include "G4AtlasTools/SensitiveDetectorBase.h"

// Uses
#include <string>

// Members
class G4VSensitiveDetector;
#include "TileGeoG4SD/TileSDOptions.h"

class TileGeoG4CalibSDTool: public SensitiveDetectorBase {
  public:
    TileGeoG4CalibSDTool(const std::string& type, const std::string& name, const IInterface *parent);
    // Default constructor is fine

    /** End of an athena event */
    StatusCode Gather() override final; //FIXME would be good to be able to avoid this.

  private:
    TileGeoG4CalibSDTool(const TileGeoG4CalibSDTool&) = delete;
    TileGeoG4CalibSDTool& operator=(const TileGeoG4CalibSDTool&) = delete;
    TileSDOptions m_options;

  protected:
    // Make me an SD!
    G4VSensitiveDetector* makeSD() override final;

};

#endif // TILEGEOG4CALIB_TILEGEOG4CALIBSDTOOL_H

