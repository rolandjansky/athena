/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTSDOVARIABLES_H
#define MDTSDOVARIABLES_H

#include "MuonPRDTest/PrdTesterModule.h"
#include "MuonSimData/MuonSimDataCollection.h"
namespace MuonPRDTest {
    class MdtSDOVariables : public PrdTesterModule {
    public:
        MdtSDOVariables(MuonTesterTree& tree, const std::string& grp_name, MSG::Level msglvl);

        ~MdtSDOVariables() = default;

        bool fill(const EventContext& ctx) override final;

        bool declare_keys() override final;

    private:
        SG::ReadHandleKey<MuonSimDataCollection> m_sdo_key{};
        ScalarBranch<unsigned int>& m_mdt_nsdo{parent().newScalar<unsigned int>("SDO_MDT_num")};
        MdtIdentifierBranch m_mdt_sdo_id{parent(), "SDO_MDT"};
        VectorBranch<int>& m_mdt_sdo_word{parent().newVector<int>("SDO_MDT_word")};
        VectorBranch<int>& m_mdt_sdo_barcode{parent().newVector<int>("SDO_MDT_barcode")};
        ThreeVectorBranch m_mdt_sdo_globalPos{parent(), "SDO_MDT_globalPos"};
        VectorBranch<float>& m_mdt_sdo_globalTime{parent().newVector<float>("SDO_MDT_globalTime")};
        VectorBranch<float>& m_mdt_sdo_localPosX{parent().newVector<float>("SDO_MDT_localPosX")};
        VectorBranch<float>& m_mdt_sdo_localPosY{parent().newVector<float>("SDO_MDT_localPosY")};
    };
}  // namespace MuonPRDTest
#endif  // MDTSDOVARIABLES_H
