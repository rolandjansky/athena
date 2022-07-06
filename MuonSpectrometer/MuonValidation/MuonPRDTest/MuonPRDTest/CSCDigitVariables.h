/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonPRDTEST_CSCDigitVARIABLES_H
#define MuonPRDTEST_CSCDigitVARIABLES_H

#include "MuonDigitContainer/CscDigitContainer.h"
#include "MuonPRDTest/PrdTesterModule.h"
namespace MuonPRDTest{
    class CscDigitVariables : public PrdTesterModule {
    public:
        CscDigitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl);
    
        ~CscDigitVariables() = default;
    
        bool fill(const EventContext& ctx) override final;
    
        bool declare_keys() override final;
    
    private:
        SG::ReadHandleKey<CscDigitContainer> m_key{};
        ScalarBranch<unsigned int>& m_CSC_nDigits{parent().newScalar<unsigned int>("N_Digits_CSC")};
        VectorBranch<float>& m_CSC_dig_localPosX{parent().newVector<float>("Digits_CSC_localPosX")};
        VectorBranch<float>& m_CSC_dig_localPosY{parent().newVector<float>("Digits_CSC_localPosY")};
        ThreeVectorBranch m_CSC_dig_globalPos{parent(), "Digits_CSC_globalPos"};
        CscIdentifierBranch m_CSC_dig_id{parent(), "Digits_CSC"};
    };
};

#endif  // MuonPRDTEST_CSCDigitVARIABLES_H