/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//**************************************************************
//
// Class PhantomBarrelSDTool
//
// Tool for configuring the sensitive detector for the phantom calorimeter in combined 2004
//
//**************************************************************

#ifndef PHANTOMCALORIMETER_PHANTOMBARRELSDTOOL_H
#define PHANTOMCALORIMETER_PHANTOMBARRELSDTOOL_H

// Base class header
#include "G4AtlasTools/SensitiveDetectorBase.h"

//STL headers
#include <string>

class G4VSensitiveDetector;

class PhantomBarrelSDTool: public SensitiveDetectorBase {
  public:
    PhantomBarrelSDTool(const std::string& type, const std::string& name, const IInterface *parent);

    ~PhantomBarrelSDTool();

    virtual StatusCode SetupEvent() override final;

    virtual StatusCode Gather() override final;

  protected:
    // Make me an SD!
    G4VSensitiveDetector* makeSD() override final;
};

#endif // PHANTOMCALORIMETER_PHANTOMBARRELSDTOOL_H
