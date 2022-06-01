/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonPRDTEST_TGCRDOVARIABLES_H
#define MuonPRDTEST_TGCRDOVARIABLES_H

#include "MuonPRDTest/PrdTesterModule.h"
#include "MuonRDO/TgcRdoContainer.h"
#include "TGCcablingInterface/ITGCcablingSvc.h"

namespace MuonPRDTest{
    class TGCRDOVariables : public PrdTesterModule {
    public:
        TGCRDOVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl, const ITGCcablingSvc* cabling_svc);
    
        ~TGCRDOVariables() = default;
    
        bool fill(const EventContext& ctx) override final;
    
        bool declare_keys() override final;
    
    private:
        SG::ReadHandleKey<TgcRdoContainer> m_key{};
        const ITGCcablingSvc* m_tgcCabling{nullptr};
        ScalarBranch<unsigned int>& m_TGC_nRDO{parent().newScalar<unsigned int>("N_RDO_TGC")};
        VectorBranch<float>& m_TGC_rdo_localPosX{parent().newVector<float>("RDO_TGC_localPosX")};
        VectorBranch<float>& m_TGC_rdo_localPosY{parent().newVector<float>("RDO_TGC_localPosY")};
        ThreeVectorBranch m_TGC_rdo_globalPos{parent(), "RDO_TGC_globalPos"};
        TgcIdentifierBranch m_TGC_rdo_id{parent(), "RDO_TGC"};
    };
};

#endif  // MuonPRDTEST_TGCRDOVARIABLES_H