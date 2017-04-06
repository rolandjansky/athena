/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * EfficiencyScaleFactor.cxx
 *
 *  Created on: Apr 10, 2014
 *      Author: goblirsc
 */

#include "MuonEfficiencyCorrections/EfficiencyScaleFactor.h"
#include <TRandom3.h>
#include <TClass.h>
namespace CP {
    EfficiencyScaleFactor::EfficiencyScaleFactor() :
                m_sf(0),
                m_eff(0),
                m_mc_eff(0),
                m_sf_sys(0),
                m_eff_sys(0),
                m_mc_eff_sys(0),
                m_sf_PtDepsys(0),
                m_eff_PtDepsys(0),
                m_sf_replicas(),
                m_eff_replicas(),
                m_mc_eff_replicas(),
                m_etaphi(),
                m_sys(),
                m_is_lowpt(false),
                m_respond_to_PtDepsys(false),
                m_PtDepsys_weight(0.),
                m_default_eff(1.),
                m_default_eff_ttva(1.),
                m_Type(CP::MuonEfficiencyType::Undefined) {
    }

    EfficiencyScaleFactor::EfficiencyScaleFactor(std::string file, std::string time_unit, SystematicSet sys, CP::MuonEfficiencyType effType, bool isLowPt, bool hasPtDepSys) :
                EfficiencyScaleFactor() {
        m_sys = sys;
        m_Type = effType;
        m_is_lowpt = isLowPt;
        m_respond_to_PtDepsys = hasPtDepSys;
        ReadFromFile(file, time_unit);
        if ((sysname().find("up") != std::string::npos) || (sysname().find("Up") != std::string::npos)) {
            m_default_eff = 2.;
            m_default_eff_ttva = 1. + 1.e-9;
        } else if ((sysname().find("down") != std::string::npos) || (sysname().find("Down") != std::string::npos)) {
            m_default_eff = 0.;
            m_default_eff_ttva = 1. - 1.e-9;
        }
    }

