/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonPRDTEST_CSCPRDVARIABLES_H
#define MuonPRDTEST_CSCPRDVARIABLES_H

#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonPRDTest/PrdTesterModule.h"

namespace MuonPRDTest{
    class CSCPRDVariables : public PrdTesterModule {
    public:
        CSCPRDVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl);
    
        ~CSCPRDVariables() = default;
    
        bool fill(const EventContext& ctx) override final;
    
        bool declare_keys() override final;
    
    private:
        SG::ReadHandleKey<Muon::CscPrepDataContainer> m_key{};
        ScalarBranch<unsigned int>& m_CSC_nPRD{parent().newScalar<unsigned int>("N_PRD_CSC")};
        VectorBranch<float>& m_CSC_PRD_localPosX{parent().newVector<float>("PRD_CSC_localPosX")};
        VectorBranch<float>& m_CSC_PRD_localPosY{parent().newVector<float>("PRD_CSC_localPosY")};
        ThreeVectorBranch m_CSC_PRD_globalPos{parent(), "PRD_CSC_globalPos"};
        VectorBranch<int>& m_CSC_PRD_time{parent().newVector<int>("PRD_CSC_time")};
        VectorBranch<int>& m_CSC_PRD_charge{parent().newVector<int>("PRD_CSC_charge")};
        CscIdentifierBranch m_CSC_PRD_id{parent(), "PRD_CSC"};
    };
};

#endif  // MuonPRDTEST_CSCPRDVARIABLES_H