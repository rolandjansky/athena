/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPRDTEST_TGCSDOVARIABLES_H
#define MUONPRDTEST_TGCSDOVARIABLES_H

#include "MuonPRDTest/PrdTesterModule.h"
#include "MuonSimData/MuonSimDataCollection.h"
namespace MuonPRDTest{
    class TgcSDOVariables : public PrdTesterModule {
    public:
        TgcSDOVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl);
    
        ~TgcSDOVariables() = default;
    
        bool fill(const EventContext& ctx) override final;
    
        bool declare_keys() override final;
    
    private:
        SG::ReadHandleKey<MuonSimDataCollection> m_key{};
    
        ScalarBranch<unsigned int>& m_tgc_nsdo{parent().newScalar<unsigned int>("nSDO_TGC")};
        TgcIdentifierBranch m_tgc_sdo_id{parent(), "SDO_TGC"};
        VectorBranch<int>& m_tgc_sdo_word{parent().newVector<int>("SDO_TGC_word")};
        VectorBranch<int>& m_tgc_sdo_barcode{parent().newVector<int>("SDO_TGC_barcode")};
        VectorBranch<float>& m_tgc_sdo_globaltime{parent().newVector<float>("SDO_TGC_global_time")};
    
        ThreeVectorBranch m_TGC_dig_globalPos{parent(), "SDO_TGC_globalPos"};
        VectorBranch<float>& m_tgc_sdo_localPosX{parent().newVector<float>("SDO_TGC_localPosX")};
        VectorBranch<float>& m_tgc_sdo_localPosY{parent().newVector<float>("SDO_TGC_localPosY")};
    };
};
#endif  // TGCSDOVARIABLES_H