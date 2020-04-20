/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
   */


#ifndef MUONCONDALG_RPCCABLINGCONDALG_H
#define MUONCONDALG_RPCCABLINGCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include <string>
#include <map>
#include <vector>

#include "GaudiKernel/EventIDRange.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/TransientAddress.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "PathResolver/PathResolver.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>

#include "RPC_CondCabling/RPCofflineId.h"

#include "RPC_CondCabling/RpcCablingCondData.h"


class RpcCablingCondAlg: public AthAlgorithm {

  public:

    RpcCablingCondAlg(const std::string& name, ISvcLocator* pSvcLocator);

    virtual ~RpcCablingCondAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;

  private:

    StatusCode loadParameters(const CondAttrListCollection* readCdoMap,
                              const CondAttrListCollection* readCdoCorr,
                              const CondAttrListCollection* readCdoEta,
                              const CondAttrListCollection* readCdoPhi);
    StatusCode ReadConf();
    StatusCode buildRDOmap(RpcCablingCondData* writeCdo);
    std::list<Identifier> give_strip_id (unsigned short int SubsystemId,
                                         unsigned short int SectorId,
                                         unsigned short int PADId,
                                         unsigned short int CMAId,
                                         unsigned short ijk,
                                         unsigned short int Channel) const;

    const std::string* m_ConfMapPString;
    const std::string* m_corr;
    std::vector<std::string> m_vecetaCM_File;
    std::vector<std::string> m_vecetaTh0;
    std::vector<std::string> m_vecetaSequence_Th;
    std::vector<std::string> m_vecetaInfo;

    std::map<std::string, std::string> m_trigroads;

    std::vector<std::string> m_vecphiCM_File;
    std::vector<std::string> m_vecphiTh0;
    std::vector<std::string> m_vecphiInfo;

    std::string m_DataName; 
    bool m_TestbeamFlag; 
    bool m_cosmic_configuration;

    int m_MaxType; 
    int m_SectorMap[64]; 
    std::string m_Version; 

    typedef std::map < int,RPC_CondCabling::SectorLogicSetup*,std::less < int > > SLmap;

    // array; for each sectorlogic type returns the SectorLogicSetup
    RpcCablingCondData::STvec m_SectorType;
    //map; for each sectorlogic number returns the SectorLogicSetup
    SLmap m_SectorLogic;

    // BooleanProperty m_RPCTriggerRoadsfromCool;
    
    // Pad parameters
    BooleanProperty m_ApplyFeetPadThresholds;
    BooleanProperty m_ForceFeetPadThresholdsFromJO;
    std::vector<unsigned short int> m_FeetPadThresholds;

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_map_schema{this, "ReadKeySchema", "/RPC/CABLING/MAP_SCHEMA", "Key of input muon rpc map schema condition data"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_map_schema_corr{this, "ReadKeySchemaCorr", "/RPC/CABLING/MAP_SCHEMA_CORR", "Key of input muon rpc map correction schema condition data"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_cm_thr_eta{this, "ReadKeyThrEta", "/RPC/TRIGGER/CM_THR_ETA", "Key of input muon rpc trigger eta condition data"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_cm_thr_phi{this, "ReadKeyThrPhi", "/RPC/TRIGGER/CM_THR_PHI", "Key of input muon rpc trigger phi condition data"};

    SG::WriteCondHandleKey<RpcCablingCondData>  m_writeKey{this, "WriteKey", "RpcCablingCondData", "Key of output RPC cabling condition data"};

    ServiceHandle<ICondSvc> m_condSvc;

};

#endif
