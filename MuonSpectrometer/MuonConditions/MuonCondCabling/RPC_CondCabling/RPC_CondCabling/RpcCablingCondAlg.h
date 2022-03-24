/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#ifndef MUONCONDALG_RPCCABLINGCONDALG_H
#define MUONCONDALG_RPCCABLINGCONDALG_H

#include <array>
#include <map>
#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "RPC_CondCabling/RpcCablingCondData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

class RpcCablingCondAlg : public AthAlgorithm {
public:
    RpcCablingCondAlg(const std::string& name, ISvcLocator* pSvcLocator);

    virtual ~RpcCablingCondAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;

private:
    typedef std::array<int, 64> sectorMap_t;
    typedef std::map<int, RPC_CondCabling::SectorLogicSetup*, std::less<int>> SLmap_t;

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_map_schema{this, "ReadKeySchema", "/RPC/CABLING/MAP_SCHEMA",
                                                                       "Key of input muon rpc map schema condition data"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_map_schema_corr{this, "ReadKeySchemaCorr", "/RPC/CABLING/MAP_SCHEMA_CORR",
                                                                            "Key of input muon rpc map correction schema condition data"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_cm_thr_eta{this, "ReadKeyThrEta", "/RPC/TRIGGER/CM_THR_ETA",
                                                                       "Key of input muon rpc trigger eta condition data"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_cm_thr_phi{this, "ReadKeyThrPhi", "/RPC/TRIGGER/CM_THR_PHI",
                                                                       "Key of input muon rpc trigger phi condition data"};

    SG::WriteCondHandleKey<RpcCablingCondData> m_writeKey{this, "WriteKey", "RpcCablingCondData",
                                                          "Key of output RPC cabling condition data"};

    Gaudi::Property<std::string> m_database_repository{this, "DatabaseRepository", "MuonRPC_Cabling/ATLAS.data"};
    Gaudi::Property<bool> m_cosmic_configuration{this, "CosmicConfiguration", false};
    Gaudi::Property<bool> m_ApplyFeetPadThresholds{this, "ApplyFeetPadThresholds", true,
                                                   "map 3 low pt thresholds from special feet pads on standard 6 (3low+3high)"};
    Gaudi::Property<bool> m_ForceFeetPadThresholdsFromJO{this, "ForceFeetPadThresholdsFromJO", false, "JO override db setting"};

    StatusCode setup(const CondAttrListCollection* readCdoMap, const CondAttrListCollection* readCdoCorr,
                     const CondAttrListCollection* readCdoEta, const CondAttrListCollection* readCdoPhi,
                     RpcCablingCondData* writeCdo) const;

    std::list<Identifier> give_strip_id(const unsigned short int SubsystemId, const unsigned short int SectorId,
                                        const unsigned short int PADId, const unsigned short int CMAId, const unsigned short ijk,
                                        const unsigned short int Channel, const sectorMap_t& smap,
                                        const RpcCablingCondData::STvec& sType) const;
    bool BoardParamCheck(const unsigned short int SubId, const unsigned short int SecId, const unsigned short int PADId,
                         const unsigned short int CMAId, const unsigned short int inputType, const unsigned int layer,
                         const unsigned int type, const unsigned short int Channel1, const unsigned short int Channel2,
                         const short int Number) const;
    bool CableParamCheck(const unsigned short int SubId, const unsigned short int SecId, const unsigned short int PADId,
                         const unsigned short int CMAId, const unsigned short int ijk, const unsigned int type,
                         const unsigned short int Channel1, const unsigned short int Channel2, const short int Number) const;
    static bool correct(const unsigned short int SubsystemId, const unsigned short int SectorId, const unsigned short int PADId,
                 const unsigned short int CMAId, const CMAinput it, const unsigned int layer, const unsigned short int Channel1,
                 const unsigned short int Channel2, const short int number, const L1RPCcabCorrection type, const sectorMap_t& smap,
                 const RpcCablingCondData::STvec& sType) ;
};

#endif  // MUONCONDALG_RPCCABLINGCONDALG_H
