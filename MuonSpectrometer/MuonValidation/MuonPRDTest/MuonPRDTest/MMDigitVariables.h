/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonPRDTEST_MMDigitVARIABLES_H
#define MuonPRDTEST_MMDigitVARIABLES_H

#include "MuonDigitContainer/MmDigitContainer.h"
#include "MuonPRDTest/PrdTesterModule.h"

namespace MuonPRDTest{
    class MMDigitVariables : public PrdTesterModule {
    public:
        MMDigitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl);
    
        ~MMDigitVariables() = default;
    
        bool fill(const EventContext& ctx) override final;
    
        bool declare_keys() override final;
    
    private:
        SG::ReadHandleKey<MmDigitContainer> m_key{};
        ScalarBranch<unsigned int>& m_NSWMM_nDigits{parent().newScalar<unsigned int>("N_Digits_MM")};
        VectorBranch<std::vector<float>>& m_NSWMM_dig_time{parent().newVector<std::vector<float>>("Digits_MM_time")};
        VectorBranch<std::vector<float>>& m_NSWMM_dig_charge{parent().newVector<std::vector<float>>("Digits_MM_charge")};
        VectorBranch<std::vector<int>>& m_NSWMM_dig_stripPosition{parent().newVector<std::vector<int>>("Digits_MM_stripPosition")};
        VectorBranch<std::vector<float>>& m_NSWMM_dig_sr_time{parent().newVector<std::vector<float>>("Digits_MM_stripResponse_time")};
        VectorBranch<std::vector<float>>& m_NSWMM_dig_sr_charge{parent().newVector<std::vector<float>>("Digits_MM_stripResponse_charge")};
        VectorBranch<std::vector<int>>& m_NSWMM_dig_sr_stripPosition{parent().newVector<std::vector<int>>("Digits_MM_stripResponse_stripPosition")};
        VectorBranch<std::vector<float>>& m_NSWMM_dig_time_trigger{parent().newVector<std::vector<float>>("Digits_MM_time_trigger")};
        VectorBranch<std::vector<float>>& m_NSWMM_dig_charge_trigger{parent().newVector<std::vector<float>>("Digits_MM_charge_trigger")};
        VectorBranch<std::vector<int>>& m_NSWMM_dig_position_trigger{parent().newVector<std::vector<int>>("Digits_MM_position_trigger")};
        VectorBranch<std::vector<int>>& m_NSWMM_dig_MMFE_VMM_id_trigger{parent().newVector<std::vector<int>>("Digits_MM_MMFE_VMM_id_trigger")};
        VectorBranch<std::vector<int>>& m_NSWMM_dig_VMM_id_trigger{parent().newVector<std::vector<int>>("Digits_MM_VMM_id_trigger")};
        VectorBranch<double>& m_NSWMM_dig_stripLposX{parent().newVector<double>("Digits_MM_stripLposX")};
        VectorBranch<double>& m_NSWMM_dig_stripLposY{parent().newVector<double>("Digits_MM_stripLposY")};
        ThreeVectorBranch m_NSWMM_dig_stripGpos{parent(), "Digits_MM_stripGpos"};
        VectorBranch<double>& m_NSWMM_dig_sr_stripLposX{parent().newVector<double>("Digits_MM_stripResponse_stripLposX")};
        VectorBranch<double>& m_NSWMM_dig_sr_stripLposY{parent().newVector<double>("Digits_MM_stripResponse_stripLposY")};
        ThreeVectorBranch m_NSWMM_dig_sr_stripGpos{parent(), "Digits_MM_stripResponse_stripGpos"};
        MmIdentifierBranch m_NSWMM_dig_id{parent(), "Digits_MM"};
    };
};
#endif  // MuonPRDTEST_MMDigitVARIABLES_H