/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCSensitiveDetectorTool_H
#define RPCSensitiveDetectorTool_H

#include "G4AtlasTools/SensitiveDetectorBase.h"

class RPCSensitiveDetectorTool : public SensitiveDetectorBase {

public:
    /** construction/destruction */
    RPCSensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface *parent);
    ~RPCSensitiveDetectorTool() {}
protected:
    G4VSensitiveDetector* makeSD() override final;
};

#endif
