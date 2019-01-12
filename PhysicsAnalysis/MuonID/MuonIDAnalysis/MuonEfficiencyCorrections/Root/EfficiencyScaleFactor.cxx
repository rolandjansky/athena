m/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "MuonEfficiencyCorrections/EfficiencyScaleFactor.h"
#include <TRandom3.h>
#include <TClass.h>
#ifdef  MUONEFFICIENCYCORRECITONS_EFFICIENCYSCALEFACTOR_H_
namespace CP {
    unsigned int EfficiencyScaleFactor::m_warningLimit = 10;
    /*EfficiencyScaleFactor::EfficiencyScaleFactor() :
                m_toolname(),
                m_sf(nullptr),
                m_eff(nullptr),
                m_mc_eff(nullptr),
                m_sf_sys(nullptr),
                m_eff_sys(nullptr),
                m_mc_eff_sys(nullptr),
                m_sf_KineDepsys(nullptr),
                m_eff_KineDepsys(nullptr),
                m_sf_replicas(),
                m_eff_replicas(),
                m_mc_eff_replicas(),
                m_sysType(),
                m_is_lowpt(false),
                m_respond_to_kineDepSyst(false),
                m_default_eff(1.),
                m_default_eff_ttva(1.),
                m_Type(CP::MuonEfficiencyType::Undefined),
                m_NominalFallBack(nullptr),
                m_SystematicBin(-1),
                m_warnsPrinted(0),
                m_firstRun(0),
                m_lastRun(999999) {
    }*/
    unsigned int EfficiencyScaleFactor::firstRun() const{
        return m_firstRun;
    }
    unsigned int EfficiencyScaleFactor::lastRun() const{
        return m_lastRun;
    }
    bool EfficiencyScaleFactor::coversRunNumber(unsigned int run) const{
        return m_firstRun <= run && run <= m_lastRun;
    }
    void EfficiencyScaleFactor::setFirstLastRun(unsigned int first, unsigned int last){
        m_firstRun = first;
        m_lastRun = last;
    }
    EfficiencyScaleFactor::EfficiencyScaleFactor(const std::string& toolname, const std::string &file, const std::string &time_unit, MuonEfficiencySystType sysType, CP::MuonEfficiencyType effType, bool isLowPt, bool hasPtDepSys) :
                EfficiencyScaleFactor() {
        m_toolname = toolname;
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
    EfficiencyScaleFactor::EfficiencyScaleFactor(EfficiencyScaleFactor* Nominal, const std::string& toolname, const std::string &file, const std::string &time_unit, MuonEfficiencySystType sysType, CP::MuonEfficiencyType effType, bool is_lowpt, bool hasPtDepSys) :
                EfficiencyScaleFactor::EfficiencyScaleFactor(toolname, file, time_unit, sysType, effType, is_lowpt, hasPtDepSys) {
        m_NominalFallBack = Nominal;

    }

    bool EfficiencyScaleFactor::ReadFromFile(const std::string& file, const std::string& time_unit) {
        // open the file
        std::unique_ptr<TFile> f (TFile::Open(file.c_str(), "READ"));
        if (!f || !f->IsOpen()) {
            Error("EfficiencyScaleFactor", "Unable to open file %s", file.c_str());
            return false;
        }
        // now we can read our six Histos
        m_eff = ReadHistFromFile("Eff", f.get(), time_unit);
        m_eff_sys = ReadHistFromFile("Eff_sys", f.get(), time_unit);

        m_mc_eff = ReadHistFromFile("MC_Eff", f.get(), time_unit);
        m_mc_eff_sys = ReadHistFromFile("MC_Eff_sys", f.get(), time_unit);

        m_sf = ReadHistFromFile("SF", f.get() ,time_unit);
        m_sf_sys = ReadHistFromFile("SF_sys", f.get(), time_unit);

        // for high pt eff, we also load the pt dependent part
//         if (m_respond_to_kineDepSyst) {
//             if (m_Type != CP::MuonEfficiencyType::BadMuonVeto) {
//                 m_sf_KineDepsys = IKinematicSystHandler_Ptr(new PtDependentSystHandler(ReadHistFromFile("SF_PtDep_sys", f.get(), time_unit)));
//                 m_eff_KineDepsys = IKinematicSystHandler_Ptr(new PtDependentSystHandler(ReadHistFromFile("Eff_PtDep_sys", f.get(), time_unit)));
//             } else {
//                 TDirectory* SystDir = nullptr;
//                 f->GetObject(("KinematicSystHandler_" + time_unit).c_str(), SystDir);
//                 m_sf_KineDepsys = IKinematicSystHandler_Ptr(new BadMuonVetoSystHandler(SystDir));
//                 m_eff_KineDepsys = IKinematicSystHandler_Ptr(new BadMuonVetoSystHandler(SystDir));
//             }
// 
//         }
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
            Error("EfficiencyScaleFactor", "The EfficiencyScaleFactor %shas itself as Nominal Fall back.", EfficiencyTypeName(m_Type).c_str());
            return false;
        }
        return m_sf_sys != nullptr && m_sf != nullptr;
    }
    std::unique_ptr<HistHandler> EfficiencyScaleFactor::ReadHistFromFile(const std::string& name, TFile* f, const std::string& time_unit) {
        
        TH1* hist_from_file =  nullptr;
        
        f->GetObject((name + std::string("_") + time_unit).c_str(), histHolder);
         // if no period weighting, the histo may also not have a time period in the name at all
           
        if (!histHolder && time_unit == "All") {
            f->GetObject(name.c_str(), histHolder);
        }
        if (!histHolder) {
            return std::unique_ptr<HistHandler>();
        }
        return package_histo(hist_from_file);
    }
    std::unique_ptr<HistHandler> EfficiencyScaleFactor::package_histo(TH1* h) {
        // make sure that the correct type of histo is used
        // Dynamic cast for TH2 Poly otherwise we can rely on the GetDimension() ,ethod
        if (dynamic_cast<TH2Poly*>(h)) {
            return std::make_unique<HistHandler_TH2Poly>(dynamic_cast<TH2Poly*>(h));
        }else if (h->GetDimension() == 3) {
            return std::make_unique<HistHandler_TH3>(h);
        }else if (h->GetDimension() == 2) {
            return std::make_unique<HistHandler_TH2>(h);
        } else if (h->GetDimension() == 1)) {
            return std::make_unique<HistHandler_TH1>(h);
        } else {
            Error("EfficiencyScaleFactor", "Unable to package histo %s (%s) in a known HistHandler", h->GetName(), h->IsA()->GetName());
            return std::unique_ptr<HistHandler>();
        }
    }
    int EfficiencyScaleFactor::nBinsSF() const {
        return m_sf ? m_sf->NBins() : -1;
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
        CorrectionCode cc = GetContentFromHist(m_mc_eff, IKinematicSystHandler_Ptr(), mu, Eff, false);
        if (cc == CorrectionCode::Error) Error("EfficiencyScaleFactor", "Could not apply the Monte Carlo efficiency");
        return cc;
    }

    CorrectionCode EfficiencyScaleFactor::GetContentFromHist(std::unique_ptr<HistHandler> Hist, IKinematicSystHandler_Ptr PtDepHist, const xAOD::Muon& mu, float & Eff, bool PtDepHistNeeded) const {
        Eff = m_default_eff;
        if (!Hist) {
            if (m_warnsPrinted < m_warningLimit){
                Warning("EfficiencyScaleFactor", "Could not find histogram for variation %s and muon with pt=%.4f, eta=%.2f and phi=%.2f, returning %.1f", sysname().c_str(), mu.pt(), mu.eta(), mu.phi(), m_default_eff);
                ++m_warnsPrinted;
            }
            return CorrectionCode::OutOfValidityRange;
        }
        if (m_Type == CP::MuonEfficiencyType::TTVA && fabs(mu.eta()) > 2.5 && fabs(mu.eta()) <= 2.7 && mu.muonType() == xAOD::Muon::MuonType::MuonStandAlone) {
            if (m_warnsPrinted < m_warningLimit){
                Info("EfficiencyScaleFactor", "No TTVA sf/efficiency provided for standalone muons with 2.5<|eta|<2.7 for variation %s and muon with pt=%.4f, eta=%.2f and phi=%.2f, returning %.1f", sysname().c_str(), mu.pt(), mu.eta(), mu.phi(), m_default_eff_ttva);
                ++m_warnsPrinted;
            }
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
            if (m_warnsPrinted < m_warningLimit){
                Info("EfficiencyScaleFactor", "No TTVA sf/efficiency provided for standalone muons with 2.5<|eta|<2.7 for variation %s and muon with pt=%.4f, eta=%.2f and phi=%.2f, returning %.1f", sysname().c_str(), mu.pt(), mu.eta(), mu.phi(), m_default_eff_ttva);
                ++m_warnsPrinted;
            }
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
    void EfficiencyScaleFactor::GenerateReplicas(int nrep, int seed) {
        GenerateReplicasFromHist(m_eff.get(), nrep, seed, m_eff_replicas);
        GenerateReplicasFromHist(m_sf.get(), nrep, seed, m_sf_replicas);
        GenerateReplicasFromHist(m_mc_eff.get(), nrep, seed, m_mc_eff_replicas);
    }
    void EfficiencyScaleFactor::GenerateReplicasFromHist(HistHandler* h, int nrep, int seed, EfficiencyScaleFactor::SFvec &replicas) {
        if (!h) return;
        TRandom3 Rndm(seed);
        replicas.clear();
        replicas.resize(nrep);
        int nbins = h->NBins();
        for (int t = 0; t < nrep; t++) {
            std::unique_ptr<HistHandler> replica = package_histo((TH1*) h->GetHist()->Clone(Form("rep%d_%s", t, h->GetHist()->GetName())));
            for (int bin = 0; bin < nbins; bin++) {
                replica->SetBinContent(bin, Rndm.Gaus(h->GetBinContent(bin), h->GetBinError(bin)));
            }
            replicas.push_back(replica);
        }
    }
    void EfficiencyScaleFactor::ApplySysVariation() {
        if (m_sysType == MuonEfficiencySystType::Nominal) return;
        //For reconstruction effiecency scale-factors disentangle the low-pt from the high pt systematics
        else if ((m_Type != CP::MuonEfficiencyType::Reco || !m_is_lowpt) && m_sysType == MuonEfficiencySystType::Sys1Down) AddSysErrors(-1.);
        else if ((m_Type != CP::MuonEfficiencyType::Reco || !m_is_lowpt) && m_sysType == MuonEfficiencySystType::Sys1Up) AddSysErrors(1.);
        else if ((m_Type != CP::MuonEfficiencyType::Reco || !m_is_lowpt) && m_sysType == MuonEfficiencySystType::Stat1Down) AddStatErrors(-1.);
        else if ((m_Type != CP::MuonEfficiencyType::Reco || !m_is_lowpt) && m_sysType == MuonEfficiencySystType::Stat1Up) AddStatErrors(1.);
        else if (m_is_lowpt && m_sysType == MuonEfficiencySystType::LowPtSys1Down) AddSysErrors(-1.);
        else if (m_is_lowpt && m_sysType == MuonEfficiencySystType::LowPtSys1Up) AddSysErrors(1.);
        else if (m_is_lowpt && m_sysType == MuonEfficiencySystType::LowPtStat1Down) AddStatErrors(-1.);
        else if (m_is_lowpt && m_sysType == MuonEfficiencySystType::LowPtStat1Up) AddStatErrors(1.);
    }
    void EfficiencyScaleFactor::AddStatErrors(float weight) {
        AddStatErrors_histo(m_sf, weight);
        AddStatErrors_histo(m_eff, weight);
        AddStatErrors_histo(m_mc_eff, weight);
    }

    void EfficiencyScaleFactor::AddSysErrors(float weight) {

        // turn on the pt dependent contribution, if there is any

//         if (m_respond_to_kineDepSyst) {
//             if (m_sf_KineDepsys) m_sf_KineDepsys->SetSystematicWeight(weight);
//             if (m_eff_KineDepsys) m_eff_KineDepsys->SetSystematicWeight(weight);
//         }
        AddSysErrors_histo(m_sf, m_sf_sys, weight);
        AddSysErrors_histo(m_eff, m_eff_sys, weight);
        AddSysErrors_histo(m_mc_eff, m_mc_eff_sys, weight);

        AddSysErrors_vector(m_sf_replicas, m_sf_sys, weight);
        AddSysErrors_vector(m_eff_replicas, m_sf_sys, weight);
        AddSysErrors_vector(m_mc_eff_replicas, m_sf_sys, weight);

    }
//    void EfficiencyScaleFactor::AddSysErrors_vector(EfficiencyScaleFactor::SFvec &Vec, std::unique_ptr<HistHandler> hsys, float weight) {
//         for (auto Histo : Vec) {
//             AddSysErrors_histo(Histo, hsys, weight);
//         }
//    }

    void EfficiencyScaleFactor::DebugPrint() const {
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
#endif
