/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONGEOMODEL_MUONDETECTORCONDALG_H
#define MUONGEOMODEL_MUONDETECTORCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonAlignmentData/CorrContainer.h"
#include "MuonAlignmentData/NswAsBuiltDbData.h"
#include "MuonGeoModel/MuonDetectorTool.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/NswPassivationDbData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

class MuonDetectorCondAlg : public AthAlgorithm {

  public:
    // Standard Constructor
    MuonDetectorCondAlg(const std::string &name, ISvcLocator *pSvcLocator);

    // Standard Destructor
    virtual ~MuonDetectorCondAlg() = default;

    virtual StatusCode initialize() override final;
    virtual StatusCode execute() override final;

    Gaudi::Property<bool> m_isData{this, "IsData", true};
    Gaudi::Property<bool> m_applyMmPassivation{this, "applyMmPassivation", false};
    Gaudi::Property<bool> m_applyALines{this, "applyALines", true};
    Gaudi::Property<bool> m_applyBLines{this, "applyBLines", true};

  private:
    ToolHandle<MuonDetectorTool> m_iGeoModelTool{this, "MuonDetectorTool", "MuonDetectorTool", "The MuonDetector tool"};
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    // Read Handles
    SG::ReadCondHandleKey<ALineMapContainer> m_readALineKey{this, "ReadALineKey", "ALineMapContainer", "Key of input muon alignment ALine condition data"};
    SG::ReadCondHandleKey<BLineMapContainer> m_readBLineKey{this, "ReadBLineKey", "BLineMapContainer", "Key of input muon alignment BLine condition data"};
    SG::ReadCondHandleKey<CscInternalAlignmentMapContainer> m_readILineKey{this, "ReadILineKey", "CscInternalAlignmentMapContainer",
                                                                           "Key of input muon alignment CSC/ILine condition data"};
    SG::ReadCondHandleKey<MdtAsBuiltMapContainer> m_readMdtAsBuiltKey{this, "ReadMdtAsBuiltKey", "MdtAsBuiltMapContainer", "Key of output muon alignment MDT/AsBuilt condition data"};
    SG::ReadCondHandleKey<NswAsBuiltDbData> m_readNswAsBuiltKey{this, "ReadNswAsBuiltKey", "NswAsBuiltDbData", "Key of NswAsBuiltDbData object containing conditions data for NSW as-built params!"};
    SG::ReadCondHandleKey<NswPassivationDbData> m_condMmPassivKey {this, "condMmPassivKey", "NswPassivationDbData", "Key of NswPassivationDbData object containing passivation data for MMs"};

    // Write Handle
    SG::WriteCondHandleKey<MuonGM::MuonDetectorManager> m_writeDetectorManagerKey{this, "WriteDetectorManagerKey", "MuonDetectorManager",
                                                                                  "Key of output MuonDetectorManager condition data"};
};

#endif
