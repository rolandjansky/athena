/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ResoTriggerMuonPlots.h"
#include "FourMomUtils/xAODP4Helpers.h"

#include <utility>

using namespace xAOD::P4Helpers;


ResoTriggerMuonPlots::ResoTriggerMuonPlots(PlotBase* pParent, const std::string& sDir, std::string sType) :
    PlotBase(pParent, sDir),
    m_sType(std::move(sType)) {
    m_pt_slices = {"0", "25", "55", "100"};
    m_etaBins = {-3.,    -2.5,   -2.4,   -1.918, -1.623, -1.348, -1.2329, -1.1479, -1.05, -0.908, -0.791,
                    -0.652, -0.476, -0.324, -0.132, 0,      0.132,  0.324,   0.476,   0.652, 0.791,  0.908,
                    1.05,   1.1479, 1.2329, 1.348,  1.623,  1.918,  2.4,     2.5,     3.};

    Res_pT = Book1D("Res" + m_sType + "_pT", "Res" + m_sType + "_pT;(1/pT-1/RECOpT)/(1/RECOpT);Entries", 200, -0.25, 0.25);
    Res_eta = Book1D("Res" + m_sType + "_eta", "Res" + m_sType + "_eta;eta-RECOeta;Entries", 200, -0.02, 0.02);
    Res_phi = Book1D("Res" + m_sType + "_phi", "Res" + m_sType + "_phi;phi-RECOphi;Entries", 200, -0.005, 0.005);

    Res_pT_vs_pT = Book2D("Res" + m_sType + "_pT_vs_pT", "Res" + m_sType + "_pT vs pT;pT [GeV];(1/pT-1/RECOpT)/(1/RECOpT)", 150, 0.,
                            150., 100, -0.25, 0.25);
    Res_eta_vs_pT =
        Book2D("Res" + m_sType + "_eta_vs_pT", "Res" + m_sType + "_eta vs pT;pT [GeV];eta-RECOeta", 150, 0., 150., 200, -0.02, 0.02);
    Res_phi_vs_pT =
        Book2D("Res" + m_sType + "_phi_vs_pT", "Res" + m_sType + "_phi vs pT;pT [GeV];phi-RECOphi", 150, 0., 150., 200, -0.005, 0.005);
    Res_pT_vs_eta.clear();
    Res_pT_vs_phi.clear();
    Res_phi_vs_phi.clear();
    Res_eta_vs_eta.clear();

    for (unsigned int i = 0; i < m_pt_slices.size() - 1; i++) {
        Res_pT_vs_eta.push_back(Book2D("Res" + m_sType + "_pT_vs_eta_from" + m_pt_slices[i] + "_to" + m_pt_slices[i + 1] + "GeV",
                                        "Res" + m_sType + "_pT vs eta from " + m_pt_slices[i] + " GeV to " + m_pt_slices[i + 1] +
                                            " GeV; eta;(1/pT-1/RECOpT)/(1/RECOpT)",
                                        m_etaBins.size() - 1, &m_etaBins[0], 100, -0.25, 0.25));
        Res_pT_vs_phi.push_back(Book2D("Res" + m_sType + "_pT_vs_phi_from" + m_pt_slices[i] + "_to" + m_pt_slices[i + 1] + "GeV",
                                        "Res" + m_sType + "_pT vs phi from " + m_pt_slices[i] + " GeV to " + m_pt_slices[i + 1] +
                                            " GeV;phi;(1/pT-1/RECOpT)/(1/RECOpT)",
                                        96, -M_PI, M_PI, 100, -0.25, 0.25));
        Res_phi_vs_phi.push_back(Book2D("Res" + m_sType + "_phi_vs_phi_from" + m_pt_slices[i] + "_to" + m_pt_slices[i + 1] + "GeV",
                                        "Res" + m_sType + "_phi vs phi from " + m_pt_slices[i] + " GeV to " + m_pt_slices[i + 1] +
                                            " GeV;phi;(1/pT-1/RECOpT)/(1/RECOpT)",
                                        96, -M_PI, M_PI, 100, -0.25, 0.25));
        Res_eta_vs_eta.push_back(
            Book2D("Res" + m_sType + "_eta_vs_eta_from" + m_pt_slices[i] + "_to" + m_pt_slices[i + 1] + "GeV",
                    "Res" + m_sType + "_eta vs eta from " + m_pt_slices[i] + " GeV to " + m_pt_slices[i + 1] + " GeV;eta;eta-RECOeta",
                    m_etaBins.size() - 1, &m_etaBins[0], 100, -0.25, 0.25));
    }
}

