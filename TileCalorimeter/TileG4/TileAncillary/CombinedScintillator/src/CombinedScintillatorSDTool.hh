/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//**************************************************************
//
// Class CombinedScintillatorSDTool
//
// Tool for configuring the sensitive detector for the Scintillator between LAR & Tile
//
//**************************************************************

#ifndef COMBINEDSCINTILLATOR_COMBINEDSCINTILLATORSDTOOL_H
#define COMBINEDSCINTILLATOR_COMBINEDSCINTILLATORSDTOOL_H

// Base class header
#include "G4AtlasTools/SensitiveDetectorBase.h"

//STL headers
#include <string>

class G4VSensitiveDetector;

class CombinedScintillatorSDTool: public SensitiveDetectorBase {
  public:
    CombinedScintillatorSDTool(const std::string& type, const std::string& name, const IInterface *parent);

    ~CombinedScintillatorSDTool();

    virtual StatusCode SetupEvent() override final;

    virtual StatusCode Gather() override final;

  protected:
    // Make me an SD!
    G4VSensitiveDetector* makeSD() override final;
};

#endif // COMBINEDSCINTILLATOR_COMBINEDSCINTILLATORSDTOOL_H
