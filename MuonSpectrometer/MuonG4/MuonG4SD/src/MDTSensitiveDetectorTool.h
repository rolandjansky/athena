/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTSENSITIVEDETECTORTOOL_H
#define MDTSENSITIVEDETECTORTOOL_H

#include "G4AtlasTools/SensitiveDetectorBase.h"

class MDTSensitiveDetectorTool : public SensitiveDetectorBase {

public:
    MDTSensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface *parent);
    ~MDTSensitiveDetectorTool() {}
protected:
    G4VSensitiveDetector* makeSD() override final;
};

#endif