    EfficiencyScaleFactor::EfficiencyScaleFactor(const EfficiencyScaleFactor & other) {
        CopyContent(other);
    }
    EfficiencyScaleFactor & EfficiencyScaleFactor::operator =(const EfficiencyScaleFactor & other) {
        if (this == &other) {
            return *this;
        }
        CopyContent(other);
        return *this;
    }
    void EfficiencyScaleFactor::CopyContent(const EfficiencyScaleFactor &other) {
        Clear();
        m_sys = other.m_sys;
        m_is_lowpt = other.m_is_lowpt;
        m_respond_to_PtDepsys = other.m_respond_to_PtDepsys;
        m_PtDepsys_weight = other.m_PtDepsys_weight;
        m_Type = other.m_Type;
        CopyHistHandler(m_sf, other.m_sf);
        CopyHistHandler(m_eff, other.m_eff);
        CopyHistHandler(m_mc_eff, other.m_mc_eff);

        CopyHistHandler(m_sf_sys, other.m_sf_sys);
        CopyHistHandler(m_eff_sys, other.m_eff_sys);
        CopyHistHandler(m_mc_eff_sys, other.m_mc_eff_sys);

        if (other.m_respond_to_PtDepsys) {
            CopyHistHandler(m_sf_PtDepsys, other.m_sf_PtDepsys);
            CopyHistHandler(m_eff_PtDepsys, other.m_eff_PtDepsys);
        }
        CopyReplicaVec(m_sf_replicas, other.m_sf_replicas);
        CopyReplicaVec(m_eff_replicas, other.m_eff_replicas);
        CopyReplicaVec(m_mc_eff_replicas, other.m_mc_eff_replicas);
    }
    void EfficiencyScaleFactor::CopyHistHandler(HistHandler* &own, const HistHandler* other) {
        if (!other) {
            own = 0;
            return;
        }
        own = package_histo(dynamic_cast<TH1*>(other->GetHist()->Clone((std::string("EffSFCloneOf") + other->GetHist()->GetName()).c_str())));

    }
    void EfficiencyScaleFactor::CopyReplicaVec(EfficiencyScaleFactor::SFvec &own, const EfficiencyScaleFactor::SFvec &other) {
        for (ciSFvec h = other.begin(); h != other.end(); ++h) {
            HistHandler* copy = 0;
            CopyHistHandler(copy, *h);
            if (copy) own.push_back(copy);
        }
    }
    bool EfficiencyScaleFactor::ReadFromFile(std::string file, std::string time_unit) {
        // open the file
        TDirectory* dir = gDirectory;
        TFile* f = TFile::Open(file.c_str(), "READ");
        if (!f) {
            Error("EfficiencyScaleFactor", "Unable to open file %s", file.c_str());
            return false;
        }
        dir->cd();
        // now we can read our four Histos
        m_eff = ReadHistFromFile("Eff", f, time_unit);
        m_eff_sys = ReadHistFromFile("Eff_sys", f, time_unit);

        m_sf = ReadHistFromFile("SF", f, time_unit);
        m_sf_sys = ReadHistFromFile("SF_sys", f, time_unit);

        m_mc_eff = ReadHistFromFile("MC_Eff", f, time_unit);
        m_mc_eff_sys = ReadHistFromFile("MC_Eff_sys", f, time_unit);

        // for high pt eff, we also load the pt dependent part
        if (m_respond_to_PtDepsys) {
            m_sf_PtDepsys = ReadHistFromFile("SF_PtDep_sys", f, time_unit);
            m_eff_PtDepsys = ReadHistFromFile("Eff_PtDep_sys", f, time_unit);
        }
        // and don't forget to close the file again!
        f->Close();
        delete f;
        return true;
    }
    bool EfficiencyScaleFactor::CheckConsistency() {
        //Check whether  the SFs could be successfully loaded
        if (!m_sf) Error("EfficiencyScaleFactor",
                         "Could not load the SF for %s and systematic %s",
                         EfficiencyTypeName(m_Type).c_str(),
                         m_sys.name().c_str());
        if (!m_sf_sys) Error("EfficiencyScaleFactor",
                             "Could not load the SF systematic for %s and systematic %s",
                             EfficiencyTypeName(m_Type).c_str(),
                             m_sys.name().c_str());
        if (m_respond_to_PtDepsys && !m_sf_PtDepsys)
          Error("EfficiencyScaleFactor",
                "Could not load the SF pt-dependent systematic for %s and systematic %s",
                EfficiencyTypeName(m_Type).c_str(),
                m_sys.name().c_str());
        return m_sf_sys != NULL && m_sf != NULL && (!m_respond_to_PtDepsys || m_sf_PtDepsys);
    }
    HistHandler* EfficiencyScaleFactor::ReadHistFromFile(std::string name, TFile* f, std::string time_unit) {
        TH1* histHolder = 0;
        f->GetObject((name + std::string("_") + time_unit).c_str(), histHolder);
        if (!histHolder) {
            // if no period weighting, the histo may also not have a time period in the name at all
            if (time_unit == "All") {
                f->GetObject(name.c_str(), histHolder);
            }
            return 0;
        }
        // replace the histos by clones so that we can close the files again
        HistHandler* out = package_histo((TH1*) (histHolder->Clone(Form("%s_%s_%s%s", name.c_str(), f->GetName(), time_unit.c_str(), sysname().c_str()))));
        if (histHolder) delete histHolder;
        return out;
    }
    HistHandler *EfficiencyScaleFactor::package_histo(TH1* h) {
        // make sure that the correct type of histo is used
        if (dynamic_cast<TH1F*>(h)) {
            return new HistHandler_TH1F(dynamic_cast<TH1F*>(h));
        } else if (dynamic_cast<TH2F*>(h)) {
            return new HistHandler_TH2F(dynamic_cast<TH2F*>(h));
        } else if (dynamic_cast<TH3F*>(h)) {
            return new HistHandler_TH3F(dynamic_cast<TH3F*>(h));
        } else if (dynamic_cast<TH2Poly*>(h)) {
            return new HistHandler_TH2Poly(dynamic_cast<TH2Poly*>(h));
        } else {
            Error("EfficiencyScaleFactor", "Unable to package histo %s (%s) in a known HistHandler", h->GetName(), h->IsA()->GetName());
            return 0;
        }
    }
    std::string EfficiencyScaleFactor::sysname() {

        // utility method - we make sure that the different histos have separate names by adding a string produced in here
        if (m_sys == SystematicSet()) return "central";
        return m_sys.begin()->name();
    }
    CorrectionCode EfficiencyScaleFactor::ScaleFactor(const xAOD::Muon& mu, float & SF) {
        CorrectionCode cc = GetContentFromHist(m_sf, m_sf_PtDepsys, mu, SF, m_respond_to_PtDepsys);
        if (cc == CorrectionCode::Error) Error("EfficiencyScaleFactor", "Could not apply the scale factor");
        return cc;
    }

