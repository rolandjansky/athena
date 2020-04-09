/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCSensitiveDetectorTool_H
#define RPCSensitiveDetectorTool_H

#include "G4AtlasTools/SensitiveDetectorBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

class RPCSensitiveDetectorTool : public SensitiveDetectorBase {

public:
    /** construction/destruction */
    RPCSensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface *parent);
    ~RPCSensitiveDetectorTool()=default;
protected:
    G4VSensitiveDetector* makeSD() override final;
private:
    ToolHandle<Muon::MuonIdHelperTool> m_idHelperTool;
};

#endif
