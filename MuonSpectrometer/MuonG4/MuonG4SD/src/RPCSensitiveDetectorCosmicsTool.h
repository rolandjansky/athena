/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCSensitiveDetectorCosmicsTool_H
#define RPCSensitiveDetectorCosmicsTool_H

#include "G4AtlasTools/SensitiveDetectorBase.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

class RPCSensitiveDetectorCosmicsTool : public SensitiveDetectorBase {

public:
    /** construction/destruction */
    RPCSensitiveDetectorCosmicsTool(const std::string& type, const std::string& name, const IInterface *parent);
    ~RPCSensitiveDetectorCosmicsTool()=default;
protected:
    G4VSensitiveDetector* makeSD() const override final;
private:
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
};

#endif
