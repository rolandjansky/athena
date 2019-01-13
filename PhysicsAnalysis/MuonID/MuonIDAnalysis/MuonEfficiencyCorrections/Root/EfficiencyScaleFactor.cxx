/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include <MuonEfficiencyCorrections/EfficiencyScaleFactor.h>
#include <MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h>
#include <MuonEfficiencyCorrections/EffiCollection.h>
#include <TRandom3.h>
#include <TClass.h>
namespace CP {
    unsigned int EfficiencyScaleFactor::m_warningLimit = 10;
    EfficiencyScaleFactor:: EfficiencyScaleFactor(std::shared_ptr<EfficiencyScaleFactor> nominal,
                                  const MuonEfficiencyScaleFactors& ref_tool,
                                  const std::string& file,
                                  const std::string& time_unit,
                                  const std::string& syst_name,
                                  int syst_type_bitmap):
            m_measurement(ref_tool.measurement()),
            m_syst_name(syst_name),
            m_is_up(syst_type_bitmap & EffiCollection::UpVariation),
            m_is_lowpt(syst_type_bitmap & EffiCollection::CentralLowPt ||  syst_type_bitmap & EffiCollection::CaloLowPt),
            m_respond_to_kineDepSyst(syst_type_bitmap & EffiCollection::PtDependent),
            m_seperateBinSyst(syst_type_bitmap & EffiCollection::UnCorrelated),
            m_sf(),
            m_eff(),
            m_mc_eff(),
            
            m_sf_decor(std::make_unique<FloatDecorator>(ref_tool.sf_decoration()+ (m_syst_name.empty()? "" :"_") + sysname(true))),
            m_eff_decor(std::make_unique<FloatDecorator>(ref_tool.data_effi_decoration()+ (m_syst_name.empty()? "" :"_") + sysname(true))),          
            m_mc_eff_decor(std::make_unique<FloatDecorator>(ref_tool.mc_effi_decoration()+ (m_syst_name.empty()? "" :"_") + sysname(true))),
            m_sf_rep_decor(std::make_unique<FloatVectorDecorator>(ref_tool.sf_replica_decoration()+ (m_syst_name.empty()? "" :"_") + sysname(true))),
            m_eff_rep_decor(std::make_unique<FloatVectorDecorator>(ref_tool.data_effi_replica_decoration()+ (m_syst_name.empty()? "" :"_") + sysname(true))),
            m_mc_eff_rep_decor(std::make_unique<FloatVectorDecorator>(ref_tool.mc_effi_replica_deocration()+ (m_syst_name.empty()? "" :"_") + sysname(true))),
          
            m_sf_KineDepsys(),
            m_eff_KineDepsys(),
            m_sf_replicas(),
            m_eff_replicas(),
            m_mc_eff_replicas(),
            m_NominalFallBack(nominal),
            m_SystematicBin(-1),
            m_default_eff(1.),
            m_default_eff_ttva(1.),
            m_warnsPrinted(0),
            m_firstRun(1),
            m_lastRun(999999){
    
            
        // open the file
        std::unique_ptr<TFile> f (TFile::Open(file.c_str(), "READ"));
        if (!f || !f->IsOpen()) {
            Error("EfficiencyScaleFactor", "Unable to open file %s", file.c_str());
            return;
        }
        // now we can read our three mean histograms Histos
        m_eff = ReadHistFromFile("Eff", f.get(), time_unit);
        m_mc_eff = ReadHistFromFile("MC_Eff", f.get(), time_unit);
        m_sf = ReadHistFromFile("SF", f.get() ,time_unit);
        /// Nominal set loaded nothing needs to be done further
        if (syst_name.empty()) return;
        
        std::function<void(std::unique_ptr<HistHandler>&, const std::string& )> syst_loader = [this, &f, &time_unit] (std::unique_ptr<HistHandler>& nominal, const std::string& hist_type) {
            if(!nominal) return;
            
            
        }
        //m_sf_sys = ReadHistFromFile("SF_sys", f.get(), time_unit);
        //m_mc_eff_sys = ReadHistFromFile("MC_Eff_sys", f.get(), time_unit);
        //m_eff_sys = ReadHistFromFile("Eff_sys", f.get(), time_unit);
    }
    EfficiencyScaleFactor::EfficiencyScaleFactor(const MuonEfficiencyScaleFactors& ref_tool,
                                  const std::string &file, 
                                  const std::string &time_unit):
                                  EfficiencyScaleFactor(std::shared_ptr<EfficiencyScaleFactor>(),
                                                        ref_tool,file, time_unit, "",0){}
           
