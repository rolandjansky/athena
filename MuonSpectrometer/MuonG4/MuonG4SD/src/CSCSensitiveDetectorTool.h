/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSENSITIVEDETECTORTOOL_H
#define CSCSENSITIVEDETECTORTOOL_H

#include "G4AtlasTools/SensitiveDetectorBase.h"

class CSCSensitiveDetectorTool : public SensitiveDetectorBase {

public:
    CSCSensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface *parent);
    ~CSCSensitiveDetectorTool() {}
protected:
    G4VSensitiveDetector* makeSD() override final;
};

#endif
