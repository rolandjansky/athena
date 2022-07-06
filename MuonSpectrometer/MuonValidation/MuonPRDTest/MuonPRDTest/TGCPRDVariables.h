/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonPRDTEST_TGCPRDVARIABLES_H
#define MuonPRDTEST_TGCPRDVARIABLES_H

#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonPRDTest/PrdTesterModule.h"

namespace MuonPRDTest{
    class TGCPRDVariables : public PrdTesterModule {
    public:
        TGCPRDVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl);
    
        ~TGCPRDVariables() = default;
    
        bool fill(const EventContext& ctx) override final;
    
        bool declare_keys() override final;
    
    private:
        SG::ReadHandleKey<Muon::TgcPrepDataContainer> m_key{};
        ScalarBranch<unsigned int>& m_TGC_nPRD{parent().newScalar<unsigned int>("N_PRD_TGC")};
        VectorBranch<float>& m_TGC_PRD_localPosX{parent().newVector<float>("PRD_TGC_localPosX")};
        VectorBranch<float>& m_TGC_PRD_localPosY{parent().newVector<float>("PRD_TGC_localPosY")};
        ThreeVectorBranch m_TGC_PRD_globalPos{parent(), "PRD_TGC_globalPos"};
        TgcIdentifierBranch m_TGC_PRD_id{parent(), "PRD_TGC"};
    };
};

#endif  // MuonPRDTEST_TGCPRDVARIABLES_H