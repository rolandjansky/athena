/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonPRDTEST_MMRDOVARIABLES_H
#define MuonPRDTEST_MMRDOVARIABLES_H

#include "MuonPRDTest/PrdTesterModule.h"
#include "MuonRDO/MM_RawDataContainer.h"

namespace MuonPRDTest{
    class MMRDOVariables : public PrdTesterModule {
    public:
        MMRDOVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl);
    
        ~MMRDOVariables() = default;
    
        bool fill(const EventContext& ctx) override final;
    
        bool declare_keys() override final;
    
    private:
        SG::ReadHandleKey<Muon::MM_RawDataContainer> m_rdokey{};
        ScalarBranch<unsigned int>& m_NSWMM_nRDO{parent().newScalar<unsigned int>("N_RDO_MM")};
        VectorBranch<int>& m_NSWMM_rdo_time{parent().newVector<int>("RDO_MM_time")};
        VectorBranch<int>& m_NSWMM_rdo_charge{parent().newVector<int>("RDO_MM_charge")};
        VectorBranch<uint16_t>& m_NSWMM_rdo_relBcid{parent().newVector<uint16_t>("RDO_MM_relBcid")};
        VectorBranch<double>& m_NSWMM_rdo_localPosX{parent().newVector<double>("RDO_MM_localPosX")};
        VectorBranch<double>& m_NSWMM_rdo_localPosY{parent().newVector<double>("RDO_MM_localPosY")};
        ThreeVectorBranch m_NSWMM_rdo_globalPos{parent(), "RDO_MM_globalPos"};
        MmIdentifierBranch m_NSWMM_rdo_id{parent(), "RDO_MM"};
    };
};

#endif  // MuonPRDTEST_MMRDOVARIABLES_H