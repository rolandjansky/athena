/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCSimHitVariables_H
#define RPCSimHitVariables_H

#include "MuonPRDTest/PrdTesterModule.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
namespace MuonPRDTest {
    class RPCSimHitVariables : public PrdTesterModule {
    public:
        RPCSimHitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl);

        bool fill(const EventContext& ctx) override final;
        bool declare_keys() override final;

    private:
        SG::ReadHandleKey<RPCSimHitCollection> m_key{};
        ThreeVectorBranch m_RPC_hitGlobalPosition{parent(), "RPC_SIM_GlobalPosition"};
        VectorBranch<int>& m_RPC_particleEncoding{parent().newVector<int>("RPC_SIM_particleEncoding")};
        VectorBranch<float>& m_RPC_kineticEnergy{parent().newVector<float>("RPC_SIM_kineticEnergy")};
        VectorBranch<float>& m_RPC_depositEnergy{parent().newVector<float>("RPC_SIM_depositEnergy")};
        VectorBranch<float>& m_RPC_StepLength{parent().newVector<float>("RPC_SIM_StepLength")};
        VectorBranch<int>& m_RPC_trackId{parent().newVector<int>("RPC_SIM_trackId")};
        VectorBranch<int>& m_RPC_truthEl{parent().newVector<int>("RPC_SIM_truthEl")};
        ThreeVectorBranch m_RPC_detector_globalPosition{parent(), "RPC_SIM_detector_globalPosition"};

        ScalarBranch<unsigned int>& m_RPC_nSimHits{parent().newScalar<unsigned int>("RPC_SIM_nHits")};
        RpcIdentifierBranch m_rpc_Id{parent(), "RPC_SIM"};

        VectorBranch<float>& m_RPC_globalTime{parent().newVector<float>("RPC_SIM_globalTime")};
        VectorBranch<float>& m_RPC_hitLocalPositionX{parent().newVector<float>("RPC_SIM_localPostionX")};
        VectorBranch<float>& m_RPC_hitLocalPositionY{parent().newVector<float>("RPC_SIM_localPostionY")};
    };
}  // namespace MuonPRDTest
#endif  // RPCSimHitVariables_H
