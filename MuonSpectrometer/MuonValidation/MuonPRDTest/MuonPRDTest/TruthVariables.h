/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPRDTEST_TRUTHVARIABLES_H
#define MUONPRDTEST_TRUTHVARIABLES_H

#include "GeneratorObjects/McEventCollection.h"
#include "MuonPRDTest/PrdTesterModule.h"
namespace MuonPRDTest {
    class TruthVariables : public PrdTesterModule {
    public:
        TruthVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl);

        bool fill(const EventContext& ctx) override final;
        bool declare_keys() override final;

    private:
        SG::ReadHandleKey<McEventCollection> m_key{};
        ScalarBranch<unsigned int>& m_Truth_nVertices{parent().newScalar<unsigned int>("TruthVertex_n")};

        CartesFourVecBranch m_Truth_vertex{parent(), "TruthVertex", CartesFourVecBranch::Type::Pos};
        VectorBranch<int>& m_Truth_vertexId{parent().newVector<int>("TruthVertex_Id")};

        ScalarBranch<unsigned int>& m_Truth_nParticles{parent().newScalar<unsigned int>("TruthParticle_n")};
        PtEtaPhiEBranch m_truthParticleP4{parent(), "TruthParticle"};
        VectorBranch<int>& m_Truth_particlePdg_id{parent().newVector<int>("TruthParticle_Pdg")};
        VectorBranch<int>& m_Truth_particleStatus{parent().newVector<int>("TruthParticle_Status")};
        VectorBranch<int>& m_Truth_particleBarcode{parent().newVector<int>("TruthParticle_Status")};
        VectorBranch<int>& m_Truth_particleProduction_vertex_id{parent().newVector<int>("TruthParticle_Production_vertex_id")};
        VectorBranch<int>& m_Truth_particleEnd_vertex_id{parent().newVector<int>("TruthParticle_End_vertex_id")};
    };
}  // namespace MuonPRDTest
#endif  // TRUTHVARIABLES_H
