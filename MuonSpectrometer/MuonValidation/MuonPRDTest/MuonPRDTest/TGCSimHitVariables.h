/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSimHitVariables_H
#define TGCSimHitVariables_H

#include "MuonPRDTest/PrdTesterModule.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
namespace MuonPRDTest{
    class TGCSimHitVariables : public PrdTesterModule {
    public:
        TGCSimHitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl);
    
        bool fill(const EventContext& ctx) override final;
        bool declare_keys() override final;
    
    private:
        SG::ReadHandleKey<TGCSimHitCollection> m_key{};
        
    
        TgcIdentifierBranch m_TGC_id{parent(), "TGC_Sim"};
        VectorBranch<float>& m_TGC_globalTime{parent().newVector<float>("TGC_Sim_globalTime")};
        
        ThreeVectorBranch m_TGC_hitLocalPosition{parent(),"TGC_Sim_hitLocalPosition"} ;
        ThreeVectorBranch m_TGC_hitGlobalPosition{parent(), "TGC_Sim_hitGlobalPosition"};
        ThreeVectorBranch m_TGC_detector_globalPosition{parent(), "TGC_Sim_detGlobalPosition"};
    
        VectorBranch<int>& m_TGC_particleEncoding{parent().newVector<int>("TGC_Sim_particleEncoding")};
        VectorBranch<float>& m_TGC_kineticEnergy{parent().newVector<float>("TGC_Sim_kineticEnergy")};
        VectorBranch<float>& m_TGC_depositEnergy{parent().newVector<float>("TGC_Sim_depositEnergy")};
        VectorBranch<float>& m_TGC_StepLength{parent().newVector<float>("TGC_Sim_StepLength")};
        VectorBranch<int>& m_TGC_trackId{parent().newVector<int>("TGC_Sim_trackId")};
        VectorBranch<int>& m_TGC_truthEl{parent().newVector<int>("TGC_Sim_truthEl")};
        ScalarBranch<unsigned int>& m_TGC_nSimHits{parent().newScalar<unsigned int>("TGC_Sim_nHits")};
    };
};
#endif  // TGCSimHitVariables_H