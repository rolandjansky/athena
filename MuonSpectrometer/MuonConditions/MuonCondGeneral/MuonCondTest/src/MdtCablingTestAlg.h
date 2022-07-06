/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
   Algorithm to test the validity of the MDT cabling
*/

#ifndef MUONMDT_CABLING_MDTCABLINGTESTALG_H
#define MUONMDT_CABLING_MDTCABLINGTESTALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"



class MdtCablingTestAlg : public AthAlgorithm {
public:
    MdtCablingTestAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~MdtCablingTestAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual unsigned int cardinality() const override final{return 1;}

    using CablingData = MuonMDT_CablingMap::CablingData;

private:
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    // MuonDetectorManager from the conditions store
    SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey{this, "DetectorManagerKey", "MuonDetectorManager",
                                                                                "Key of input MuonDetectorManager condition data"};

    SG::ReadCondHandleKey<MuonMDT_CablingMap> m_cablingKey{this, "WriteKey", "MuonMDT_CablingMap", "Key of input MDT cabling map"};
    
};

#endif
