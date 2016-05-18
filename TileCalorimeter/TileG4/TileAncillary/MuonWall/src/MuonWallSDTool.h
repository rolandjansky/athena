/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//**************************************************************
//
// Class MuonWallSDTool
//
// Tool for configuring the sensitive detector for the Muon Wall
//
//**************************************************************

#ifndef MUONWALL_MUONWALLSDTOOL_H
#define MUONWALL_MUONWALLSDTOOL_H

// Base class header
#include "G4AtlasTools/SensitiveDetectorBase.h"

//STL headers
#include <string>

class G4VSensitiveDetector;

class MuonWallSDTool: public SensitiveDetectorBase {
  public:
    MuonWallSDTool(const std::string& type, const std::string& name, const IInterface *parent);

    ~MuonWallSDTool();

    virtual StatusCode SetupEvent() override final;

    virtual StatusCode Gather() override final;

  protected:
    // Make me an SD!
    G4VSensitiveDetector* makeSD() override final;
};

#endif // MUONWALL_MUONWALLSDTOOL_H