    std::string EfficiencyScaleFactor::sysname(bool with_direction) const {
        return m_syst_name.empty() ? "" : EfficiencyTypeName(m_measurement) +  "_" + m_syst_name + (with_direction ? (m_is_up ?"__1UP" : "__1DN") :"");  
    }
    bool EfficiencyScaleFactor::SeperateSystBins() const {
        return m_seperateBinSyst;
    }
    bool EfficiencyScaleFactor::IsUpVariation() const{
        return m_is_up;
    }
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
    bool EfficiencyScaleFactor::CheckConsistency()  {
        //Check whether  the SFs could be successfully loaded
        if (!m_sf) {
            Error("EfficiencyScaleFactor()", "Could not load the SF for and systematic %s", sysname().c_str());
            return false;
        }
        if (m_respond_to_kineDepSyst && !m_sf_KineDepsys->initialize()) {
            Error("EfficiencyScaleFactor()", "Could not load the SF pt-dependent systematic for %s and systematic %s", EfficiencyTypeName(m_measurement).c_str(), sysname().c_str() );
            return false;
        }
        if (m_NominalFallBack.get() == this) {
            Error("EfficiencyScaleFactor()", "The EfficiencyScaleFactor %s has itself as Nominal Fall back.", EfficiencyTypeName(m_measurement).c_str());
            m_NominalFallBack.reset();
            return false;
        }
        if (m_NominalFallBack) {
            if (!m_NominalFallBack->sysname().empty() || sysname().empty()){
                Error("EfficiencyScaleFactor()", "Either nominal is assigned with a fall back or the nominal fall back is invald");
                return false;
            }
        }
        if (firstRun() > lastRun()){
            Error("EfficiencyScaleFactor()", "Invalid run number range. Since the map is ranging from %u to %u.", firstRun(), lastRun());
            return false;
        }
        return true;
    }
    std::unique_ptr<HistHandler> EfficiencyScaleFactor::ReadHistFromFile(const std::string& name, TFile* f, const std::string& time_unit) {
        
        TH1* hist_from_file =  nullptr;
        
        f->GetObject((name + std::string("_") + time_unit).c_str(), hist_from_file);
         // if no period weighting, the histo may also not have a time period in the name at all
           
        if (!hist_from_file && time_unit == "All") {
            f->GetObject(name.c_str(), hist_from_file);
        }
        if (!hist_from_file) {
            return std::unique_ptr<HistHandler>();
        }
        return package_histo(hist_from_file);
    }
    std::unique_ptr<HistHandler> EfficiencyScaleFactor::package_histo(TH1* h) {
        // make sure that the correct type of histo is used
        // Dynamic cast for TH2 Poly otherwise we can rely on the GetDimension() ,ethod
        if (!h) return std::unique_ptr<HistHandler>();
        if (dynamic_cast<TH2Poly*>(h)) {
            return std::make_unique<HistHandler_TH2Poly>(dynamic_cast<TH2Poly*>(h));
        }else if (h->GetDimension() == 3) {
            return std::make_unique<HistHandler_TH3>(h);
        }else if (h->GetDimension() == 2) {
            return std::make_unique<HistHandler_TH2>(h);
        } else if (h->GetDimension() == 1) {
            return std::make_unique<HistHandler_TH1>(h);
        } 
        Error("EfficiencyScaleFactor", "Unable to package histo %s (%s) in a known HistHandler", h->GetName(), h->IsA()->GetName());
        return std::unique_ptr<HistHandler>();
    }
    int EfficiencyScaleFactor::nBins() const {
        return m_sf ? m_sf->NBins() : -1;
    }
    CorrectionCode EfficiencyScaleFactor::ScaleFactor(const xAOD::Muon& mu, float & SF) const {
        if (m_seperateBinSyst && m_NominalFallBack) {
            int bin = -1;
            CorrectionCode cc = m_sf->FindBin(mu, bin);
            if (cc == CP::CorrectionCode::Error) {
                return cc;
            }
            if (bin != m_SystematicBin) {
                return m_NominalFallBack->ScaleFactor(mu, SF);
            }
        }
        CorrectionCode cc = GetContentFromHist(m_sf.get(), m_sf_KineDepsys.get(), mu, SF);
        if (cc == CorrectionCode::Error) Error("EfficiencyScaleFactor", "Could not apply the scale factor");
        return cc;
    }   

