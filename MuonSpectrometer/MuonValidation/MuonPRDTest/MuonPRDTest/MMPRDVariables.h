/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonPRDTEST_MMPRDVARIABLES_H
#define MuonPRDTEST_MMPRDVARIABLES_H

#include "MuonPRDTest/PrdTesterModule.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonRDO/MM_RawDataContainer.h"


namespace MuonPRDTest{
    class MMPRDVariables : public PrdTesterModule {
    public:
        MMPRDVariables(MuonTesterTree& tree, const std::string& prd_container_name, MSG::Level msglvl);
    
        ~MMPRDVariables() = default;
    
        bool fill(const EventContext& ctx) override final;
    
        bool declare_keys() override final;
    
    private:
        SG::ReadHandleKey<Muon::MMPrepDataContainer> m_key{};
        ScalarBranch<unsigned int>& m_NSWMM_nPRD{parent().newScalar<unsigned int>("N_PRD_MM")};
        VectorBranch<int>& m_NSWMM_PRD_time{parent().newVector<int>("PRD_MM_time")};
        VectorBranch<double>& m_NSWMM_PRD_covMatrix_1_1{parent().newVector<double>("PRD_MM_covMatrix_1_1")};
        VectorBranch<int>& m_NSWMM_PRD_nRdos{parent().newVector<int>("PRD_MM_nRdos")};
        VectorBranch<double>& m_NSWMM_PRD_localPosX{parent().newVector<double>("PRD_MM_localPosX")};
        VectorBranch<double>& m_NSWMM_PRD_localPosY{parent().newVector<double>("PRD_MM_localPosY")};
        ThreeVectorBranch m_NSWMM_PRD_globalPos{parent(), "PRD_MM_globalPos"};
        VectorBranch<double>& m_NSWMM_PRD_uTPCAngle{parent().newVector<double>("PRD_MM_uTPCAngle")};
        VectorBranch<double>& m_NSWMM_PRD_uTPCChiSqProb{parent().newVector<double>("PRD_MM_uTPCChiSqProb")};
        MatrixBranch<unsigned int>& m_NSWMM_PRD_stripNumbers{parent().newMatrix<unsigned int>("PRD_MM_stripNumbers")}; //numbers of strips associated to the PRD cluster
        MatrixBranch<int>& m_NSWMM_PRD_stripTimes{parent().newMatrix<int>("PRD_MM_stripTimes")};                       //times of strips associated to the PRD cluster
        MatrixBranch<int>& m_NSWMM_PRD_stripCharges{parent().newMatrix<int>("PRD_MM_stripCharges")};                   //times of strips associated to the PRD cluster
        MmIdentifierBranch m_NSWMM_PRD_id{parent(), "PRD_MM"};
    };
};

#endif  // MuonPRDTEST_MMPRDVARIABLES_H
