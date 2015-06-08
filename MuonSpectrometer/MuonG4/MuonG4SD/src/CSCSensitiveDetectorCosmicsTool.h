/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSensitiveDetectorCosmicsTool_H
#define CSCSensitiveDetectorCosmicsTool_H

#include "G4AtlasTools/SensitiveDetectorBase.h"

class CSCSensitiveDetectorCosmicsTool : public SensitiveDetectorBase {

public:
    /** construction/destruction */
    CSCSensitiveDetectorCosmicsTool(const std::string& type, const std::string& name, const IInterface *parent);
    ~CSCSensitiveDetectorCosmicsTool() {}
protected:
    G4VSensitiveDetector* makeSD() override final;
};

#endif
