/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGCSimHitVariables_H
#define sTGCSimHitVariables_H

#include "MuonPRDTest/PrdTesterModule.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonSimEvent/sTGCSimHitCollection.h"

namespace MuonPRDTest{
    class sTGCSimHitVariables : public PrdTesterModule {
    public:
        sTGCSimHitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl);
    
        bool fill(const EventContext& ctx) override final;
        bool declare_keys() override final;
        
    private:   
        SG::ReadHandleKey<sTGCSimHitCollection> m_key{};
        ThreeVectorBranch m_NSWsTGC_hitGlobalPosition{parent(), "Hits_sTGC_GlobalPosition"};
        ThreeVectorBranch m_NSWsTGC_hitGlobalDirection{parent(), "Hits_sTGC_GlobalDirection"};
        ThreeVectorBranch m_NSWsTGC_hitGlobalPrePosition{parent(), "Hits_sTGC_GlobalPrePosition"};
        VectorBranch<int>& m_NSWsTGC_particleEncoding{parent().newVector<int>("Hits_sTGC_particleEncoding")};
        VectorBranch<float>& m_NSWsTGC_kineticEnergy{parent().newVector<float>("Hits_sTGC_kineticEnergy")};
        VectorBranch<float>& m_NSWsTGC_depositEnergy{parent().newVector<float>("Hits_sTGC_depositEnergy")};
        VectorBranch<bool>& m_NSWsTGC_isInsideBounds{parent().newVector<bool>("Hits_sTGC_isInsideBounds")};
        VectorBranch<int>& m_NSWsTGC_trackId{parent().newVector<int>("Hits_sTGC_trackId")};
        ScalarBranch<unsigned int>& m_NSWsTGC_nSimHits{parent().newScalar<unsigned int>("Hits_sTGC_nHits")};
        sTgcIdentifierBranch m_NSWsTGC_Id{parent(), "Hits_sTGC_off"};
        VectorBranch<float>& m_NSWsTGC_globalTime{parent().newVector<float>("Hits_sTGC_globalTime")};
        VectorBranch<int>& m_NSWsTGC_detectorNumber{parent().newVector<int>("Hits_sTGC_detectorNumber")};
        VectorBranch<int>& m_NSWsTGC_wedgeId{parent().newVector<int>("Hits_sTGC_wedgeId")};
        VectorBranch<int>& m_NSWsTGC_wedgeType{parent().newVector<int>("Hits_sTGC_wedgeType")};
        ThreeVectorBranch m_NSWsTGC_detector_globalPosition{parent(), "Hits_sTGC_detector_globalPosition"};
        ThreeVectorBranch m_NSWsTGC_hitToDsurfacePosition{parent(), "Hits_sTGC_hitToDsurfacePosition"};
        ThreeVectorBranch m_NSWsTGC_hitToRsurfacePosition{parent(), "Hits_sTGC_hitToRsurfacePosition"};
        VectorBranch<float>& m_NSWsTGC_FastDigitRsurfacePositionX{parent().newVector<float>("Hits_sTGC_FastDigitRsurfacePositionX")};
        VectorBranch<float>& m_NSWsTGC_FastDigitRsurfacePositionY{parent().newVector<float>("Hits_sTGC_FastDigitRsurfacePositionY")};
        VectorBranch<int>& m_NSWsTGC_stripNumber{parent().newVector<int>("Hits_sTGC_stripNumber")};
    };
};

#endif  // sTGCSimHitVariables_H
