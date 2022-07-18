/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDTest/RPCSDOVariables.h"
namespace MuonPRDTest {
    RpcSDOVariables::RpcSDOVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "SDO_RPC", false, msglvl), m_key{container_name} {}

    bool RpcSDOVariables::declare_keys() { return declare_dependency(m_key); }

    bool RpcSDOVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fill RpcSDOVariable()");
        SG::ReadHandle<MuonSimDataCollection> rpcSdoContainer{m_key, ctx};
        if (!rpcSdoContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrive digit container " << m_key.fullKey());
            return false;
        }
        unsigned int n_sdo{0};
        for (const auto& coll : *rpcSdoContainer) {
            const Identifier& id = coll.first;
            const MuonSimData& rpc_sdo = coll.second;
            m_rpc_sdo_id.push_back(id);
            m_rpc_sdo_globaltime.push_back(rpc_sdo.getTime());
            m_rpc_sdo_word.push_back(rpc_sdo.word());

            ATH_MSG_DEBUG("RPC SDO: " << idHelperSvc()->toString(id));

            ATH_MSG_DEBUG("Get the truth deposits from the SDO.");
            std::vector<MuonSimData::Deposit> deposits;
            rpc_sdo.deposits(deposits);
            m_RPC_dig_globalPos.push_back(rpc_sdo.globalPosition());

            // use the information of the first deposit
            int barcode = deposits[0].first.barcode();
            double MuonMCdata_firstentry = deposits[0].second.firstEntry();
            double MuonMCdata_secondentry = deposits[0].second.secondEntry();

            ATH_MSG_DEBUG("RPC SDO barcode=" << barcode);
            ATH_MSG_DEBUG("RPC SDO localPosX=" << std::setw(9) << std::setprecision(2) << MuonMCdata_firstentry
                                               << ", localPosY=" << std::setw(9) << std::setprecision(2) << MuonMCdata_secondentry);

            m_rpc_sdo_barcode.push_back(barcode);
            m_rpc_sdo_localPosX.push_back(MuonMCdata_firstentry);
            m_rpc_sdo_localPosY.push_back(MuonMCdata_secondentry);
            ++n_sdo;
        }
        m_rpc_nsdo = n_sdo;

        ATH_MSG_DEBUG("Processed " << n_sdo << " RPC SDOs");
        return true;
    }
}  // namespace MuonPRDTest