    CorrectionCode EfficiencyScaleFactor::DataEfficiency(const xAOD::Muon& mu, float & Eff) {
        CorrectionCode cc = GetContentFromHist(m_eff, m_eff_PtDepsys, mu, Eff, m_respond_to_PtDepsys);
        if (cc == CorrectionCode::Error) Error("EfficiencyScaleFactor", "Could not apply the data efficiency");
        return cc;
    }
    CorrectionCode EfficiencyScaleFactor::MCEfficiency(const xAOD::Muon& mu, float & Eff) {
        CorrectionCode cc = GetContentFromHist(m_mc_eff, NULL, mu, Eff, false);
        if (cc == CorrectionCode::Error) Error("EfficiencyScaleFactor", "Could not apply the Monte Carlo efficiency");
        return cc;
    }
    CorrectionCode EfficiencyScaleFactor::GetContentFromHist(HistHandler* Hist, HistHandler* PtDepHist, const xAOD::Muon& mu, float & Eff, bool PtDepHistNeeded) {
        if (!Hist) {
            Warning("EfficiencyScaleFactor", "Could not find histogram for variation %s and muon with pt=%.4f, eta=%.2f and phi=%.2f, returning %.1f", sysname().c_str(), mu.pt(), mu.eta(), mu.phi(), m_default_eff);
            Eff = m_default_eff;
            return CorrectionCode::OutOfValidityRange;
        } else if (PtDepHistNeeded && !PtDepHist) {
            Error("EfficiencyScaleFactor",
                  "Could not find pt-dependent histogram for variation %s and muon with pt=%.4f, eta=%.2f and phi=%.2f", sysname().c_str(), mu.pt(), mu.eta(), mu.phi());
            return CorrectionCode::Error;
        }
        if (m_Type == CP::MuonEfficiencyType::TTVA && fabs(mu.eta()) > 2.5 && fabs(mu.eta()) <= 2.7 && mu.muonType() == xAOD::Muon::MuonType::MuonStandAlone) {
            static bool Warned = false;
            if (!Warned) Info("EfficiencyScaleFactor",
                              "No TTVA sf/efficiency provided for standalone muons with 2.5<|eta|<2.7 for variation %s and muon with pt=%.4f, eta=%.2f and phi=%.2f, returning %.1f", sysname().c_str(), mu.pt(), mu.eta(), mu.phi(), m_default_eff_ttva);
            Warned = true;
            Eff = m_default_eff_ttva;
            return CorrectionCode::Ok;
        }
        int bin = -1;
        CorrectionCode cc = Hist->FindBin(mu, bin);
        if (cc != CorrectionCode::Ok) return cc;
        else Eff = Hist->GetBinContent(bin);

        // if we need to, apply an additional relative pt dependent contribution
        if (PtDepHistNeeded && mu.pt() > 200000.) {
            int binsys = -1;
            cc = PtDepHist->FindBin(mu, binsys);
            if (cc != CorrectionCode::Ok) {
                return cc;
            }
            // pt dependent sys is stored as relative error per TeV of pT
            Eff = Eff * (1 + m_PtDepsys_weight * PtDepHist->GetBinContent(binsys) * mu.pt() / 1.0e6);
        }
        return CorrectionCode::Ok;
    }
    CorrectionCode EfficiencyScaleFactor::ScaleFactorReplicas(const xAOD::Muon& mu, std::vector<float> & SF) {
        return GetContentReplicasFromHist(m_sf_replicas, mu, SF);
    }
    CorrectionCode EfficiencyScaleFactor::DataEfficiencyReplicas(const xAOD::Muon& mu, std::vector<float> & eff) {
        return GetContentReplicasFromHist(m_eff_replicas, mu, eff);
    }
    CorrectionCode EfficiencyScaleFactor::MCEfficiencyReplicas(const xAOD::Muon& mu, std::vector<float> & eff) {
        return GetContentReplicasFromHist(m_mc_eff_replicas, mu, eff);
    }
    CorrectionCode EfficiencyScaleFactor::GetContentReplicasFromHist(EfficiencyScaleFactor::SFvec &replicas, const xAOD::Muon& mu, std::vector<float> & SF) {
        if (replicas.size() != SF.size()) GenerateReplicas(SF.size(), 1000. * mu.phi() + mu.eta());
        if (replicas.empty()) return CorrectionCode::OutOfValidityRange;
        if (m_Type == CP::MuonEfficiencyType::TTVA && fabs(mu.eta()) > 2.5 && fabs(mu.eta()) <= 2.7 && mu.muonType() == xAOD::Muon::MuonType::MuonStandAlone) {
            static bool Warned = false;
            if (!Warned) Info("EfficiencyScaleFactor",
                              "No TTVA sf/efficiency provided for standalone muons with 2.5<|eta|<2.7 for variation %s and muon with pt=%.4f, eta=%.2f and phi=%.2f, returning %.1f", sysname().c_str(), mu.pt(), mu.eta(), mu.phi(), m_default_eff_ttva);
            Warned = true;
            for (auto& r : SF)
                r = m_default_eff_ttva;
            return CorrectionCode::Ok;
        }
        int bin = -1;
        CorrectionCode res = (*replicas.begin())->FindBin(mu, bin);
        if (res != CorrectionCode::Ok) return res;
        else {
            for (size_t k = 0; k < SF.size(); k++) {
                SF[k] = replicas.at(k)->GetBinContent(bin);
            }
        }
        return res;
    }
    EfficiencyScaleFactor::~EfficiencyScaleFactor() {
        Clear();
    }
    void EfficiencyScaleFactor::DeleteOldReplicas(EfficiencyScaleFactor::SFvec &Vec, bool ClearVec) {
        for (auto &old : Vec) {
            if (old) delete old;
            old = NULL;
        }
        if (ClearVec) Vec.clear();
    }
    void EfficiencyScaleFactor::Clear() {
        DeleteOldReplicas(m_sf_replicas, true);
        DeleteOldReplicas(m_eff_replicas, true);
        DeleteOldReplicas(m_mc_eff_replicas, true);
        if (m_sf) delete m_sf;
        if (m_eff) delete m_eff;
        if (m_mc_eff) delete m_mc_eff;
        if (m_mc_eff_sys) delete m_mc_eff_sys;
        if (m_sf_sys) delete m_sf_sys;
        if (m_eff_sys) delete m_eff_sys;
        if (m_sf_PtDepsys) delete m_sf_PtDepsys;
        if (m_eff_PtDepsys) delete m_eff_PtDepsys;
    }
    void EfficiencyScaleFactor::GenerateReplicas(int nrep, int seed) {
        GenerateReplicasFromHist(m_eff, nrep, seed, m_eff_replicas);
        GenerateReplicasFromHist(m_sf, nrep, seed, m_sf_replicas);
        GenerateReplicasFromHist(m_mc_eff, nrep, seed, m_mc_eff_replicas);
    }
    void EfficiencyScaleFactor::GenerateReplicasFromHist(HistHandler* h, int nrep, int seed, EfficiencyScaleFactor::SFvec &replicas) {
        if (!h) return;
        DeleteOldReplicas(replicas);
        TRandom3 Rndm(seed);
        replicas.resize(nrep);
        int nbins = h->NBins();
        for (int t = 0; t < nrep; t++) {
            HistHandler* replica = package_histo((TH1*) h->GetHist()->Clone(Form("rep%d_%s", t, h->GetHist()->GetName())));
            for (int bin = 0; bin < nbins; bin++) {
                replica->SetBinContent(bin, Rndm.Gaus(h->GetBinContent(bin), h->GetBinError(bin)));
            }
            replicas.at(t) = replica;
        }
    }
    void EfficiencyScaleFactor::ApplySysVariation() {
        if (m_sys == SystematicSet()) return;
        if (!m_is_lowpt) {
            if (*(m_sys.begin()) == SystematicVariation("MUON_" + EfficiencyTypeName(m_Type) + "_STAT", -1)) AddStatErrors(-1.);
            if (*(m_sys.begin()) == SystematicVariation("MUON_" + EfficiencyTypeName(m_Type) + "_STAT", 1)) AddStatErrors(1.);
            if (*(m_sys.begin()) == SystematicVariation("MUON_" + EfficiencyTypeName(m_Type) + "_SYS", -1)) AddSysErrors(-1.);
            if (*(m_sys.begin()) == SystematicVariation("MUON_" + EfficiencyTypeName(m_Type) + "_SYS", 1)) AddSysErrors(1.);
        } else {
            if (*(m_sys.begin()) == SystematicVariation("MUON_" + EfficiencyTypeName(m_Type) + "_STAT_LOWPT", -1)) AddStatErrors(-1.);
            if (*(m_sys.begin()) == SystematicVariation("MUON_" + EfficiencyTypeName(m_Type) + "_STAT_LOWPT", 1)) AddStatErrors(1.);
            if (*(m_sys.begin()) == SystematicVariation("MUON_" + EfficiencyTypeName(m_Type) + "_SYS_LOWPT", -1)) AddSysErrors(-1.);
            if (*(m_sys.begin()) == SystematicVariation("MUON_" + EfficiencyTypeName(m_Type) + "_SYS_LOWPT", 1)) AddSysErrors(1.);
        }
    }
    void EfficiencyScaleFactor::AddStatErrors(float weight) {
        AddStatErrors_histo(m_sf, weight);
        AddStatErrors_histo(m_eff, weight);
        AddStatErrors_histo(m_mc_eff, weight);
    }

