/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonPRDTEST_CSCRDOVARIABLES_H
#define MuonPRDTEST_CSCRDOVARIABLES_H

#include "MuonPRDTest/PrdTesterModule.h"
#include "MuonRDO/CscRawDataContainer.h"
#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"
#include "MuonIdHelpers/CscIdHelper.h"

namespace MuonPRDTest{
    class CSCRDOVariables : public PrdTesterModule {
    public:
        CSCRDOVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl, const MuonIdHelper* idhelper, const Muon::ICSC_RDO_Decoder* rdo_decoder);
    
        ~CSCRDOVariables() = default;
    
        bool fill(const EventContext& ctx) override final;
    
        bool declare_keys() override final;
    
    private:
        void setHelper(const MuonIdHelper* idhelper){
            m_CscIdHelper = dynamic_cast<const CscIdHelper*>(idhelper);
            if(!m_CscIdHelper) {
               throw std::runtime_error("casting IdHelper to CscIdHelper failed");
            }
        }
        SG::ReadHandleKey<CscRawDataContainer> m_key{};
        const CscIdHelper* m_CscIdHelper{nullptr};
        const Muon::ICSC_RDO_Decoder* m_rdo_decoder{nullptr};
        ScalarBranch<unsigned int>& m_CSC_nRDO{parent().newScalar<unsigned int>("N_RDO_CSC")};
        VectorBranch<float>& m_CSC_rdo_localPosX{parent().newVector<float>("RDO_CSC_localPosX")};
        VectorBranch<float>& m_CSC_rdo_localPosY{parent().newVector<float>("RDO_CSC_localPosY")};
        VectorBranch<int>& m_CSC_rdo_time{parent().newVector<int>("RDO_CSC_time")};
        ThreeVectorBranch m_CSC_rdo_globalPos{parent(), "RDO_CSC_globalPos"};
        CscIdentifierBranch m_CSC_rdo_id{parent(), "RDO_CSC"};
    };
};

#endif  // MuonPRDTEST_CSCRDOVARIABLES_H