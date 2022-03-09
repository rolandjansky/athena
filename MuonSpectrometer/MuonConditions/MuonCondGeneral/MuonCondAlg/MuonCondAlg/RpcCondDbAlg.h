/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDALG_RPCCONDDBALG_H
#define MUONCONDALG_RPCCONDDBALG_H

// STL includes
#include <zlib.h>

#include <sstream>
#include <string>
#include <vector>

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"

// Athena includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "MuonCondData/RpcCondDbData.h"
#include "MuonCondSvc/MdtStringUtils.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

class RpcCondDbAlg : public AthReentrantAlgorithm {
public:
    RpcCondDbAlg(const std::string &name, ISvcLocator *svc);
    virtual ~RpcCondDbAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext &) const override;

private:
    StatusCode loadDataDeadPanels(EventIDRange &, RpcCondDbData *, const EventContext &) const;
    StatusCode loadDataOffPanels(EventIDRange &, RpcCondDbData *, const EventContext &) const;
    StatusCode loadMcElementStatus(EventIDRange &, RpcCondDbData *, const EventContext &) const;

    bool m_isOnline{false};
    bool m_isData{false};
    bool m_isRun1{false};

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    SG::WriteCondHandleKey<RpcCondDbData> m_writeKey{this, "WriteKey", "RpcCondDbData", "Key of output RPC condition data"};

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_deadPanels{this, "ReadKey_DA_DP", "/RPC/DCS/DeadRopanels",
                                                                                 "Key of input RPC condition data for data dead panels"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_offPanels{this, "ReadKey_DA_OP", "/RPC/DCS/OffRopanels",
                                                                                "Key of input RPC condition data for data off panels"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_mc_deadElements{this, "ReadKey_MC_DE", "/RPC/DQMF/ELEMENT_STATUS",
                                                                                   "Key of input RPC condition data for MC dead elements"};

    float m_panelEfficiency;
};

#endif
