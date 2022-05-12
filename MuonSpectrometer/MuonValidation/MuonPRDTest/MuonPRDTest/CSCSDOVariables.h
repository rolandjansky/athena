/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPRDTEST_CSCSDOVARIABLES_H
#define MUONPRDTEST_CSCSDOVARIABLES_H

#include "MuonPRDTest/PrdTesterModule.h"
#include "MuonSimData/CscSimDataCollection.h"
namespace MuonPRDTest{
    class CscSDOVariables : public PrdTesterModule {
    public:
        CscSDOVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl);
    
        ~CscSDOVariables() = default;
    
        bool fill(const EventContext& ctx) override final;
    
        bool declare_keys() override final;
    
    private:
        SG::ReadHandleKey<CscSimDataCollection> m_key{};
    
        ScalarBranch<unsigned int>& m_csc_nsdo{parent().newScalar<unsigned int>("nSDO_CSC")};
        CscIdentifierBranch m_csc_sdo_id{parent(), "SDO_CSC"};
        VectorBranch<int>& m_csc_sdo_word{parent().newVector<int>("SDO_CSC_word")};
        VectorBranch<int>& m_csc_sdo_barcode{parent().newVector<int>("SDO_CSC_barcode")};
        VectorBranch<double>& m_csc_sdo_charge{parent().newVector<double>("SDO_CSC_charge")};
    
        ThreeVectorBranch m_CSC_dig_globalPos{parent(), "SDO_CSC_globalPos"};
        VectorBranch<float>& m_csc_sdo_localPosX{parent().newVector<float>("SDO_CSC_localPosX")};
        VectorBranch<float>& m_csc_sdo_localPosY{parent().newVector<float>("SDO_CSC_localPosY")};
    };
};
#endif  // CSCSDOVARIABLES_H