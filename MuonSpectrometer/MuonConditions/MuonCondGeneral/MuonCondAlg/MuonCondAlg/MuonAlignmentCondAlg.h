/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDALG_MUONALIGNMENTCONDALG_H
#define MUONCONDALG_MUONALIGNMENTCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Blob.h"
#include "StoreGate/CondHandleKeyArray.h"
#include "StoreGate/ReadCondHandleKey.h"
// typedefs for A/BLineMapContainer
#include "CoralUtilities/blobaccess.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonAlignmentData/CorrContainer.h"
#include "MuonAlignmentData/NswAsBuiltDbData.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "nlohmann/json.hpp"

class StoreGateSvc;
class StatusCode;

namespace MuonGM {
    class MuonDetectorManager;
}

class MuonAlignmentCondAlg : public AthAlgorithm {
public:
    MuonAlignmentCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~MuonAlignmentCondAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;


private:
    // Read Handles
    SG::ReadCondHandleKey<CondAttrListCollection> m_readMdtBarrelKey{this, "ReadMdtBarrelKey", "/MUONALIGN/MDT/BARREL",
                                                                     "Key of input MDT/BARREL condition data"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readMdtEndcapSideAKey{this, "ReadMdtEndcapSideAKey", "/MUONALIGN/MDT/ENDCAP/SIDEA",
                                                                          "Key of input MDT/ENDCAP/SIDEA condition data"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readMdtEndcapSideCKey{this, "ReadMdtEndcapSideCKey", "/MUONALIGN/MDT/ENDCAP/SIDEC",
                                                                          "Key of input MDT/ENDCAP/SIDEC condition data"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readTgcSideAKey{this, "ReadTgcSideAKey", "/MUONALIGN/TGC/SIDEA",
                                                                    "Key of input TGC/SIDEA condition data"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readTgcSideCKey{this, "ReadTgcSideCKey", "/MUONALIGN/TGC/SIDEC",
                                                                    "Key of input TGC/SIDEC condition data"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readCscILinesKey{this, "ReadCscILinesKey", "/MUONALIGN/CSC/ILINES",
                                                                     "Key of input CSC/ILINES condition data"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readMdtAsBuiltParamsKey{this, "ReadMdtAsBuiltParamsKey", "/MUONALIGN/MDT/ASBUILTPARAMS",
                                                                            "Key of MDT/ASBUILTPARAMS input condition data"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readMmAsBuiltParamsKey{this, "ReadMmAsBuiltParamsKey", "/MUONALIGN/ASBUILTPARAMS/MM",
                                                                            "Key of MM/ASBUILTPARAMS input condition data"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readSTgcAsBuiltParamsKey{this, "ReadSTgcAsBuiltParamsKey", "/MUONALIGN/ASBUILTPARAMS/STGC",
                                                                            "Key of sTGC/ASBUILTPARAMS input condition data"};

    // Write Handles
    SG::WriteCondHandleKey<ALineMapContainer> m_writeALineKey{this, "WriteALineKey", "ALineMapContainer",
                                                              "Key of output muon alignment ALine condition data"};
    SG::WriteCondHandleKey<BLineMapContainer> m_writeBLineKey{this, "WriteBLineKey", "BLineMapContainer",
                                                              "Key of output muon alignment BLine condition data"};
    SG::WriteCondHandleKey<CscInternalAlignmentMapContainer> m_writeILineKey{this, "WriteILineKey", "CscInternalAlignmentMapContainer",
                                                                             "Key of output muon alignment CSC/ILine condition data"};
    SG::WriteCondHandleKey<MdtAsBuiltMapContainer> m_writeMdtAsBuiltKey{this, "WriteMdtAsBuiltKey", "MdtAsBuiltMapContainer",
                                                                     "Key of output muon alignment MDT/AsBuilt condition data"};
    SG::WriteCondHandleKey<NswAsBuiltDbData> m_writeNswAsBuiltKey{this, "WriteNswAsBuiltKey", "NswAsBuiltDbData",
                                                                     "Key of output muon alignment MM+STGC/AsBuilt condition data"};

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    const MuonGM::MuonDetectorManager* m_muonDetMgrDS{nullptr};
    std::string m_geometryVersion{""};
   
    Gaudi::Property<std::vector<std::string>> m_parlineFolder{this, "ParlineFolders", {}, "Database folders"};
                                                              
    Gaudi::Property<bool> m_isData{this, "IsData", true};

  
    Gaudi::Property<bool> m_dumpALines{this, "DumpALines", false};
    Gaudi::Property<bool> m_dumpBLines{this, "DumpBLines", false};
    Gaudi::Property<bool> m_dumpILines{this, "DumpILines", false};
    Gaudi::Property<bool> m_ILinesFromDb{this, "ILinesFromCondDB", false};

    bool m_MdtAsBuiltRequested {false};
    bool m_NswAsBuiltRequested {false};
    bool m_ILineRequested {false};
    Gaudi::Property<std::string> m_aLinesFile{this, "ALinesFile", "" };
    Gaudi::Property<std::string> m_asBuiltFile{this, "AsBuiltFile", ""};

    // new folder format 2020
    Gaudi::Property<bool> m_newFormat2020 {this, "NewFormat2020", false};

    StatusCode loadParameters();
    StatusCode loadAlignABLines();
    StatusCode loadAlignABLines(const std::string& folderName, ALineMapContainer* writeALineCdo, BLineMapContainer* writeBLineCdo,
                                EventIDRange& rangeALineW, EventIDRange& rangeBLineW);
    StatusCode loadAlignABLinesData(const std::string& folderName, const std::string& data, nlohmann::json& json, bool);
    StatusCode loadAlignILines(const std::string& folderName);
    StatusCode loadAlignILinesData(const std::string& folderName, const std::string& data, nlohmann::json& json);
    StatusCode loadMdtAlignAsBuilt(const std::string& folderName);
    StatusCode loadNswAlignAsBuilt(const std::string& mmFolderName, const std::string& sTgcFolderName);

    void setALinesFromAscii(ALineMapContainer* writeALineCdo) const;
    void setAsBuiltFromAscii(MdtAsBuiltMapContainer* writeCdo) const;
    void dumpALines(const std::string& folderName, ALineMapContainer* writeALineCdo);
    void dumpBLines(const std::string& folderName, BLineMapContainer* writeBLineCdo);
    void dumpILines(const std::string& folderName, CscInternalAlignmentMapContainer* writeCdo);
};

#endif