    void EfficiencyScaleFactor::AddSysErrors(float weight) {

        // turn on the pt dependent contribution, if there is any
        if (m_respond_to_PtDepsys) {
            m_PtDepsys_weight = weight;
        }
        AddSysErrors_histo(m_sf, m_sf_sys, weight);
        AddSysErrors_histo(m_eff, m_eff_sys, weight);
        AddSysErrors_histo(m_mc_eff, m_mc_eff_sys, weight);

        AddSysErrors_vector(m_sf_replicas, m_sf_sys, weight);
        AddSysErrors_vector(m_eff_replicas, m_sf_sys, weight);
        AddSysErrors_vector(m_mc_eff_replicas, m_sf_sys, weight);

    }
    void EfficiencyScaleFactor::AddSysErrors_vector(EfficiencyScaleFactor::SFvec &Vec, HistHandler* hsys, float weight) {
        for (auto Histo : Vec) {
            AddSysErrors_histo(Histo, hsys, weight);
        }
    }

    void EfficiencyScaleFactor::AddSysErrors_histo(HistHandler* h, HistHandler *hsys, float weight) {
        if (!h || !hsys) return;
        for (int t = 0; h && t < h->NBins(); ++t) {
            double binc = h->GetBinContent(t);
            binc += weight * hsys->GetBinContent(t);
            h->SetBinContent(t, binc);
        }
    }
    void EfficiencyScaleFactor::AddStatErrors_histo(HistHandler* h, float weight) {
        if (!h) return;
        for (int t = 0; t < h->NBins(); t++) {
            double binc = h->GetBinContent(t);
            binc += weight * h->GetBinError(t);
            h->SetBinContent(t, binc);
        }
    }

    void EfficiencyScaleFactor::DebugPrint(void) {
        m_sf->GetHist()->Print();
        m_sf_sys->GetHist()->Print();
        if (m_sf_PtDepsys) m_sf_PtDepsys->GetHist()->Print();

        //Print only histograms if they are actually available
        if (m_eff) m_eff->GetHist()->Print();
        if (m_eff_sys) m_eff_sys->GetHist()->Print();
        if (m_eff_PtDepsys) m_eff_PtDepsys->GetHist()->Print();

        if (m_mc_eff) m_mc_eff->GetHist()->Print();
        if (m_mc_eff_sys) m_mc_eff_sys->GetHist()->Print();
    }

} /* namespace CP */
