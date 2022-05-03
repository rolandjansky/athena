/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonPRDTEST_RPCDigitVARIABLES_H
#define MuonPRDTEST_RPCDigitVARIABLES_H

#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonPRDTest/PrdTesterModule.h"
namespace MuonPRDTest {
    class RpcDigitVariables : public PrdTesterModule {
    public:
        RpcDigitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl);

        ~RpcDigitVariables() = default;

        bool fill(const EventContext& ctx) override final;

        bool declare_keys() override final;

    private:
        SG::ReadHandleKey<RpcDigitContainer> m_key{};
        ScalarBranch<unsigned int>& m_RPC_nDigits{parent().newScalar<unsigned int>("N_Digits_RPC")};
        VectorBranch<float>& m_RPC_dig_localPosX{parent().newVector<float>("Digits_RPC_localPosX")};
        VectorBranch<float>& m_RPC_dig_localPosY{parent().newVector<float>("Digits_RPC_localPosY")};
        VectorBranch<float>& m_RPC_dig_time{parent().newVector<float>("Digits_RPC_time")};
        VectorBranch<int>& m_RPC_dig_stripNumber{parent().newVector<int>("Digits_RPC_stripNumber")};
        ThreeVectorBranch m_RPC_dig_globalPos{parent(), "Digits_RPC_globalPos"};
        RpcIdentifierBranch m_RPC_dig_id{parent(), "Digits_RPC"};
    };
}  // namespace MuonPRDTest
#endif  // MuonPRDTEST_RPCDigitVARIABLES_H
