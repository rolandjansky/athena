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
                m_sf_KineDepsys(0),
                m_eff_KineDepsys(0),
                m_sf_replicas(),
                m_eff_replicas(),
                m_mc_eff_replicas(),
                m_etaphi(),
                m_sysType(),
                m_is_lowpt(false),
                m_respond_to_kineDepSyst(false),
                m_default_eff(1.),
                m_default_eff_ttva(1.),
                m_Type(CP::MuonEfficiencyType::Undefined),
                m_NominalFallBack(nullptr),
                m_SystematicBin(-1) {
    }

    EfficiencyScaleFactor::EfficiencyScaleFactor(const std::string &file, const std::string &time_unit, MuonEfficiencySystType sysType, CP::MuonEfficiencyType effType, bool isLowPt, bool hasPtDepSys) :
                EfficiencyScaleFactor() {
        m_sysType = sysType;
        m_Type = effType;
        m_is_lowpt = isLowPt;
        m_respond_to_kineDepSyst = hasPtDepSys;
        ReadFromFile(file, time_unit);
        if ((sysname().find("up") != std::string::npos) || (sysname().find("Up") != std::string::npos)) {
            m_default_eff = 2.;
            m_default_eff_ttva = 1. + 1.e-9;
        } else if ((sysname().find("down") != std::string::npos) || (sysname().find("Down") != std::string::npos)) {
            m_default_eff = 0.;
            m_default_eff_ttva = 1. - 1.e-9;
        }
    }
    EfficiencyScaleFactor::EfficiencyScaleFactor(EfficiencyScaleFactor* Nominal, const std::string &file, const std::string &time_unit, MuonEfficiencySystType sysType, CP::MuonEfficiencyType effType, bool is_lowpt, bool hasPtDepSys) :
                EfficiencyScaleFactor::EfficiencyScaleFactor(file, time_unit, sysType, effType, is_lowpt, hasPtDepSys) {
        m_NominalFallBack = Nominal;

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
        m_sysType = other.m_sysType;
        m_is_lowpt = other.m_is_lowpt;
        m_respond_to_kineDepSyst = other.m_respond_to_kineDepSyst;
        m_Type = other.m_Type;
        CopyHistHandler(m_sf, other.m_sf);
        CopyHistHandler(m_eff, other.m_eff);
        CopyHistHandler(m_mc_eff, other.m_mc_eff);

        CopyHistHandler(m_sf_sys, other.m_sf_sys);
        CopyHistHandler(m_eff_sys, other.m_eff_sys);
        CopyHistHandler(m_mc_eff_sys, other.m_mc_eff_sys);

        if (other.m_respond_to_kineDepSyst) {
            if (m_sf_KineDepsys) {
                (*m_sf_KineDepsys) = (*other.m_sf_KineDepsys);
            }
            if (m_eff_KineDepsys) {
                (*m_eff_KineDepsys) = (*other.m_eff_KineDepsys);
            }
        }
        CopyReplicaVec(m_sf_replicas, other.m_sf_replicas);
        CopyReplicaVec(m_eff_replicas, other.m_eff_replicas);
        CopyReplicaVec(m_mc_eff_replicas, other.m_mc_eff_replicas);
        m_NominalFallBack = other.m_NominalFallBack;
        m_SystematicBin = other.m_SystematicBin;
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
        if (m_respond_to_kineDepSyst) {
            if (m_Type != CP::MuonEfficiencyType::BadMuonVeto) {
                m_sf_KineDepsys = new PtDependentSystHandler(ReadHistFromFile("SF_PtDep_sys", f, time_unit));
                m_eff_KineDepsys = new PtDependentSystHandler(ReadHistFromFile("Eff_PtDep_sys", f, time_unit));
            } else {
                TDirectory* SystDir = nullptr;
                f->GetObject(("KinematicSystHandler_" + time_unit).c_str(), SystDir);
                m_sf_KineDepsys = new BadMuonVetoSystHandler(SystDir);
                m_eff_KineDepsys = new BadMuonVetoSystHandler(SystDir);
            }

        }
        // and don't forget to close the file again!
        f->Close();
        delete f;
        return true;
    }
    bool EfficiencyScaleFactor::CheckConsistency() const {
        //Check whether  the SFs could be successfully loaded
        if (!m_sf) Error("EfficiencyScaleFactor", "%s", ("Could not load the SF for " + EfficiencyTypeName(m_Type) + " and systematic " + EfficiencySystName(m_sysType)).c_str());
        if (!m_sf_sys) Error("EfficiencyScaleFactor", "%s", ("Could not load the SF systematic for " + EfficiencyTypeName(m_Type) + " and systematic " + EfficiencySystName(m_sysType)).c_str());
        if (m_respond_to_kineDepSyst && !m_sf_KineDepsys->initialize()) {
            Error("EfficiencyScaleFactor", "%s", ("Could not load the SF pt-dependent systematic for " + EfficiencyTypeName(m_Type) + " and systematic " + EfficiencySystName(m_sysType)).c_str());
            return false;
        }
        if (m_NominalFallBack == this) {
            Error("EfficiencyScaleFactor", "%s", ("The EfficiencyScaleFactor " + EfficiencyTypeName(m_Type) + " has itself as Nominal Fall back").c_str());
            return false;
        }
        return m_sf_sys != nullptr && m_sf != nullptr;
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
            return new HistHandler_TH1(dynamic_cast<TH1F*>(h));
        } else if (dynamic_cast<TH2F*>(h)) {
            return new HistHandler_TH2(dynamic_cast<TH2F*>(h));
        } else if (dynamic_cast<TH3F*>(h)) {
            return new HistHandler_TH3(dynamic_cast<TH3F*>(h));
        } else if (dynamic_cast<TH2Poly*>(h)) {
            return new HistHandler_TH2Poly(dynamic_cast<TH2Poly*>(h));
        } else {
            Error("EfficiencyScaleFactor", "Unable to package histo %s (%s) in a known HistHandler", h->GetName(), h->IsA()->GetName());
            return 0;
        }
    }
    int EfficiencyScaleFactor::nBinsSF() const {
        if (m_sf) {
            return m_sf->NBins();
        }
        return -1;
    }
    int EfficiencyScaleFactor::nBinsEfficiency() const {
        if (m_eff) {
            return m_eff->NBins();
        }
        if (m_mc_eff) {
            return m_mc_eff->NBins();
        }
        return -1;
    }
    std::string EfficiencyScaleFactor::sysname() const {
        return EfficiencySystName(m_sysType);
    }
    CorrectionCode EfficiencyScaleFactor::ScaleFactor(const xAOD::Muon& mu, float & SF) const {
        if (m_NominalFallBack) {
            int bin = -1;
            CorrectionCode cc = m_sf->FindBin(mu, bin);
            if (cc == CP::CorrectionCode::Error) {
                return cc;
            }
            if (bin != m_SystematicBin) {
                return m_NominalFallBack->ScaleFactor(mu, SF);
            }
        }
        CorrectionCode cc = GetContentFromHist(m_sf, m_sf_KineDepsys, mu, SF, m_respond_to_kineDepSyst);
        if (cc == CorrectionCode::Error) Error("EfficiencyScaleFactor", "Could not apply the scale factor");
        return cc;
    }

    CorrectionCode EfficiencyScaleFactor::DataEfficiency(const xAOD::Muon& mu, float & Eff) const {
        if (m_NominalFallBack && m_eff) {
            int bin = -1;
            CorrectionCode cc = m_eff->FindBin(mu, bin);
            if (cc == CP::CorrectionCode::Error) {
                return cc;
            }
            if (bin != m_SystematicBin) {
                return m_NominalFallBack->DataEfficiency(mu, Eff);
            }
        }
        CorrectionCode cc = GetContentFromHist(m_eff, m_eff_KineDepsys, mu, Eff, m_respond_to_kineDepSyst);
        if (cc == CorrectionCode::Error) Error("EfficiencyScaleFactor", "Could not apply the data efficiency");
        return cc;
    }
    CorrectionCode EfficiencyScaleFactor::MCEfficiency(const xAOD::Muon& mu, float & Eff) const {
        if (m_NominalFallBack && m_mc_eff) {
            int bin = -1;
            CorrectionCode cc = m_mc_eff->FindBin(mu, bin);
            if (cc == CP::CorrectionCode::Error) {
                return cc;
            }
            if (bin != m_SystematicBin) {
                return m_NominalFallBack->MCEfficiency(mu, Eff);
            }
        }
        CorrectionCode cc = GetContentFromHist(m_mc_eff, nullptr, mu, Eff, false);
        if (cc == CorrectionCode::Error) Error("EfficiencyScaleFactor", "Could not apply the Monte Carlo efficiency");
        return cc;
    }

    CorrectionCode EfficiencyScaleFactor::GetContentFromHist(HistHandler* Hist, IKinematicSystHandler* PtDepHist, const xAOD::Muon& mu, float & Eff, bool PtDepHistNeeded) const {
        Eff = m_default_eff;
        if (!Hist) {
            Warning("EfficiencyScaleFactor", "Could not find histogram for variation %s and muon with pt=%.4f, eta=%.2f and phi=%.2f, returning %.1f", sysname().c_str(), mu.pt(), mu.eta(), mu.phi(), m_default_eff);
            return CorrectionCode::OutOfValidityRange;
        }
        if (m_Type == CP::MuonEfficiencyType::TTVA && fabs(mu.eta()) > 2.5 && fabs(mu.eta()) <= 2.7 && mu.muonType() == xAOD::Muon::MuonType::MuonStandAlone) {
            static bool Warned = false;
            if (!Warned) Info("EfficiencyScaleFactor", "No TTVA sf/efficiency provided for standalone muons with 2.5<|eta|<2.7 for variation %s and muon with pt=%.4f, eta=%.2f and phi=%.2f, returning %.1f", sysname().c_str(), mu.pt(), mu.eta(), mu.phi(), m_default_eff_ttva);
            Warned = true;
            Eff = m_default_eff_ttva;
            return CorrectionCode::Ok;
        }
        int bin = -1;
        CorrectionCode cc = Hist->FindBin(mu, bin);
        if (cc != CorrectionCode::Ok) return cc;
        else Eff = Hist->GetBinContent(bin);
        if (PtDepHistNeeded) {
            return PtDepHist->GetKineDependent(mu, Eff);
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
            if (!Warned) Info("EfficiencyScaleFactor", "No TTVA sf/efficiency provided for standalone muons with 2.5<|eta|<2.7 for variation %s and muon with pt=%.4f, eta=%.2f and phi=%.2f, returning %.1f", sysname().c_str(), mu.pt(), mu.eta(), mu.phi(), m_default_eff_ttva);
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
            old = nullptr;
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
        if (m_sf_KineDepsys) delete m_sf_KineDepsys;
        if (m_eff_KineDepsys) delete m_eff_KineDepsys;
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
        if (m_sysType == MuonEfficiencySystType::Nominal) return;
        else if (m_sysType == MuonEfficiencySystType::Sys1Down) AddSysErrors(-1.);
        else if (m_sysType == MuonEfficiencySystType::Sys1Up) AddSysErrors(1.);
        else if (m_sysType == MuonEfficiencySystType::Stat1Down) AddStatErrors(-1.);
        else if (m_sysType == MuonEfficiencySystType::Stat1Up) AddStatErrors(1.);
        else if (m_sysType == MuonEfficiencySystType::LowPtSys1Down) AddSysErrors(-1.);
        else if (m_sysType == MuonEfficiencySystType::LowPtSys1Up) AddSysErrors(1.);
        else if (m_sysType == MuonEfficiencySystType::LowPtStat1Down) AddStatErrors(-1.);
        else if (m_sysType == MuonEfficiencySystType::LowPtStat1Up) AddStatErrors(1.);
    }
    void EfficiencyScaleFactor::AddStatErrors(float weight) {
        AddStatErrors_histo(m_sf, weight);
        AddStatErrors_histo(m_eff, weight);
        AddStatErrors_histo(m_mc_eff, weight);
    }

    void EfficiencyScaleFactor::AddSysErrors(float weight) {

        // turn on the pt dependent contribution, if there is any

        if (m_respond_to_kineDepSyst) {
            if (m_sf_KineDepsys) m_sf_KineDepsys->SetSystematicWeight(weight);
            if (m_eff_KineDepsys) m_eff_KineDepsys->SetSystematicWeight(weight);
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

        //Print only histograms if they are actually available
        if (m_eff) m_eff->GetHist()->Print();
        if (m_eff_sys) m_eff_sys->GetHist()->Print();

        if (m_mc_eff) m_mc_eff->GetHist()->Print();
        if (m_mc_eff_sys) m_mc_eff_sys->GetHist()->Print();
    }
    bool EfficiencyScaleFactor::SetSystematicBin(int bin) {
        if (!m_NominalFallBack) {
            Error("EfficiencyScaleFactor::SetSystematicBin()", "No fallback has been given");
            return false;
        }
        if (bin < 1 || bin > nBinsSF()) {
            Error("EfficiencyScaleFactor::SetSystematicBin()", "The current bin %i is out of the maximum range %u ", bin, nBinsSF());
            return false;
        }
        m_SystematicBin = bin;
        return true;
    }
    std::string EfficiencyScaleFactor::GetBinName(int bin) const {
        if (bin < 1 || bin > nBinsSF()) {
            Error("EfficiencyScaleFactor::GetBinName()", "The current bin %i is out of the maximum range %u ", bin, nBinsSF());
            return "Out of range";
        }
        return m_sf->GetBinName(bin);
    }
    int EfficiencyScaleFactor::FindBinSF(const xAOD::Muon & mu) const {
        int bin = -1;
        CorrectionCode cc = m_sf->FindBin(mu, bin);
        if (cc == CP::CorrectionCode::Error) {
            return -1;
        }
        return bin;
    }

} /* namespace CP */
