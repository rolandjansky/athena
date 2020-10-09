/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTSensitiveDetectorCosmicsTool_H
#define MDTSensitiveDetectorCosmicsTool_H

#include "G4AtlasTools/SensitiveDetectorBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

class MDTSensitiveDetectorCosmicsTool : public SensitiveDetectorBase {

public:
    MDTSensitiveDetectorCosmicsTool(const std::string& type, const std::string& name, const IInterface *parent);
    ~MDTSensitiveDetectorCosmicsTool()=default;
protected:
    G4VSensitiveDetector* makeSD() override final;
private:
    ToolHandle<Muon::MuonIdHelperTool> m_idHelperTool;
};

#endif
