/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPRDTEST_sTGCSDOVARIABLES_H
#define MUONPRDTEST_sTGCSDOVARIABLES_H

#include "MuonPRDTest/PrdTesterModule.h"
#include "MuonSimData/MuonSimDataCollection.h"

namespace MuonPRDTest{
    class sTgcSDOVariables : public PrdTesterModule {
    public:
        sTgcSDOVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl);
    
        ~sTgcSDOVariables() = default;
    
        bool fill(const EventContext& ctx) override final;
    
        bool declare_keys() override final;
    
    private:
        SG::ReadHandleKey<MuonSimDataCollection> m_key{};
    
        ScalarBranch<unsigned int>& m_NSWsTGC_nsdo{parent().newScalar<unsigned int>("nSDO_sTGC")};
        sTgcIdentifierBranch m_NSWsTGC_sdo_id{parent(), "SDO_sTGC"};
        VectorBranch<int>& m_NSWsTGC_sdo_word{parent().newVector<int>("SDO_sTGC_word")};
        VectorBranch<int>& m_NSWsTGC_sdo_barcode{parent().newVector<int>("SDO_sTGC_barcode")};
        VectorBranch<float>& m_NSWsTGC_sdo_globaltime{parent().newVector<float>("SDO_sTGC_global_time")};
    
        ThreeVectorBranch m_NSWsTGC_dig_globalPos{parent(), "SDO_sTGC_globalPos"};
        VectorBranch<float>& m_NSWsTGC_sdo_E{parent().newVector<float>("SDO_sTGC_E")};
        VectorBranch<float>& m_NSWsTGC_sdo_tof{parent().newVector<float>("SDO_sTGC_tof")};
        VectorBranch<float>& m_NSWsTGC_sdo_localPosX{parent().newVector<float>("SDO_sTGC_localPosX")};
        VectorBranch<float>& m_NSWsTGC_sdo_localPosY{parent().newVector<float>("SDO_sTGC_localPosY")};
    };
};

#endif  // sTGCSDOVARIABLES_H