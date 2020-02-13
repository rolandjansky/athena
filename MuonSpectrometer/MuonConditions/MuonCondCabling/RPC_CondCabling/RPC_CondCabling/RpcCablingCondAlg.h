/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */


#ifndef MUONCONDALG_RPCCABLINGCONDALG_H
#define MUONCONDALG_RPCCABLINGCONDALG_H

#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "AthenaBaseComps/AthAlgorithm.h"
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
#include "Identifier/IdentifierHash.h"
#include "PathResolver/PathResolver.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "Identifier/Identifier.h"
#include "MuonIdHelpers/RpcIdHelper.h"

#include "RPC_CondCabling/RPCofflineId.h"

#include "RPC_CondCabling/RpcCablingCondData.h"


class RpcCablingCondAlg: public AthAlgorithm {

  public:

    RpcCablingCondAlg(const std::string& name, ISvcLocator* pSvcLocator);

    virtual ~RpcCablingCondAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;

  private:

    StatusCode loadParameters(const CondAttrListCollection* readCdoMap);
    StatusCode ReadConf();
    StatusCode buildRDOmap(EventIDRange range_map_schema);
    std::list<Identifier> give_strip_id (unsigned short int SubsystemId,
                                         unsigned short int SectorId,
                                         unsigned short int PADId,
                                         unsigned short int CMAId,
                                         unsigned short ijk,
                                         unsigned short int Channel) const;


    const std::string* m_ConfMapPString;

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



    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_map_schema{this, "ReadKey", "/RPC/CABLING/MAP_SCHEMA", "Key of input muon rpc map schema condition data"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_map_schema_corr{this, "ReadKey", "/RPC/CABLING/MAP_SCHEMA_CORR", "Key of input muon rpc map correction schema condition data"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_cm_thr_eta{this, "ReadKey", "/RPC/TRIGGER/CM_THR_ETA", "Key of input muon rpc trigger eta condition data"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_cm_thr_phi{this, "ReadKey", "/RPC/TRIGGER/CM_THR_PHI", "Key of input muon rpc trigger phi condition data"};

    SG::WriteCondHandleKey<RpcCablingCondData>  m_writeKey{this, "WriteKey", "RpcCablingCondData", "Key of output RPC cabling condition data"};

    ServiceHandle<ICondSvc> m_condSvc;

};

#endif
