/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCSensitiveDetectorCosmicsTool_H
#define RPCSensitiveDetectorCosmicsTool_H

#include "G4AtlasTools/SensitiveDetectorBase.h"

class RPCSensitiveDetectorCosmicsTool : public SensitiveDetectorBase {

public:
    /** construction/destruction */
    RPCSensitiveDetectorCosmicsTool(const std::string& type, const std::string& name, const IInterface *parent);
    ~RPCSensitiveDetectorCosmicsTool() {}
protected:
    G4VSensitiveDetector* makeSD() override final;
};

#endif