    CorrectionCode EfficiencyScaleFactor::DataEfficiency(const xAOD::Muon& mu, float & Eff) const {
        if (m_seperateBinSyst && m_NominalFallBack && m_eff) {
            int bin = -1;
            CorrectionCode cc = m_eff->FindBin(mu, bin);
            if (cc == CP::CorrectionCode::Error) {
                return cc;
            }
            if (bin != m_SystematicBin) {
                return m_NominalFallBack->DataEfficiency(mu, Eff);
            }
        }
        CorrectionCode cc = GetContentFromHist(m_eff.get(), m_eff_KineDepsys.get(), mu, Eff);
        if (cc == CorrectionCode::Error) Error("EfficiencyScaleFactor", "Could not apply the data efficiency");
        return cc;
    }
    CorrectionCode EfficiencyScaleFactor::MCEfficiency(const xAOD::Muon& mu, float & Eff) const {
        if (m_seperateBinSyst && m_NominalFallBack && m_mc_eff) {
            int bin = -1;
            CorrectionCode cc = m_mc_eff->FindBin(mu, bin);
            if (cc == CP::CorrectionCode::Error) {
                return cc;
            }
            if (bin != m_SystematicBin) {
                return m_NominalFallBack->MCEfficiency(mu, Eff);
            }
        }
        CorrectionCode cc = GetContentFromHist(m_mc_eff.get(), nullptr, mu, Eff);
        if (cc == CorrectionCode::Error) Error("EfficiencyScaleFactor", "Could not apply the Monte Carlo efficiency");
        return cc;
    }

