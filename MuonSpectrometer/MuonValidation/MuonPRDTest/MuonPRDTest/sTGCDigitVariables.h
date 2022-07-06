/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonPRDTEST_sTGCDigitVARIABLES_H
#define MuonPRDTEST_sTGCDigitVARIABLES_H

#include "MuonDigitContainer/sTgcDigitContainer.h"
#include "MuonPRDTest/PrdTesterModule.h"

namespace MuonPRDTest{
    class sTgcDigitVariables : public PrdTesterModule {
    public:
        sTgcDigitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl);
    
        ~sTgcDigitVariables() = default;
    
        bool fill(const EventContext& ctx) override final;
    
        bool declare_keys() override final;
    
    private:
        SG::ReadHandleKey<sTgcDigitContainer> m_key{};
        ScalarBranch<unsigned int>& m_NSWsTGC_nDigits{parent().newScalar<unsigned int>("N_Digits_sTGC")};
        VectorBranch<float>& m_NSWsTGC_dig_localPosX{parent().newVector<float>("Digits_sTGC_localPosX")};
        VectorBranch<float>& m_NSWsTGC_dig_localPosY{parent().newVector<float>("Digits_sTGC_localPosY")};
        VectorBranch<double>& m_NSWsTGC_dig_time{parent().newVector<double>("Digits_sTGC_time")};
        VectorBranch<int>& m_NSWsTGC_dig_bctag{parent().newVector<int>("Digits_sTGC_bctag")};
        VectorBranch<double>& m_NSWsTGC_dig_charge{parent().newVector<double>("Digits_sTGC_charge")};
        VectorBranch<bool>& m_NSWsTGC_dig_isDead{parent().newVector<bool>("Digits_sTGC_isDead")};
        VectorBranch<bool>& m_NSWsTGC_dig_isPileup{parent().newVector<bool>("Digits_sTGC_isPileup")};
        VectorBranch<int>& m_NSWsTGC_dig_channelNumber{parent().newVector<int>("Digits_sTGC_channelNumber")};
        ThreeVectorBranch m_NSWsTGC_dig_globalPos{parent(), "Digits_sTGC_globalPos"};
        ThreeVectorBranch m_NSWsTGC_dig_PadglobalCornerPos{parent(), "Digits_sTGC_PadglobalCornerPos"};
        VectorBranch<float>& m_NSWsTGC_dig_channelPosX{parent().newVector<float>("Digits_sTGC_localPosX")};
        VectorBranch<float>& m_NSWsTGC_dig_channelPosY{parent().newVector<float>("Digits_sTGC_localPosY")};
        sTgcIdentifierBranch m_NSWsTGC_dig_id{parent(), "Digits_sTGC"};
    };
};
#endif  // MuonPRDTEST_sTGCDigitVARIABLES_H
