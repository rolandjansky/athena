/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSimHitVariables_H
#define CSCSimHitVariables_H

#include "MuonPRDTest/PrdTesterModule.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
namespace MuonPRDTest{
    class CSCSimHitVariables : public PrdTesterModule {
    public:
        CSCSimHitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl);
    
        bool fill(const EventContext& ctx) override final;
        bool declare_keys() override final;
    
    private:
        SG::ReadHandleKey<CSCSimHitCollection> m_key{};
        
    
        CscIdentifierBranch m_CSC_id{parent(), "CSC_Sim"};
        VectorBranch<float>& m_CSC_globalTime{parent().newVector<float>("CSC_Sim_globalTime")};
        ThreeVectorBranch m_CSC_hitGlobalPosition{parent(), "CSC_Sim_hitGlobalPosition"};
        ThreeVectorBranch m_CSC_detector_globalPosition{parent(), "CSC_Sim_detGlobalPosition"};
    
        VectorBranch<float>& m_CSC_kineticEnergy{parent().newVector<float>("CSC_Sim_kineticEnergy")};
        VectorBranch<float>& m_CSC_depositEnergy{parent().newVector<float>("CSC_Sim_depositEnergy")};
        VectorBranch<int>& m_CSC_trackId{parent().newVector<int>("CSC_Sim_trackId")};
        VectorBranch<int>& m_CSC_truthEl{parent().newVector<int>("CSC_Sim_truthEl")};
        ScalarBranch<unsigned int>& m_CSC_nSimHits{parent().newScalar<unsigned int>("CSC_Sim_nHits")};
    };
};
#endif  // CSCSimHitVariables_H