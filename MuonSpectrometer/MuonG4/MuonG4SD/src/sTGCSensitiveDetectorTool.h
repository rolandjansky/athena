/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGCSensitiveDetectorTool_H
#define sTGCSensitiveDetectorTool_H

#include "G4AtlasTools/SensitiveDetectorBase.h"

class sTGCSensitiveDetectorTool : public SensitiveDetectorBase {

public:
    /** construction/destruction */
    sTGCSensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface *parent);
    ~sTGCSensitiveDetectorTool() {}
protected:
    G4VSensitiveDetector* makeSD() override final;
};

#endif
