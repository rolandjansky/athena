/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonPRDTEST_TGCDigitVARIABLES_H
#define MuonPRDTEST_TGCDigitVARIABLES_H

#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonPRDTest/PrdTesterModule.h"
namespace MuonPRDTest{
    class TgcDigitVariables : public PrdTesterModule {
    public:
        TgcDigitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl);
    
        ~TgcDigitVariables() = default;
    
        bool fill(const EventContext& ctx) override final;
    
        bool declare_keys() override final;
    
    private:
        SG::ReadHandleKey<TgcDigitContainer> m_key{};
        ScalarBranch<unsigned int>& m_TGC_nDigits{parent().newScalar<unsigned int>("N_Digits_TGC")};
        VectorBranch<float>& m_TGC_dig_localPosX{parent().newVector<float>("Digits_TGC_localPosX")};
        VectorBranch<float>& m_TGC_dig_localPosY{parent().newVector<float>("Digits_TGC_localPosY")};
        VectorBranch<int>& m_TGC_dig_stripNumber{parent().newVector<int>("Digits_TGC_stripNumber")};
        ThreeVectorBranch m_TGC_dig_globalPos{parent(), "Digits_TGC_globalPos"};
        TgcIdentifierBranch m_TGC_dig_id{parent(), "Digits_TGC"};
    };
};

#endif  // MuonPRDTEST_TGCDigitVARIABLES_H