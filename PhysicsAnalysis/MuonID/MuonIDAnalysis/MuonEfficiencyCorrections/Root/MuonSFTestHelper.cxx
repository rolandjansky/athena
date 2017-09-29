/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "MuonEfficiencyCorrections/MuonSFTestHelper.h"
#include "PATInterfaces/SystematicsUtil.h"
#include <TH1D.h>
#define CHECK_CPCorr(Arg) \
    if (Arg.code() == CP::CorrectionCode::Error){    \
        Error(#Arg,"Correction Code 'Error' (returned in line %i) ",__LINE__); \
        return CP::CorrectionCode::Error;   \
    } 

namespace TestMuonSF {

    void WriteHistogram(TFile* File, TH1* &Histo) {
        if (!Histo) {
            return;
        }
        File->cd();
        Histo->Write();
        delete Histo;
        Histo = nullptr;
    }
   //###########################################################
   //                       SFBranches
   //###########################################################
   SFBranches::SFBranches(TTree* tree):
        m_tree(tree){
    }
    SFBranches::~SFBranches(){
    }
   //############################################################
   //                   TriggerSFBranches
   //############################################################
   TriggerSFBranches::TriggerSFBranches(TTree* tree,const  ToolHandle<CP::IMuonTriggerScaleFactors>& Handle, const std::string& Trigger):
                    SFBranches(tree),
                    m_handle(Handle),
                    m_trigger(Trigger),
                    m_nominal_SF(1.),
                    m_stat_up_SF(1.),
                    m_stat_down_SF(1.),
                    m_sys_up_SF(1.),
                    m_sys_down_SF(1.){}

    CP::CorrectionCode TriggerSFBranches::fill(const xAOD::MuonContainer* muons){
        if (getSF(muons, m_nominal_SF, CP::SystematicVariation("",0)) == CP::CorrectionCode::Error) return CP::CorrectionCode::Error;
        if (getSF(muons, m_stat_up_SF, CP::SystematicVariation("MUON_EFF_TrigStatUncertainty", +1)) == CP::CorrectionCode::Error) return CP::CorrectionCode::Error;
        if (getSF(muons, m_stat_down_SF, CP::SystematicVariation("MUON_EFF_TrigStatUncertainty",-1)) == CP::CorrectionCode::Error) return CP::CorrectionCode::Error;
        if (getSF(muons, m_sys_up_SF, CP::SystematicVariation("MUON_EFF_TrigSystUncertainty", +1)) == CP::CorrectionCode::Error) return CP::CorrectionCode::Error;
        if (getSF(muons, m_sys_down_SF, CP::SystematicVariation("MUON_EFF_TrigSystUncertainty",-1)) == CP::CorrectionCode::Error) return CP::CorrectionCode::Error;
        return CP::CorrectionCode::Ok;
    }
    bool TriggerSFBranches::init(){
        if (!initBranch(m_nominal_SF,"")) return false;
        if (!initBranch(m_stat_up_SF, "STAT_UP")) return false;
        if (!initBranch(m_stat_down_SF, "STAT_DOWN")) return false;
        if (!initBranch(m_sys_up_SF, "SYS_UP")) return false;
        if (!initBranch(m_sys_down_SF, "SYS_DOWN")) return false;
        return true;
    }
    std::string TriggerSFBranches::name() const{
        return m_trigger;
    }
    CP::CorrectionCode TriggerSFBranches::getSF(const xAOD::MuonContainer* muons, double &Var, const CP::SystematicVariation &syst){
        if (muons->empty()){
            return CP::CorrectionCode::Ok;
        }
        CP::SystematicSet syst_set;
        syst_set.insert(syst);
        if (m_handle->applySystematicVariation(syst_set) != CP::SystematicCode::Ok) {
            return CP::CorrectionCode::Error;
        }
        return m_handle->getTriggerScaleFactor(*muons, Var, name());
    }
   
   //############################################################
   //                   MuonEffiBranches
   //############################################################
   MuonEffiBranches::MuonEffiBranches(TTree* tree, const ToolHandle<CP::IMuonEfficiencyScaleFactors> &handle):
            SFBranches(tree),
            m_handle(handle),
            m_SFs(){
   }
   CP::CorrectionCode MuonEffiBranches::fill(const xAOD::Muon* muon){
        return fill(*muon);
    }
    CP::CorrectionCode MuonEffiBranches::fill (const xAOD::Muon& muon){
        for (auto& Syst_SF : m_SFs){
            if (m_handle->applySystematicVariation(Syst_SF.first) != CP::SystematicCode::Ok) {
                return CP::CorrectionCode::Error;
            }
            CP::CorrectionCode cc = m_handle->getEfficiencyScaleFactor(muon, Syst_SF.second.scale_factor);
            if (cc == CP::CorrectionCode::Error) return CP::CorrectionCode::Error;
//             else if (cc != CP::CorrectionCode::Ok) Syst_SF.second.scale_factor = FLT_MAX;

            cc = m_handle->getDataEfficiency(muon, Syst_SF.second.data_eff);
            if (cc == CP::CorrectionCode::Error) return CP::CorrectionCode::Error;
//             else if (cc != CP::CorrectionCode::Ok) Syst_SF.second.data_eff = FLT_MAX;

            cc = m_handle->getMCEfficiency(muon, Syst_SF.second.mc_eff);
            if (cc == CP::CorrectionCode::Error) return CP::CorrectionCode::Error;
//             else if (cc != CP::CorrectionCode::Ok) Syst_SF.second.mc_eff = FLT_MAX;            
        } 
        return CP::CorrectionCode::Ok;
    }
    std::string MuonEffiBranches::name() const{
        return getProperty<std::string>(m_handle.operator->(), "WorkingPoint");
    }
    bool MuonEffiBranches::init(){
//         for (const auto& set : CP::make_systematics_vector(m_handle->recommendedSystematics())) {
        return true;
    }
    //###########################################################
    //                  MuonSFTestHelper
    //###########################################################
    MuonSFTestHelper::MuonSFTestHelper(const std::string& Name) :
                    m_Tool(Name),
                    m_Nominal(),
                    m_Syst(),
                    m_Histos(),
                    m_init(false),
                    m_Print(false),
                    m_FillHistos(true) {

    }
    void MuonSFTestHelper::PrintSFs(bool B) {
        m_Print = B;
    }
    void MuonSFTestHelper::FillHistos(bool B) {
        m_FillHistos = B;
    }

    StatusCode MuonSFTestHelper::initialize() {
        if (m_init) {
            return StatusCode::SUCCESS;
        }
        if (!m_Tool.setProperty("ScaleFactorDecorationName", m_Tool.name()).isSuccess()) {
            Error("MuonSFTestHelper", "Failed to set ScaleFactorDecorationName property of the tool %s", m_Tool.name().c_str());
            return StatusCode::FAILURE;
        }
        if (!m_Tool.initialize().isSuccess()) {
            Error("MuonSFTestHelper", "Failed to initialize the tool %s", m_Tool.name().c_str());
            return StatusCode::FAILURE;
        }
        //Now fill the systematic vectors
        for (const auto& set : CP::make_systematics_vector(m_Tool.recommendedSystematics())) {
            if (set.name().empty()) continue;
            m_Syst.push_back(new CP::SystematicSet(set));
        }
        m_Syst.push_back(new CP::SystematicSet(m_Nominal));
        for (auto Sys : m_Syst) {
            CreateHistogram(*Sys);
        }
        m_init = true;
        return StatusCode::SUCCESS;
    }
    void MuonSFTestHelper::CreateHistogram(CP::SystematicSet& Set) {
        TestScaleFactors Histo;
        Histo.SF = nullptr;
        Histo.eff = nullptr;
        Histo.mceff = nullptr;
        Histo.relSystSF = nullptr;
        Histo.relSystEff = nullptr;
        Histo.relSystmcEff = nullptr;
        if (m_FillHistos) {
            unsigned int nBins = 100000;
            Histo.SF = new TH1D(("SF_" + m_Tool.name() + Set.name()).c_str(), "", nBins, 0., 2.1);
            Histo.SF->GetXaxis()->SetTitle("SF value");
            Histo.eff = new TH1D(("DataEff_" + m_Tool.name() + Set.name()).c_str(), "", nBins, 0., 1.1);
            Histo.eff->GetXaxis()->SetTitle("Data efficiency value");
            Histo.mceff = new TH1D(("MCEff_" + m_Tool.name() + Set.name()).c_str(), "", nBins, 0., 1.1);
            Histo.mceff->GetXaxis()->SetTitle("MC efficiency value");

            if (!Set.name().empty()) {
                Histo.relSystSF = new TH1D(("relSF_" + m_Tool.name() + Set.name()).c_str(), "", nBins, 0., 1.1);
                Histo.relSystEff = new TH1D(("relDataEff_" + m_Tool.name() + Set.name()).c_str(), "", nBins, 0., 1.1);
                Histo.relSystmcEff = new TH1D(("relMCEff_" + m_Tool.name() + Set.name()).c_str(), "", nBins, 0., 1.1);
                std::string xTitle = Set.name().find("STAT") != std::string::npos ? "Statistical" : "Systematic";
                Histo.relSystSF->GetXaxis()->SetTitle(Form("%s error of the SF", xTitle.c_str()));
                Histo.relSystEff->GetXaxis()->SetTitle(Form("%s error of the data efficiency", xTitle.c_str()));
                Histo.relSystmcEff->GetXaxis()->SetTitle(Form("%s error of the MC efficiency", xTitle.c_str()));
            }
        }
        m_Histos.insert(std::pair<CP::SystematicSet*, TestScaleFactors>(&Set, Histo));

    }
    CP::SystematicCode MuonSFTestHelper::GetMCEfficiency(float &eff, const CP::SystematicSet &Set) const {
        for (const auto& Tester : m_Histos) {
            if (Tester.first->name() == Set.name()) {
                eff = Tester.second.mcefficiency_Value;
                return CP::SystematicCode::Ok;
            }
        }
        return CP::SystematicCode::Unsupported;
    }
    CP::SystematicCode MuonSFTestHelper::GetEfficiency(float &eff, const CP::SystematicSet &Set) const {
        for (const auto& Tester : m_Histos) {
            if (Tester.first->name() == Set.name()) {
                eff = Tester.second.efficiency_Value;
                return CP::SystematicCode::Ok;
            }
        }
        return CP::SystematicCode::Unsupported;

    }
    CP::SystematicCode MuonSFTestHelper::GetScaleFactor(float &eff, const CP::SystematicSet &Set) const {
        for (const auto& Tester : m_Histos) {
            if (Tester.first->name() == Set.name()) {
                eff = Tester.second.sf_Value;
                return CP::SystematicCode::Ok;
            }
        }
        return CP::SystematicCode::Unsupported;

    }

    CP::CorrectionCode MuonSFTestHelper::TestSF(const xAOD::Muon& mu) {
        for (auto& Sys : m_Syst) {
            if (!m_Tool.applySystematicVariation(*Sys)) {
                return CP::CorrectionCode::Error;
            }
            float sf(0), eff(0.), mcEff(0.);
            CHECK_CPCorr(m_Tool.getEfficiencyScaleFactor(mu, sf));
            // Do not try to retrieve reconstruction efficiencies for high-eta muons
            if ((m_Tool.name().find("Reco") == std::string::npos) || fabs(mu.eta()) <= 2.5) {
                CHECK_CPCorr(m_Tool.getDataEfficiency(mu, eff));
                CHECK_CPCorr(m_Tool.getMCEfficiency(mu, mcEff));
            }
            std::string SystName = Sys->name().empty() ? "Nominal" : Sys->name();
            // Info(m_Tool.name().c_str(), " Calculated SF: %f, mcEff: %f and eff: %f for systematic %s using muon with pt: %.3f, eta: %.2f, phi : %.3f", sf, mcEff, eff, SystName.c_str(), mu.pt() / 1.e3, mu.eta(), mu.phi());
            TestScaleFactors& Histos = m_Histos.at(Sys);
            Histos.sf_Value = sf;
            Histos.efficiency_Value = eff;
            Histos.mcefficiency_Value = mcEff;

            if (m_FillHistos) {
                Histos.SF->Fill(sf);
                Histos.eff->Fill(eff);
                Histos.mceff->Fill(mcEff);
                if (!Sys->name().empty()) {
                    float CentralSF(0.), CentralEff(0.), CentralmcEff(0.);
                    if (!m_Tool.applySystematicVariation(m_Nominal)) {
                        return CP::CorrectionCode::Error;
                    }
                    CHECK_CPCorr(m_Tool.getEfficiencyScaleFactor(mu, CentralSF));
                    // Do not try to retrieve reconstruction efficiencies for high-eta muons
                    if ((m_Tool.name().find("Reco") == std::string::npos) || fabs(mu.eta()) <= 2.5) {
                        CHECK_CPCorr(m_Tool.getDataEfficiency(mu, CentralEff));
                        CHECK_CPCorr(m_Tool.getMCEfficiency(mu, CentralmcEff));
                    }
                    float relSystSF = fabs(CentralSF - sf);
                    float relSystEff = fabs(CentralEff - eff);
                    float relSystmcEff = fabs(CentralmcEff - eff);
                    if (relSystSF > 1.) Warning(m_Tool.name().c_str(), "SF %f has for systematic %s a relative uncertainty of %.2f", CentralSF, Sys->name().c_str(), relSystSF / CentralSF);
                    if (relSystEff > 1.) Warning(m_Tool.name().c_str(), "Data Efficiency %f has for systematic %s a relative uncertainty of %.2f", CentralEff, Sys->name().c_str(), relSystEff / CentralEff);
                    if (relSystmcEff > 1.) Warning(m_Tool.name().c_str(), "MC Efficiency %f has for systematic %s a relative uncertainty of %.2f", CentralmcEff, Sys->name().c_str(), relSystmcEff / CentralmcEff);
                    Histos.relSystSF->Fill(relSystSF);
                    Histos.relSystEff->Fill(relSystEff);
                    Histos.relSystmcEff->Fill(relSystmcEff);
                }
            }
        }
        return CP::CorrectionCode::Ok;
    }
    MuonSFTestHelper::~MuonSFTestHelper() {
        for (auto& Histo : m_Histos) {
            if (Histo.second.SF) delete Histo.second.SF;
            if (Histo.second.eff) delete Histo.second.eff;
            if (Histo.second.mceff) delete Histo.second.mceff;
            if (Histo.second.relSystSF) delete Histo.second.relSystSF;
            if (Histo.second.relSystEff) delete Histo.second.relSystEff;
            if (Histo.second.relSystmcEff) delete Histo.second.relSystmcEff;
        }
        for (auto& Sys : m_Syst) {
            delete Sys;
        }
    }
    bool MuonSFTestHelper::WriteHistosToFile(TFile* file) {
        if (!file) {
            return false;
        }
        for (auto& Histo : m_Histos) {
            WriteHistogram(file, Histo.second.SF);
            WriteHistogram(file, Histo.second.eff);
            WriteHistogram(file, Histo.second.mceff);
            WriteHistogram(file, Histo.second.relSystSF);
            WriteHistogram(file, Histo.second.relSystEff);
            WriteHistogram(file, Histo.second.relSystmcEff);
        }
        return true;
    }

    const CP::MuonEfficiencyScaleFactors& MuonSFTestHelper::GetTool() const {
        return m_Tool;
    }

    MuonSFReleaseComparer::MuonSFReleaseComparer(MuonSFTestHelper& Rel1, MuonSFTestHelper& Rel2) :
                    m_Rel1(Rel1),
                    m_Rel2(Rel2),
                    m_Histos() {

    }
    void MuonSFReleaseComparer::CreateHistogram(const CP::SystematicSet &Set) {
        unsigned int nBins = 100000;
        ComparingHistos H;
        H.SF = new TH1D(Form("CompSF_%s%s", m_Rel1.GetTool().name().c_str(), Set.name().c_str()), "", nBins, 0, 2.1);
        H.eff = new TH1D(Form("CompEff_%s%s", m_Rel1.GetTool().name().c_str(), Set.name().c_str()), "", nBins, 0, 2.1);
        m_Histos.insert(std::pair<CP::SystematicSet, ComparingHistos>(Set, H));
    }

    StatusCode MuonSFReleaseComparer::initialize() {
        if (m_Rel1.initialize().isSuccess() || m_Rel2.initialize().isSuccess()) {
            return StatusCode::FAILURE;
        }
        for (const auto& set : CP::make_systematics_vector(m_Rel1.GetTool().recommendedSystematics())) {
            if (set.name().empty()) continue;
            for (const auto& sys : set) {
                if (!m_Rel2.GetTool().isAffectedBySystematic(sys)) {
                    Error("MuonSFReleaseComprarer::initialize()", "Tool %s does not share the systematic %s provided by %s", m_Rel2.GetTool().name().c_str(), set.name().c_str(), m_Rel1.GetTool().name().c_str());
                    return StatusCode::FAILURE;
                }
            }
            CreateHistogram(set);
        }
        CreateHistogram(CP::SystematicSet());
        return StatusCode::SUCCESS;
    }
    CP::CorrectionCode MuonSFReleaseComparer::TestSF(const xAOD::Muon& mu) {
        CHECK_CPCorr(m_Rel1.TestSF(mu));
        CHECK_CPCorr(m_Rel2.TestSF(mu));
        for (auto& SystHistos : m_Histos) {
            float sfRel1(0.), sfRel2(0.), effRel1(0.), effRel2(0.);
            m_Rel1.GetScaleFactor(sfRel1, SystHistos.first);
            m_Rel2.GetScaleFactor(sfRel2, SystHistos.first);
            m_Rel1.GetEfficiency(effRel1, SystHistos.first);
            m_Rel2.GetEfficiency(effRel2, SystHistos.first);
            SystHistos.second.SF->Fill(fabs(sfRel1 - sfRel2));
            SystHistos.second.eff->Fill(fabs(effRel1 - effRel2));
        }
        return CP::CorrectionCode::Ok;
    }
    bool MuonSFReleaseComparer::WriteHistosToFile(TFile* file) {
        if (!m_Rel1.WriteHistosToFile(file) || !m_Rel2.WriteHistosToFile(file)) {
            return false;
        }
        for (auto& Histo : m_Histos) {
            WriteHistogram(file, Histo.second.SF);
            WriteHistogram(file, Histo.second.eff);
        }
        return true;
    }
    MuonSFReleaseComparer::~MuonSFReleaseComparer() {
        for (auto& Histo : m_Histos) {
            if (Histo.second.SF) delete Histo.second.SF;
            if (Histo.second.eff) delete Histo.second.eff;
        }
    }

}

