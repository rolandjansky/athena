/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPRDTEST_MMSDOVARIABLES_H
#define MUONPRDTEST_MMSDOVARIABLES_H

#include "MuonPRDTest/PrdTesterModule.h"
#include "MuonSimData/MuonSimDataCollection.h"

namespace MuonPRDTest{
    class MMSDOVariables : public PrdTesterModule {
    public:
        MMSDOVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl);
    
        ~MMSDOVariables() = default;
    
        bool fill(const EventContext& ctx) override final;
    
        bool declare_keys() override final;
    
    private:
        SG::ReadHandleKey<MuonSimDataCollection> m_key{};
    
        ScalarBranch<unsigned int>& m_NSWMM_nsdo{parent().newScalar<unsigned int>("nSDO_MM")};
        MmIdentifierBranch m_NSWMM_sdo_id{parent(), "SDO_MM"};
        VectorBranch<int>& m_NSWMM_sdo_word{parent().newVector<int>("SDO_MM_word")};
        VectorBranch<int>& m_NSWMM_sdo_barcode{parent().newVector<int>("SDO_MM_barcode")};
        VectorBranch<float>& m_NSWMM_sdo_globaltime{parent().newVector<float>("SDO_MM_global_time")};
    
        ThreeVectorBranch m_NSWMM_dig_globalPos{parent(), "SDO_MM_globalPos"};
        VectorBranch<float>& m_NSWMM_sdo_localPosX{parent().newVector<float>("SDO_MM_localPosX")};
        VectorBranch<float>& m_NSWMM_sdo_localPosY{parent().newVector<float>("SDO_MM_localPosY")};
    };
};

#endif  // MMSDOVARIABLES_H