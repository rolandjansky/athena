/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTSimHitVariables_H
#define MDTSimHitVariables_H

#include "MuonPRDTest/PrdTesterModule.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
namespace MuonPRDTest {
    class MDTSimHitVariables : public PrdTesterModule {
    public:
        MDTSimHitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl);

        ~MDTSimHitVariables() = default;

        bool fill(const EventContext& ctx) override final;

        bool declare_keys() override final;

    private:
        SG::ReadHandleKey<MDTSimHitCollection> m_simHitKey{};

        MdtIdentifierBranch m_MDT_id{parent(), "MDT_Sim"};
        VectorBranch<float>& m_MDT_globalTime{parent().newVector<float>("MDT_Sim_globalTime")};
        ThreeVectorBranch m_MDT_hitLocalPosition{parent(), "MDT_Sim_hitLocalPosition"};
        ThreeVectorBranch m_MDT_hitGlobalPosition{parent(), "MDT_Sim_hitGlobalPosition"};
        ThreeVectorBranch m_MDT_detector_globalPosition{parent(), "MDT_Sim_detGlobalPosition"};
        VectorBranch<float>& m_MDT_driftRadius{parent().newVector<float>("MDT_Sim_driftRadius")};

        VectorBranch<int>& m_MDT_particleEncoding{parent().newVector<int>("MDT_Sim_particleEncoding")};
        VectorBranch<float>& m_MDT_kineticEnergy{parent().newVector<float>("MDT_Sim_kineticEnergy")};
        VectorBranch<float>& m_MDT_depositEnergy{parent().newVector<float>("MDT_Sim_depositEnergy")};
        VectorBranch<float>& m_MDT_StepLength{parent().newVector<float>("MDT_Sim_StepLength")};
        VectorBranch<int>& m_MDT_trackId{parent().newVector<int>("MDT_Sim_trackId")};
        VectorBranch<int>& m_MDT_truthEl{parent().newVector<int>("MDT_Sim_truthEl")};
        ScalarBranch<unsigned int>& m_MDT_nSimHits{parent().newScalar<unsigned int>("MDT_Sim_nHits")};
    };
}  // namespace MuonPRDTest
#endif  // MDTSimHitVariables_H
