/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSTATIONINTERSECTCONDALG_H
#define MUONSTATIONINTERSECTCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include "MuonStationIntersectCond/MuonIntersectGeoData.h"
#include "MuonCondData/MdtCondDbData.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

class MuonStationIntersectCondAlg: public AthAlgorithm{
public:

MuonStationIntersectCondAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MuonStationIntersectCondAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

private:
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    SG::WriteCondHandleKey<Muon::MuonIntersectGeoData> m_writeKey{this, "WriteKey", "MuonStationIntersects", "Key of output MDT intersection"};

    SG::ReadCondHandleKey<MdtCondDbData> m_condKey{this,  "MdtCondKey", "MdtCondDbData","Key of MdtCondDbData"};

    SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey{this, "DetectorManagerKey", "MuonDetectorManager", "Key of input MuonDetectorManager condition data"};



};


#endif