/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPRDTEST_RPCSDOVARIABLES_H
#define MUONPRDTEST_RPCSDOVARIABLES_H

#include "MuonPRDTest/PrdTesterModule.h"
#include "MuonSimData/MuonSimDataCollection.h"
namespace MuonPRDTest {
    class RpcSDOVariables : public PrdTesterModule {
    public:
        RpcSDOVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl);

        ~RpcSDOVariables() = default;

        bool fill(const EventContext& ctx) override final;

        bool declare_keys() override final;

    private:
        SG::ReadHandleKey<MuonSimDataCollection> m_key{};

        ScalarBranch<unsigned int>& m_rpc_nsdo{parent().newScalar<unsigned int>("nSDO_RPC")};
        RpcIdentifierBranch m_rpc_sdo_id{parent(), "SDO_RPC"};
        VectorBranch<int>& m_rpc_sdo_word{parent().newVector<int>("SDO_RPC_word")};
        VectorBranch<int>& m_rpc_sdo_barcode{parent().newVector<int>("SDO_RPC_barcode")};
        VectorBranch<float>& m_rpc_sdo_globaltime{parent().newVector<float>("SDO_RPC_global_time")};

        ThreeVectorBranch m_RPC_dig_globalPos{parent(), "SDO_RPC_globalPos"};
        VectorBranch<float>& m_rpc_sdo_localPosX{parent().newVector<float>("SDO_RPC_localPosX")};
        VectorBranch<float>& m_rpc_sdo_localPosY{parent().newVector<float>("SDO_RPC_localPosY")};
    };
}  // namespace MuonPRDTest
#endif  // RPCSDOVARIABLES_H
