/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GenericMuonSensitiveDetectorTool_H
#define GenericMuonSensitiveDetectorTool_H

#include "G4AtlasTools/SensitiveDetectorBase.h"

class GenericMuonSensitiveDetectorTool : public SensitiveDetectorBase {

public:
    GenericMuonSensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface *parent);
    ~GenericMuonSensitiveDetectorTool() {}
protected:
    G4VSensitiveDetector* makeSD() override final;
};

#endif
