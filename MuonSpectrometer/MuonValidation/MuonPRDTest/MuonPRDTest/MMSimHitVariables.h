/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMSimHitVariables_H
#define MMSimHitVariables_H

#include "MuonPRDTest/PrdTesterModule.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonSimEvent/MMSimHitCollection.h"

namespace MuonPRDTest{
    class MMSimHitVariables : public PrdTesterModule {
    public:
        MMSimHitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl);
    
        bool fill(const EventContext& ctx) override final;
        bool declare_keys() override final;
        
    private:   
        SG::ReadHandleKey<MMSimHitCollection> m_key{};
        ThreeVectorBranch m_NSWMM_hitGlobalPosition{parent(), "Hits_MM_GlobalPosition"};
        ThreeVectorBranch m_NSWMM_hitGlobalDirection{parent(), "Hits_MM_GlobalDirection"};
        VectorBranch<int>& m_NSWMM_particleEncoding{parent().newVector<int>("Hits_MM_particleEncoding")};
        VectorBranch<float>& m_NSWMM_kineticEnergy{parent().newVector<float>("Hits_MM_kineticEnergy")};
        VectorBranch<float>& m_NSWMM_depositEnergy{parent().newVector<float>("Hits_MM_depositEnergy")};
        VectorBranch<bool>& m_NSWMM_isInsideBounds{parent().newVector<bool>("Hits_MM_isInsideBounds")};
        VectorBranch<int>& m_NSWMM_trackId{parent().newVector<int>("Hits_MM_trackId")};
        ScalarBranch<unsigned int>& m_NSWMM_nSimHits{parent().newScalar<unsigned int>("Hits_MM_nHits")};
        MmIdentifierBranch m_NSWMM_Id{parent(), "Hits_MM_off"};
        VectorBranch<float>& m_NSWMM_globalTime{parent().newVector<float>("Hits_MM_globalTime")};
        ThreeVectorBranch m_NSWMM_detector_globalPosition{parent(), "Hits_MM_detector_globalPosition"};
        ThreeVectorBranch m_NSWMM_hitToDsurfacePosition{parent(), "Hits_MM_hitToDsurfacePosition"};
        ThreeVectorBranch m_NSWMM_hitToRsurfacePosition{parent(), "Hits_MM_hitToRsurfacePosition"};
        VectorBranch<float>& m_NSWMM_FastDigitRsurfacePositionX{parent().newVector<float>("Hits_MM_FastDigitRsurfacePositionX")};
        VectorBranch<float>& m_NSWMM_FastDigitRsurfacePositionY{parent().newVector<float>("Hits_MM_FastDigitRsurfacePositionY")};
    };
};

#endif  // MMSimHitVariables_H