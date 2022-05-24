/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonPRDTEST_sTGCPRDVARIABLES_H
#define MuonPRDTEST_sTGCPRDVARIABLES_H

#include "MuonPRDTest/PrdTesterModule.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"

namespace MuonPRDTest{
    class sTGCPRDVariables : public PrdTesterModule {
    public:
        sTGCPRDVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl);
    
        ~sTGCPRDVariables() = default;
    
        bool fill(const EventContext& ctx) override final;
    
        bool declare_keys() override final;
    
    private:
        SG::ReadHandleKey<Muon::sTgcPrepDataContainer> m_key{};
        ScalarBranch<unsigned int>& m_NSWsTGC_nPRD{parent().newScalar<unsigned int>("N_PRD_sTGC")};
        VectorBranch<int>& m_NSWsTGC_PRD_charge{parent().newVector<int>("PRD_sTGC_charge")};
        VectorBranch<uint16_t>& m_NSWsTGC_PRD_bcTag{parent().newVector<uint16_t>("RDO_sTGC_bcTag")};
        VectorBranch<double>& m_NSWsTGC_PRD_localPosX{parent().newVector<double>("PRD_sTGC_localPosX")};
        VectorBranch<double>& m_NSWsTGC_PRD_localPosY{parent().newVector<double>("PRD_sTGC_localPosY")};
        ThreeVectorBranch m_NSWsTGC_PRD_globalPos{parent(), "PRD_sTGC_globalPos"};
        VectorBranch<double>& m_NSWsTGC_PRD_covMatrix_1_1{parent().newVector<double>("PRD_sTGC_covMatrix_1_1")};
        VectorBranch<double>& m_NSWsTGC_PRD_covMatrix_2_2{parent().newVector<double>("PRD_sTGC_covMatrix_2_2")};
        sTgcIdentifierBranch m_NSWsTGC_PRD_id{parent(), "PRD_sTGC"};
    };
};

#endif  // MuonPRDTEST_sTGCPRDVARIABLES_H