/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MicromegasSensitiveDetectorTool_H
#define MicromegasSensitiveDetectorTool_H

#include "G4AtlasTools/SensitiveDetectorBase.h"

class MicromegasSensitiveDetectorTool : public SensitiveDetectorBase {

public:
    /** construction/destruction */
    MicromegasSensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface *parent);
    ~MicromegasSensitiveDetectorTool() {}
protected:
    G4VSensitiveDetector* makeSD() override final;
};

#endif