void ResoTriggerMuonPlots::fill(const xAOD::Muon& Trigmu, const xAOD::Muon& Recomu) {
    // fill plots Res_pt,eta,phi
    float pt = 0.001 * Recomu.pt();
    float eta = Recomu.eta();
    float phi = Recomu.phi();
    // float respt = (Trigmu.pt() - Recomu.pt())/Recomu.pt();
    float respt = (1. / Trigmu.pt() - 1. / Recomu.pt()) / (1. / Recomu.pt());

    const float d_phi = deltaPhi(Trigmu.phi(), phi);
    const float d_eta = Trigmu.eta() - eta;
    Res_pT->Fill(respt);
    Res_eta->Fill(d_eta);
    Res_phi->Fill(d_phi);

    Res_pT_vs_pT->Fill(pt, respt);
    Res_eta_vs_pT->Fill(pt, d_eta);
    Res_phi_vs_pT->Fill(pt, d_phi);

    for (unsigned int i = 0; i < m_pt_slices.size() - 1; i++) {
        float pt_min = atof(m_pt_slices[i].c_str());
        float pt_max = atof(m_pt_slices[i + 1].c_str());
        if (pt >= pt_min && pt < pt_max) {
            Res_pT_vs_eta[i]->Fill(eta, respt);
            Res_pT_vs_phi[i]->Fill(phi, respt);
            Res_phi_vs_phi[i]->Fill(phi, d_phi);
            Res_eta_vs_eta[i]->Fill(eta, d_eta);
        }
    }
}

void ResoTriggerMuonPlots::fill(const xAOD::L2StandAloneMuon& L2SAmu, const xAOD::Muon& Recomu) {
    float pt = 0.001 * Recomu.pt();
    float eta = Recomu.eta();
    float phi = Recomu.phi();
    float respt = 0;
    if (L2SAmu.pt() > 0.) respt = (1. / (1000. * L2SAmu.pt()) - 1. / Recomu.pt()) / (1. / Recomu.pt());
    if (L2SAmu.pt() < 0.) respt = (1. / (-1000. * L2SAmu.pt()) - 1. / Recomu.pt()) / (1. / Recomu.pt());

    const float d_eta = L2SAmu.eta() - eta;
    const float d_phi = deltaPhi(L2SAmu.phi(), phi);
    Res_pT->Fill(respt);
    Res_eta->Fill(d_eta);
    Res_phi->Fill(d_phi);

    Res_pT_vs_pT->Fill(pt, respt);
    Res_eta_vs_pT->Fill(pt, d_eta);
    Res_phi_vs_pT->Fill(pt, d_phi);

    for (unsigned int i = 0; i < m_pt_slices.size() - 1; i++) {
        float pt_min = atof(m_pt_slices[i].c_str());
        float pt_max = atof(m_pt_slices[i + 1].c_str());
        if (pt >= pt_min && pt < pt_max) {
            Res_phi_vs_phi[i]->Fill(phi, d_phi);
            Res_eta_vs_eta[i]->Fill(eta, d_eta);
            Res_pT_vs_eta[i]->Fill(eta, respt);
            Res_pT_vs_phi[i]->Fill(phi, respt);
        }
    }
}

void ResoTriggerMuonPlots::fill(const xAOD::L2CombinedMuon& L2CBmu, const xAOD::Muon& Recomu) {
    float pt = 0.001 * Recomu.pt();
    float eta = Recomu.eta();
    float phi = Recomu.phi();
    float respt = (1. / (L2CBmu.pt()) - 1. / Recomu.pt()) / (1. / Recomu.pt());
    ;

    const float d_phi = deltaPhi(L2CBmu.phi() , phi);
    const float d_eta = L2CBmu.eta() - eta;
    Res_pT->Fill(respt);
    Res_eta->Fill(d_eta);
    Res_phi->Fill(d_phi);

    Res_pT_vs_pT->Fill(pt, respt);
    Res_eta_vs_pT->Fill(pt, d_eta);
    Res_phi_vs_pT->Fill(pt, d_phi);

    for (unsigned int i = 0; i < m_pt_slices.size() - 1; i++) {
        float pt_min = atof(m_pt_slices[i].c_str());
        float pt_max = atof(m_pt_slices[i + 1].c_str());
        if (pt >= pt_min && pt < pt_max) {
            Res_phi_vs_phi[i]->Fill(phi, d_phi);
            Res_eta_vs_eta[i]->Fill(eta, d_eta);
            Res_pT_vs_eta[i]->Fill(eta, respt);
            Res_pT_vs_phi[i]->Fill(phi, respt);
        }
    }
}
