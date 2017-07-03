/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include <IsolationSelection/IsolationWP.h>
#include <IsolationSelection/TestMacroHelpers.h>

namespace CP {

    IsoCorrectionTestHelper::IsoCorrectionTestHelper(TTree* outTree, const std::string& ContainerName) :
                m_pt(),
                m_eta(),
                m_phi(),
                m_e(),
                m_Q(),

                m_orig_TrackIsol(),
                m_corr_TrackIsol(),

                m_orig_CaloIsol(),
                m_corr_CaloIsol(),

                m_orig_passIso(),
                m_corr_passIso() {
    }
    StatusCode IsoCorrectionTestHelper::Fill(xAOD::IParticleContainer* Particles) {
        if (!Particles) {
            Error("IsoCorrectionTestHelper::Fill()", "No particles given");
            return StatusCode::FAILURE;
        }
        //Clear the old vectors first
        m_pt.clear();
        m_eta.clear();
        m_phi.clear();
        m_e.clear();
        m_Q.clear();

        m_orig_TrackIsol.clear();
        m_corr_TrackIsol.clear();

        m_orig_CaloIsol.clear();
        m_corr_CaloIsol.clear();
        m_orig_passIso.clear();
        m_corr_passIso.clear();
        for (const auto object : *Particles) {
            m_pt.push_back(object->pt());
            m_eta.push_back(object->eta());
            m_phi.push_back(object->phi());
            m_e.push_back(object->e());
            m_Q.push_back(Charge(object));
        }

        return StatusCode::SUCCESS;

    }
    float IsoCorrectionTestHelper::Charge(const xAOD::IParticle* P) const {
        static SG::AuxElement::ConstAccessor<float> acc_charge("charge");
        if (!acc_charge.isAvailable(*P)) return 0;
        else return acc_charge(*P);
    }
//
//    private:
//
//    float Charge(const xAOD::IParticle* P) const;
//
//    std::vector<float> m_pt;
//    std::vector<float> m_eta;
//    std::vector<float> m_phi;
//    std::vector<float> m_e;
//    std::vector<int> m_Q;
//
//    std::vector<float> m_orig_TrackIsol;
//    std::vector<float> m_corr_TrackIsol;
//
//    std::vector<float> m_orig_CaloIsol;
//    std::vector<float> m_corr_CaloIsol;
//
//    std::vector<char> m_orig_passIso;
//    std::vector<char> m_corr_passIso;

}