    CorrectionCode EfficiencyScaleFactor::GetContentFromHist(HistHandler* Hist, IKinematicSystHandler* PtDepHist, const xAOD::Muon& mu, float & Eff) const {
        Eff = m_default_eff;
        if (!Hist) {
            if (m_warnsPrinted < m_warningLimit){
                Warning("EfficiencyScaleFactor", "Could not find histogram for variation %s and muon with pt=%.4f, eta=%.2f and phi=%.2f, returning %.1f", sysname().c_str(), mu.pt(), mu.eta(), mu.phi(), m_default_eff);
                ++m_warnsPrinted;
            }
            return CorrectionCode::OutOfValidityRange;
        }
        if (m_measurement == CP::MuonEfficiencyType::TTVA && fabs(mu.eta()) > 2.5 && fabs(mu.eta()) <= 2.7 && mu.muonType() == xAOD::Muon::MuonType::MuonStandAlone) {
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
        if (PtDepHist) {
            return PtDepHist->GetKineDependent(mu, Eff);
        }
        return CorrectionCode::Ok;
    }
    CorrectionCode EfficiencyScaleFactor::ApplyScaleFactor(const xAOD::Muon& mu) const{
        float sf = 0;
        CorrectionCode result = ScaleFactor(mu, sf);
        (*m_sf_decor)(mu) = sf;
        return result;  
    }
    CorrectionCode EfficiencyScaleFactor::ApplyScaleFactorReplicas(const xAOD::Muon& mu, int n_replicas) {
        std::vector<float> replicas(n_replicas);
        CorrectionCode result = ScaleFactorReplicas(mu, replicas);
        (*m_sf_rep_decor)(mu)= replicas;
        return result;
    }
    
    CorrectionCode EfficiencyScaleFactor::ApplyDataEfficiency(const xAOD::Muon& mu) const {
        float effi = 0;
        CorrectionCode result = DataEfficiency(mu, effi);
        (*m_eff_decor)(mu) = effi;
        return result;  
    }
    CorrectionCode EfficiencyScaleFactor::ApplyDataEfficiencyReplicas(const xAOD::Muon& mu, int n_replicas){
        std::vector<float> replicas(n_replicas);
        CorrectionCode result = DataEfficiencyReplicas(mu, replicas);
        (*m_eff_rep_decor)(mu) = replicas;
        return result;
    }
   CorrectionCode EfficiencyScaleFactor::ApplyMCEfficiency(const xAOD::Muon& mu) const {
        float effi = 0;
        CorrectionCode result = MCEfficiency(mu, effi);
        (*m_mc_eff_decor)(mu) = effi;
        return result;  
    }
    CorrectionCode EfficiencyScaleFactor::ApplyMCEfficiencyReplicas(const xAOD::Muon& mu, int n_replicas){
        std::vector<float> replicas(n_replicas);
        CorrectionCode result = MCEfficiencyReplicas(mu, replicas);
        (*m_mc_eff_rep_decor)(mu) = replicas;
        return result;
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
    CorrectionCode EfficiencyScaleFactor::GetContentReplicasFromHist(EfficiencyScaleFactor::SFReplicaVec &replicas, const xAOD::Muon& mu, std::vector<float> & SF) {
        if (replicas.size() != SF.size()) GenerateReplicas(SF.size(), 1000. * mu.phi() + mu.eta());
        if (replicas.empty()) return CorrectionCode::OutOfValidityRange;
        if (m_measurement == CP::MuonEfficiencyType::TTVA && fabs(mu.eta()) > 2.5 && fabs(mu.eta()) <= 2.7 && mu.muonType() == xAOD::Muon::MuonType::MuonStandAlone) {
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
    void EfficiencyScaleFactor::GenerateReplicasFromHist(HistHandler* h, int nrep, int seed, EfficiencyScaleFactor::SFReplicaVec &replicas) {
        if (!h) return;
        TRandom3 Rndm(seed);
        replicas.clear();
        replicas.resize(nrep);
        int nbins = h->NBins();
        for (int t = 0; t < nrep; t++) {
            replicas.push_back(package_histo(h->GetHist()));
            HistHandler* replica = replicas.back().get();
            for (int bin = 0; bin < nbins; bin++) {
                replica->SetBinContent(bin, Rndm.Gaus(h->GetBinContent(bin), h->GetBinError(bin)));
            }            
        }
    }
    
    bool EfficiencyScaleFactor::SetSystematicBin(int bin) {
        if (!m_NominalFallBack) {
            Error("EfficiencyScaleFactor::SetSystematicBin()", "No fallback has been given");
            return false;
        }
        if (!m_seperateBinSyst || bin < 1 || bin > nBins()) {
            Error("EfficiencyScaleFactor::SetSystematicBin()", "The current bin %i is out of the maximum range %u ", bin, nBins());
            return false;
        }
        m_SystematicBin = bin;
        return true;
    }
    std::string EfficiencyScaleFactor::GetBinName(int bin) const {
        if (bin < 1 || bin > nBins()) {
            Error("EfficiencyScaleFactor::GetBinName()", "The current bin %i is out of the maximum range %u ", bin, nBins());
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
