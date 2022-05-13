/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonPRDTEST_MDTDigitVARIABLES_H
#define MuonPRDTEST_MDTDigitVARIABLES_H

#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonPRDTest/PrdTesterModule.h"
namespace MuonPRDTest {
    class MdtDigitVariables : public PrdTesterModule {
    public:
        MdtDigitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl);

        ~MdtDigitVariables() = default;

        bool fill(const EventContext& ctx) override final;

        bool declare_keys() override final;

    private:
        SG::ReadHandleKey<MdtDigitContainer> m_digitKey{};

        ScalarBranch<int>& m_MDT_nDigits{parent().newScalar<int>("Digits_MDT_n")};
        MdtIdentifierBranch m_MDT_digit_id{parent(), "Digits_MDT"};

        VectorBranch<float>& m_MDT_dig_time{parent().newVector<float>("Digits_MDT_time")};
        VectorBranch<float>& m_MDT_dig_charge{parent().newVector<float>("Digits_MDT_charge")};
        VectorBranch<int>& m_MDT_dig_numberOfMultilayers{parent().newVector<int>("Digits_MDT_numberOfMultilayers")};

        ThreeVectorBranch m_MDT_dig_globalPos{parent(), "Digits_MDT_globalPos"};
        ThreeVectorBranch m_MDT_dig_localTubePos{parent(), "Digits_MDT_localTubePos"};
    };
}  // namespace MuonPRDTest
#endif  // MuonPRDTEST_MDTDigitVARIABLES_H
