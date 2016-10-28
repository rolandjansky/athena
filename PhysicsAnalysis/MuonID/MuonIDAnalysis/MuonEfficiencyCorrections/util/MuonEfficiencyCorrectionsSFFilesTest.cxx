/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <memory>
#include <cstdlib>
#include <string>
#include <map>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TStopwatch.h>
#include <TString.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#   include "xAODRootAccess/tools/ReturnCheck.h"
#   include "AsgTools/Check.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"

// Local include(s):
#include "MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h"
#include "PATInterfaces/ISystematicsTool.h"

#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

#include "AsgTools/ToolHandle.h"
#include "AsgTools/AnaToolHandle.h"
#include "PileupReweighting/IPileupReweightingTool.h"

#define CHECK_CPCorr(Arg) \
    if (Arg.code() == CP::CorrectionCode::Error){    \
        Error(#Arg,"Correction Code 'Error' (returned in line %i) ",__LINE__); \
        return 1;   \
    }
#define CHECK_CPSys(Arg) \
    if (Arg.code() == CP::SystematicCode::Unsupported){    \
        Warning(#Arg,"Unsupported systematic (in line %i) ",__LINE__); \
    }      

enum HistoType {
    SF = 0, Sys, Stat, Eff, MCEff
};

std::vector<TH1D*> m_histos;
std::vector<TH1D*> m_Comphistos;

TH1D* CreateHistogram(std::string name, unsigned int type, std::string bonusname = "") {
    std::string histoname = name;
    if (type == SF) histoname += "_SF";
    else if (type == Sys) histoname += "_Sys";
    else if (type == Stat) histoname += "_Stat";
    histoname += bonusname;
    float lowestbin(0.), highestbin(1.);
    if (type == SF) {
        highestbin = 2.1;
    }
    TH1D *histo = new TH1D(histoname.c_str(), histoname.c_str(), 100000, lowestbin, highestbin);
    if (type == SF) histo->GetXaxis()->SetTitle("SF value");
    else if (type == Sys) histo->GetXaxis()->SetTitle("Relative systematic SF uncertainty");
    else if (type == Stat) histo->GetXaxis()->SetTitle("Relative statistical SF uncertainty");
    histo->GetYaxis()->SetTitle("Fraction of muons");
    m_histos.push_back(histo);
    return histo;
}

TH1D* CreateComparisonHistogram(std::string name, unsigned int type, std::string bonusname = "") {
    std::string histoname = "Comparison_" + name;
    if (type == SF) histoname += "_SF";
    else if (type == Eff) histoname += "_Eff";
    else if (type == MCEff) histoname += "_MCEff";
    else if (type == Sys) histoname += "_Sys";
    else if (type == Stat) histoname += "_Stat";
    histoname += bonusname;
    float lowestbin(0.), highestbin(1.);
    TH1D *histo = new TH1D(histoname.c_str(), histoname.c_str(), 100000, lowestbin, highestbin);
    if (type == SF) histo->GetXaxis()->SetTitle("|SF_{new}-SF_{old}|");
    else if (type == Eff) histo->GetXaxis()->SetTitle("|Eff_{new}-Eff_{old}|");
    else if (type == MCEff) histo->GetXaxis()->SetTitle("|MC-Eff_{new}-MC-Eff_{old}|");
    else if (type == Sys) histo->GetXaxis()->SetTitle("|Sys_{new}-Sys_{old}|");
    else if (type == Stat) histo->GetXaxis()->SetTitle("|Stat-Eff_{new}-Stat_{old}|");
    histo->GetYaxis()->SetTitle("Fraction of muons");
    m_Comphistos.push_back(histo);
    return histo;
}

int main(int argc, char* argv[]) {

    // force strict checking of return codes
    CP::SystematicCode::enableFailure();
    xAOD::TReturnCode::enableFailure();
    StatusCode::enableFailure();

    // The application's name:
    const char* APP_NAME = argv[0];

    std::string CustomInputFolder = "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/MuonEfficiencyCorrections/160928_PostICHEP/";

    std::string OutputFilename = "Applied_SFs.root";
    std::string InDir = "";
    std::string SFComparisonFolder = "";
    long long int nmax = -1;
    // read the config provided by the user
    for (int k = 1; k < argc - 1; ++k) {
        if (std::string(argv[k]).find("-i") == 0) {
            InDir = argv[k + 1];
        }
        if (std::string(argv[k]).find("-c") == 0) {
            SFComparisonFolder = argv[k + 1];
        }
        if (std::string(argv[k]).find("-n") == 0) {
            nmax = atoi(argv[k + 1]);
        }
        if (std::string(argv[k]).find("-o") == 0) {
            OutputFilename = argv[k + 1];
        }
    }
    bool doComparison = (SFComparisonFolder != "");

    // check( if we received a file name:
    if (argc < 3) {
        Error(APP_NAME, "No file name received!");
        Error(APP_NAME, "  Usage: %s -i [xAOD file name]", APP_NAME);
        return 1;
    }

    RETURN_CHECK(APP_NAME, xAOD::Init(APP_NAME));

    // Open the input file:
    const TString fileName = InDir;
    Info(APP_NAME, "Opening file: %s", fileName.Data());
    std::auto_ptr<TFile> ifile(TFile::Open(fileName, "READ"));
    if (!ifile.get()) {
        Error(APP_NAME, " Unable to load xAOD input file");
    }

    // Create a TEvent object:
    xAOD::TEvent event;
    RETURN_CHECK(APP_NAME, event.readFrom(ifile.get(), xAOD::TEvent::kClassAccess));
    Info(APP_NAME, "Number of events in the file: %i", static_cast<int>(event.getEntries()));

    // Decide how many events to run over:
    Long64_t entries = event.getEntries();
    if ((nmax != -1) && (nmax < entries)) entries = nmax;

    TStopwatch tsw;
    tsw.Start();
    double t_init = tsw.CpuTime();
    tsw.Reset();
    tsw.Start();
    
    unsigned int nMuons = 0;

    asg::AnaToolHandle<CP::IPileupReweightingTool> m_prw_tool("CP::PileupReweightingTool/myTool");
    // This is just a placeholder configuration for testing. Do not use these config files for your analysis!
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("DefaultChannel", 410000));
    std::vector<std::string> m_ConfigFiles { "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root" };
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("ConfigFiles", m_ConfigFiles));
    std::vector<std::string> m_LumiCalcFiles { "/afs/cern.ch/atlas/project/muon/mcp/PRWFiles/ilumicalc_histograms_data15_13TeV.periodAllYear_DetStatus-v79-repro20-02_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.root", "/afs/cern.ch/atlas/project/muon/mcp/PRWFiles/ilumicalc_histograms_data16_13TeV.periodAllYear_DetStatus-v82-pro20-13_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.root" };
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("LumiCalcFiles", m_LumiCalcFiles));
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("DataScaleFactor", 1. / 1.16));
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("DataScaleFactorUP", 1.));
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("DataScaleFactorDOWN", 1. / 1.23));
    ASG_CHECK_SA(APP_NAME, m_prw_tool.initialize());

    CP::MuonEfficiencyScaleFactors m_effi_corr("Medium");
    ASG_CHECK_SA(APP_NAME, asg::setProperty(m_effi_corr, "WorkingPoint", "Medium"));
    ASG_CHECK_SA(APP_NAME, asg::setProperty(m_effi_corr, "CustomInputFolder", CustomInputFolder));
    ASG_CHECK_SA(APP_NAME, asg::setProperty(m_effi_corr, "LowPtThreshold", -1.));
    ASG_CHECK_SA(APP_NAME, m_effi_corr.initialize());
    CP::SystematicSet statup;
    statup.insert(CP::SystematicVariation("MUON_EFF_STAT", 1));
    CP::SystematicSet statdown;
    statdown.insert(CP::SystematicVariation("MUON_EFF_STAT", -1));
    CP::SystematicSet sysup;
    sysup.insert(CP::SystematicVariation("MUON_EFF_SYS", 1));
    CP::SystematicSet sysdown;
    sysdown.insert(CP::SystematicVariation("MUON_EFF_SYS", -1));

    
    CP::MuonEfficiencyScaleFactors m_effi_corr_loose("TestLooseSFs");
    ASG_CHECK_SA(APP_NAME, asg::setProperty(m_effi_corr_loose, "WorkingPoint", "Loose"));
    ASG_CHECK_SA(APP_NAME, asg::setProperty(m_effi_corr_loose, "CustomInputFolder", CustomInputFolder));
    ASG_CHECK_SA(APP_NAME, m_effi_corr_loose.initialize());

    CP::MuonEfficiencyScaleFactors m_effi_corr_HighPt("TestHighPtSFs");
    ASG_CHECK_SA(APP_NAME, asg::setProperty(m_effi_corr_HighPt, "WorkingPoint", "HighPt"));
    ASG_CHECK_SA(APP_NAME, asg::setProperty(m_effi_corr_HighPt, "CustomInputFolder", CustomInputFolder));
    ASG_CHECK_SA(APP_NAME, m_effi_corr_HighPt.initialize());

    CP::MuonEfficiencyScaleFactors m_effi_corr_Tight("TestTightSFs");
    ASG_CHECK_SA(APP_NAME, asg::setProperty(m_effi_corr_Tight, "WorkingPoint", "Tight"));
    ASG_CHECK_SA(APP_NAME, asg::setProperty(m_effi_corr_Tight, "CustomInputFolder", CustomInputFolder));
    ASG_CHECK_SA(APP_NAME, m_effi_corr_Tight.initialize());

    CP::MuonEfficiencyScaleFactors m_ttva_corr("TestTTVASFs");
    ASG_CHECK_SA(APP_NAME, asg::setProperty(m_ttva_corr, "WorkingPoint", "TTVA"));
    ASG_CHECK_SA(APP_NAME, asg::setProperty(m_ttva_corr, "CustomInputFolder", CustomInputFolder));
    ASG_CHECK_SA(APP_NAME, m_ttva_corr.initialize());

    CP::SystematicSet statupTTVA;
    statupTTVA.insert(CP::SystematicVariation("MUON_TTVA_STAT", 1));
    CP::SystematicSet sysupTTVA;
    sysupTTVA.insert(CP::SystematicVariation("MUON_TTVA_SYS", 1));

    CP::MuonEfficiencyScaleFactors m_effi_corr_medium_comp("CompMediumSFs");
    CP::MuonEfficiencyScaleFactors m_effi_corr_loose_comp("CompLooseSFs");
    CP::MuonEfficiencyScaleFactors m_effi_corr_HighPt_comp("CompHighPtSFs");
    CP::MuonEfficiencyScaleFactors m_effi_corr_Tight_comp("CompTightSFs");
    CP::MuonEfficiencyScaleFactors m_ttva_corr_comp("CompTTVASFs");
    if (doComparison) {
        ASG_CHECK_SA(APP_NAME, asg::setProperty(m_effi_corr_medium_comp, "WorkingPoint", "Loose"));
        ASG_CHECK_SA(APP_NAME, asg::setProperty(m_effi_corr_medium_comp, "CustomInputFolder", SFComparisonFolder));
        ASG_CHECK_SA(APP_NAME, m_effi_corr_medium_comp.initialize());

        ASG_CHECK_SA(APP_NAME, asg::setProperty(m_effi_corr_loose_comp, "WorkingPoint", "Loose"));
        ASG_CHECK_SA(APP_NAME, asg::setProperty(m_effi_corr_loose_comp, "CustomInputFolder", SFComparisonFolder));
        ASG_CHECK_SA(APP_NAME, m_effi_corr_loose_comp.initialize());

        ASG_CHECK_SA(APP_NAME, asg::setProperty(m_effi_corr_HighPt_comp, "WorkingPoint", "HighPt"));
        ASG_CHECK_SA(APP_NAME, asg::setProperty(m_effi_corr_HighPt_comp, "CustomInputFolder", SFComparisonFolder));
        ASG_CHECK_SA(APP_NAME, m_effi_corr_HighPt_comp.initialize());

        ASG_CHECK_SA(APP_NAME, asg::setProperty(m_effi_corr_Tight_comp, "WorkingPoint", "Tight"));
        ASG_CHECK_SA(APP_NAME, asg::setProperty(m_effi_corr_Tight_comp, "CustomInputFolder", SFComparisonFolder));
        ASG_CHECK_SA(APP_NAME, m_effi_corr_Tight_comp.initialize());

        ASG_CHECK_SA(APP_NAME, asg::setProperty(m_ttva_corr_comp, "WorkingPoint", "TTVA"));
        ASG_CHECK_SA(APP_NAME, asg::setProperty(m_ttva_corr_comp, "CustomInputFolder", SFComparisonFolder));
        ASG_CHECK_SA(APP_NAME, m_ttva_corr_comp.initialize());
    }



    CP::MuonEfficiencyScaleFactors m_iso_effi_corr("TestIsolationSF");
    ASG_CHECK_SA(APP_NAME, m_iso_effi_corr.setProperty("WorkingPoint", "GradientIso"));
    ASG_CHECK_SA(APP_NAME, asg::setProperty(m_iso_effi_corr, "CustomInputFolder", CustomInputFolder));
    ASG_CHECK_SA(APP_NAME, m_iso_effi_corr.initialize());

    CP::SystematicSet isosysup;
    isosysup.insert(CP::SystematicVariation("MUON_ISO_SYS", 1));
    CP::SystematicSet isostatup;
    isostatup.insert(CP::SystematicVariation("MUON_ISO_STAT", 1));

    // write a root file containing the SFs, stat and sys errors
    TFile *f = new TFile(OutputFilename.c_str(), "RECREATE");
    TH1D *histSF_Medium = CreateHistogram("Medium", HistoType::SF);
    TH1D *histSF_Medium_sys = CreateHistogram("Medium", HistoType::Sys);
    TH1D *histSF_Medium_stat = CreateHistogram("Medium", HistoType::Stat);
    TH1D *histSF_Loose = CreateHistogram("Loose", HistoType::SF);
    TH1D *histSF_Loose_sys = CreateHistogram("Loose", HistoType::Sys);
    TH1D *histSF_Loose_stat = CreateHistogram("Loose", HistoType::Stat);
    TH1D *histSF_HighPt = CreateHistogram("HighPt", HistoType::SF);
    TH1D *histSF_HighPt_sys = CreateHistogram("HighPt", HistoType::Sys);
    TH1D *histSF_HighPt_stat = CreateHistogram("HighPt", HistoType::Stat);
    TH1D *histSF_Tight = CreateHistogram("Tight", HistoType::SF);
    TH1D *histSF_Tight_sys = CreateHistogram("Tight", HistoType::Sys);
    TH1D *histSF_Tight_stat = CreateHistogram("Tight", HistoType::Stat);
    TH1D *histSF_TTVA = CreateHistogram("TTVA", HistoType::SF);
    TH1D *histSF_TTVA_sys = CreateHistogram("TTVA", HistoType::Sys);
    TH1D *histSF_TTVA_stat = CreateHistogram("TTVA", HistoType::Stat);
    TH1D *histSF_Iso = CreateHistogram("Iso", HistoType::SF);
    TH1D *histSF_Iso_sys = CreateHistogram("Iso", HistoType::Sys);
    TH1D *histSF_Iso_stat = CreateHistogram("Iso", HistoType::Stat);

    // also check 0<|eta|<2.5 and 2.5<|eta|<2.7 separately
    TH1D *histSF_Medium_NoHighEta = CreateHistogram("Medium", HistoType::SF, "_NoHighEta");
    TH1D *histSF_Medium_sys_NoHighEta = CreateHistogram("Medium", HistoType::Sys, "_NoHighEta");
    TH1D *histSF_Medium_stat_NoHighEta = CreateHistogram("Medium", HistoType::Stat, "_NoHighEta");
    TH1D *histSF_Loose_NoHighEta = CreateHistogram("Loose", HistoType::SF, "_NoHighEta");
    TH1D *histSF_Loose_sys_NoHighEta = CreateHistogram("Loose", HistoType::Sys, "_NoHighEta");
    TH1D *histSF_Loose_stat_NoHighEta = CreateHistogram("Loose", HistoType::Stat, "_NoHighEta");
    TH1D *histSF_HighPt_NoHighEta = CreateHistogram("HighPt", HistoType::SF, "_NoHighEta");
    TH1D *histSF_HighPt_sys_NoHighEta = CreateHistogram("HighPt", HistoType::Sys, "_NoHighEta");
    TH1D *histSF_HighPt_stat_NoHighEta = CreateHistogram("HighPt", HistoType::Stat, "_NoHighEta");
    TH1D *histSF_Tight_NoHighEta = CreateHistogram("Tight", HistoType::SF, "_NoHighEta");
    TH1D *histSF_Tight_sys_NoHighEta = CreateHistogram("Tight", HistoType::Sys, "_NoHighEta");
    TH1D *histSF_Tight_stat_NoHighEta = CreateHistogram("Tight", HistoType::Stat, "_NoHighEta");
    TH1D *histSF_Medium_HighEta = CreateHistogram("Medium", HistoType::SF, "_HighEta");
    TH1D *histSF_Medium_sys_HighEta = CreateHistogram("Medium", HistoType::Sys, "_HighEta");
    TH1D *histSF_Medium_stat_HighEta = CreateHistogram("Medium", HistoType::Stat, "_HighEta");
    TH1D *histSF_Loose_HighEta = CreateHistogram("Loose", HistoType::SF, "_HighEta");
    TH1D *histSF_Loose_sys_HighEta = CreateHistogram("Loose", HistoType::Sys, "_HighEta");
    TH1D *histSF_Loose_stat_HighEta = CreateHistogram("Loose", HistoType::Stat, "_HighEta");
    TH1D *histSF_HighPt_HighEta = CreateHistogram("HighPt", HistoType::SF, "_HighEta");
    TH1D *histSF_HighPt_sys_HighEta = CreateHistogram("HighPt", HistoType::Sys, "_HighEta");
    TH1D *histSF_HighPt_stat_HighEta = CreateHistogram("HighPt", HistoType::Stat, "_HighEta");
    TH1D *histSF_Tight_HighEta = CreateHistogram("Tight", HistoType::SF, "_HighEta");
    TH1D *histSF_Tight_sys_HighEta = CreateHistogram("Tight", HistoType::Sys, "_HighEta");
    TH1D *histSF_Tight_stat_HighEta = CreateHistogram("Tight", HistoType::Stat, "_HighEta");

    // also for comparisons, check 0<|eta|<2.5 and 2.5<|eta|<2.7 separately
    TH1D *histSFComp_Medium_NoHighEta = CreateComparisonHistogram("Medium", HistoType::SF, "_NoHighEta");
    TH1D *histSFComp_Medium_Eff_NoHighEta = CreateComparisonHistogram("Medium", HistoType::Eff, "_NoHighEta");
    TH1D *histSFComp_Medium_sys_NoHighEta = CreateComparisonHistogram("Medium", HistoType::Sys, "_NoHighEta");
    TH1D *histSFComp_Medium_stat_NoHighEta = CreateComparisonHistogram("Medium", HistoType::Stat, "_NoHighEta");
    TH1D *histSFComp_Loose_NoHighEta = CreateComparisonHistogram("Loose", HistoType::SF, "_NoHighEta");
    TH1D *histSFComp_Loose_Eff_NoHighEta = CreateComparisonHistogram("Loose", HistoType::Eff, "_NoHighEta");
    TH1D *histSFComp_Loose_sys_NoHighEta = CreateComparisonHistogram("Loose", HistoType::Sys, "_NoHighEta");
    TH1D *histSFComp_Loose_stat_NoHighEta = CreateComparisonHistogram("Loose", HistoType::Stat, "_NoHighEta");
    TH1D *histSFComp_HighPt_NoHighEta = CreateComparisonHistogram("HighPt", HistoType::SF, "_NoHighEta");
    TH1D *histSFComp_HighPt_Eff_NoHighEta = CreateComparisonHistogram("HighPt", HistoType::Eff, "_NoHighEta");
    TH1D *histSFComp_HighPt_sys_NoHighEta = CreateComparisonHistogram("HighPt", HistoType::Sys, "_NoHighEta");
    TH1D *histSFComp_HighPt_stat_NoHighEta = CreateComparisonHistogram("HighPt", HistoType::Stat, "_NoHighEta");
    TH1D *histSFComp_Tight_NoHighEta = CreateComparisonHistogram("Tight", HistoType::SF, "_NoHighEta");
    TH1D *histSFComp_Tight_Eff_NoHighEta = CreateComparisonHistogram("Tight", HistoType::Eff, "_NoHighEta");
    TH1D *histSFComp_Tight_sys_NoHighEta = CreateComparisonHistogram("Tight", HistoType::Sys, "_NoHighEta");
    TH1D *histSFComp_Tight_stat_NoHighEta = CreateComparisonHistogram("Tight", HistoType::Stat, "_NoHighEta");
    TH1D *histSFComp_Medium_HighEta = CreateComparisonHistogram("Medium", HistoType::SF, "_HighEta");
    TH1D *histSFComp_Medium_sys_HighEta = CreateComparisonHistogram("Medium", HistoType::Sys, "_HighEta");
    TH1D *histSFComp_Medium_stat_HighEta = CreateComparisonHistogram("Medium", HistoType::Stat, "_HighEta");
    TH1D *histSFComp_Loose_HighEta = CreateComparisonHistogram("Loose", HistoType::SF, "_HighEta");
    TH1D *histSFComp_Loose_sys_HighEta = CreateComparisonHistogram("Loose", HistoType::Sys, "_HighEta");
    TH1D *histSFComp_Loose_stat_HighEta = CreateComparisonHistogram("Loose", HistoType::Stat, "_HighEta");
    TH1D *histSFComp_HighPt_HighEta = CreateComparisonHistogram("HighPt", HistoType::SF, "_HighEta");
    TH1D *histSFComp_HighPt_sys_HighEta = CreateComparisonHistogram("HighPt", HistoType::Sys, "_HighEta");
    TH1D *histSFComp_HighPt_stat_HighEta = CreateComparisonHistogram("HighPt", HistoType::Stat, "_HighEta");
    TH1D *histSFComp_Tight_HighEta = CreateComparisonHistogram("Tight", HistoType::SF, "_HighEta");
    TH1D *histSFComp_Tight_sys_HighEta = CreateComparisonHistogram("Tight", HistoType::Sys, "_HighEta");
    TH1D *histSFComp_Tight_stat_HighEta = CreateComparisonHistogram("Tight", HistoType::Stat, "_HighEta");
    TH1D *histSFComp_TTVA = CreateComparisonHistogram("TTVA", HistoType::SF);
    TH1D *histSFComp_TTVA_Eff = CreateComparisonHistogram("TTVA", HistoType::Eff);
    TH1D *histSFComp_TTVA_sys = CreateComparisonHistogram("TTVA", HistoType::Sys);
    TH1D *histSFComp_TTVA_stat = CreateComparisonHistogram("TTVA", HistoType::Stat);

    // check pt > 15GeV only
    TH1D *histSF_Medium_sys_pt15 = CreateHistogram("Medium", HistoType::Sys, "_pt15");
    TH1D *histSF_Loose_sys_pt15 = CreateHistogram("Loose", HistoType::Sys, "_pt15");
    
    if (doComparison) for (auto hist : m_Comphistos) m_histos.push_back(hist);
    
    for (Long64_t entry = 0; entry < entries; ++entry) {

        // Tell the object which entry to look at:
        event.getEntry(entry);

        // Print some event information for fun:
        const xAOD::EventInfo* ei = 0;
        RETURN_CHECK(APP_NAME, event.retrieve(ei, "EventInfo"));
        RETURN_CHECK(APP_NAME, m_prw_tool->apply(*ei));
        if (entry % 5000 == 0) Info(APP_NAME, "===>>>  start processing event #%i, run #%i (%i events processed so far)  <<<===", static_cast<int>(ei->eventNumber()), static_cast<int>(ei->runNumber()), static_cast<int>(entry));

        // Get the Muons from the event:
        const xAOD::MuonContainer* muons = 0;
        RETURN_CHECK(APP_NAME, event.retrieve(muons, "Muons"));

        // Print their properties, using the tools:
        xAOD::MuonContainer::const_iterator mu_itr = muons->begin();
        xAOD::MuonContainer::const_iterator mu_end = muons->end();

        for (; mu_itr != mu_end; ++mu_itr) {

            if (fabs((*mu_itr)->eta()) > 2.7) continue;
            if (fabs((*mu_itr)->pt()) < 5000.) continue;
            
            //ignore calo-tag muons:
            if ((*mu_itr)->author() == xAOD::Muon::CaloTag) continue;
            
            ++nMuons;

            // Use the "simple interface" of the tool(s):
            float eff = 0.0, sf = 0.0;

            CHECK_CPSys(m_effi_corr.applySystematicVariation(CP::SystematicSet()));
            CHECK_CPCorr(m_effi_corr.getEfficiencyScaleFactor(**mu_itr, sf));
            float centralSF = sf;
            CHECK_CPSys(m_effi_corr.applySystematicVariation(statup));
            CHECK_CPCorr(m_effi_corr.getEfficiencyScaleFactor(**mu_itr, sf));
            float statupSF = sf;
            CHECK_CPSys(m_effi_corr.applySystematicVariation(sysup));
            CHECK_CPCorr(m_effi_corr.getEfficiencyScaleFactor(**mu_itr, sf));
            float upSF = sf;
            CHECK_CPSys(m_effi_corr.applySystematicVariation(CP::SystematicSet()));

            histSF_Medium->Fill(fabs(centralSF));
            histSF_Medium_stat->Fill(fabs(statupSF - centralSF));
            if (fabs(statupSF - centralSF) > 1) std::cout << "WARNING: MediumWP: Statistical error greater than 100% for muon with " << Form("pt=%.1f, eta=%.1f, phi=%.1f, author=%i", (*mu_itr)->pt(), (*mu_itr)->eta(), (*mu_itr)->phi(), (*mu_itr)->author()) << std::endl;
            histSF_Medium_sys->Fill(fabs(upSF - centralSF));
            if (fabs(upSF - centralSF) > 1) std::cout << "WARNING: MediumWP: Systematic error greater than 100% for muon with " << Form("pt=%.1f, eta=%.1f, phi=%.1f, author=%i", (*mu_itr)->pt(), (*mu_itr)->eta(), (*mu_itr)->phi(), (*mu_itr)->author()) << std::endl;
            if ((*mu_itr)->pt() > 15000.) histSF_Medium_sys_pt15->Fill(fabs(upSF - centralSF));

            if (fabs((*mu_itr)->eta()) < 2.5) {
                histSF_Medium_NoHighEta->Fill(fabs(centralSF));
                histSF_Medium_stat_NoHighEta->Fill(fabs(statupSF - centralSF));
                histSF_Medium_sys_NoHighEta->Fill(fabs(upSF - centralSF));
            }
            if (fabs((*mu_itr)->eta()) > 2.5) {
                histSF_Medium_HighEta->Fill(fabs(centralSF));
                histSF_Medium_stat_HighEta->Fill(fabs(statupSF - centralSF));
                histSF_Medium_sys_HighEta->Fill(fabs(upSF - centralSF));
            }
                
                
            if (doComparison) {
                CHECK_CPSys(m_effi_corr_medium_comp.applySystematicVariation(CP::SystematicSet()));
                CHECK_CPCorr(m_effi_corr_medium_comp.getEfficiencyScaleFactor(**mu_itr, sf));
                float centralSFMediumComp = sf;
                CHECK_CPSys(m_effi_corr_medium_comp.applySystematicVariation(statup));
                CHECK_CPCorr(m_effi_corr_medium_comp.getEfficiencyScaleFactor(**mu_itr, sf));
                float statupSFMediumComp = sf;
                CHECK_CPSys(m_effi_corr_medium_comp.applySystematicVariation(sysup));
                CHECK_CPCorr(m_effi_corr_medium_comp.getEfficiencyScaleFactor(**mu_itr, sf));
                float upSFMediumComp = sf;
                CHECK_CPSys(m_effi_corr_medium_comp.applySystematicVariation(CP::SystematicSet()));

                if (fabs((*mu_itr)->eta()) < 2.5) {
                    float EffMediumComp = 1.;
                    CHECK_CPCorr(m_effi_corr_medium_comp.getDataEfficiency(**mu_itr, EffMediumComp));
                    CHECK_CPCorr(m_effi_corr.getDataEfficiency(**mu_itr, eff));
                    histSFComp_Medium_NoHighEta->Fill(fabs(centralSF - centralSFMediumComp));
                    histSFComp_Medium_Eff_NoHighEta->Fill(fabs(eff - EffMediumComp));
                    histSFComp_Medium_stat_NoHighEta->Fill(fabs(statupSF - statupSFMediumComp));
                    histSFComp_Medium_sys_NoHighEta->Fill(fabs(upSF-upSFMediumComp));
                } else if (fabs((*mu_itr)->eta()) > 2.5) {
                    histSFComp_Medium_HighEta->Fill(fabs(centralSF - centralSFMediumComp));
                    histSFComp_Medium_stat_HighEta->Fill(fabs(statupSF - statupSFMediumComp));
                    histSFComp_Medium_sys_HighEta->Fill(fabs(upSF-upSFMediumComp));
                }
            }


            // fill the Loose histos
            CHECK_CPSys(m_effi_corr_loose.applySystematicVariation(CP::SystematicSet()));
            CHECK_CPCorr(m_effi_corr_loose.getEfficiencyScaleFactor(**mu_itr, sf));
            float centralSFLoose = sf;
            CHECK_CPSys(m_effi_corr_loose.applySystematicVariation(statup));
            CHECK_CPCorr(m_effi_corr_loose.getEfficiencyScaleFactor(**mu_itr, sf));
            float statupSFLoose = sf;
            CHECK_CPSys(m_effi_corr_loose.applySystematicVariation(sysup));
            CHECK_CPCorr(m_effi_corr_loose.getEfficiencyScaleFactor(**mu_itr, sf));
            float upSFLoose = sf;
            CHECK_CPSys(m_effi_corr_loose.applySystematicVariation(CP::SystematicSet()));

            histSF_Loose->Fill(fabs(centralSFLoose));
            if (fabs(statupSFLoose - centralSFLoose) > 1) std::cout << "WARNING: LooseWP: Statistical error greater than 100% for muon with " << Form("pt=%.1f, eta=%.1f, phi=%.1f", (*mu_itr)->pt(), (*mu_itr)->eta(), (*mu_itr)->phi()) << std::endl;
            histSF_Loose_stat->Fill(fabs(statupSFLoose - centralSFLoose));
            if (fabs(upSFLoose - centralSFLoose) > 1) std::cout << "WARNING: LooseWP: Systematic error greater than 100% for muon with " << Form("pt=%.1f, eta=%.1f, phi=%.1f", (*mu_itr)->pt(), (*mu_itr)->eta(), (*mu_itr)->phi()) << std::endl;
            histSF_Loose_sys->Fill(fabs(upSFLoose - centralSFLoose));
            if ((*mu_itr)->pt() > 15000.) histSF_Loose_sys_pt15->Fill(fabs(upSFLoose - centralSFLoose));

            if (fabs((*mu_itr)->eta()) < 2.5) {
                histSF_Loose_NoHighEta->Fill(fabs(centralSFLoose));
                histSF_Loose_stat_NoHighEta->Fill(fabs(statupSFLoose - centralSFLoose));
                histSF_Loose_sys_NoHighEta->Fill(fabs(upSFLoose - centralSFLoose));
            }
            if (fabs((*mu_itr)->eta()) > 2.5) {
                histSF_Loose_HighEta->Fill(fabs(centralSFLoose));
                histSF_Loose_stat_HighEta->Fill(fabs(statupSFLoose - centralSFLoose));
                histSF_Loose_sys_HighEta->Fill(fabs(upSFLoose - centralSFLoose));
            }

            if (doComparison) {
                CHECK_CPSys(m_effi_corr_loose_comp.applySystematicVariation(CP::SystematicSet()));
                CHECK_CPCorr(m_effi_corr_loose_comp.getEfficiencyScaleFactor(**mu_itr, sf));
                float centralSFLooseComp = sf;
                CHECK_CPSys(m_effi_corr_loose_comp.applySystematicVariation(statup));
                CHECK_CPCorr(m_effi_corr_loose_comp.getEfficiencyScaleFactor(**mu_itr, sf));
                float statupSFLooseComp = sf;
                CHECK_CPSys(m_effi_corr_loose_comp.applySystematicVariation(sysup));
                CHECK_CPCorr(m_effi_corr_loose_comp.getEfficiencyScaleFactor(**mu_itr, sf));
                float upSFLooseComp = sf;
                CHECK_CPSys(m_effi_corr_loose_comp.applySystematicVariation(CP::SystematicSet()));

                if (fabs((*mu_itr)->eta()) < 2.5) {
                    float EffLooseComp = 1.;
                    CHECK_CPCorr(m_effi_corr_medium_comp.getDataEfficiency(**mu_itr, EffLooseComp));
                    CHECK_CPCorr(m_effi_corr_loose.getDataEfficiency(**mu_itr, eff));
                    histSFComp_Loose_NoHighEta->Fill(fabs(centralSFLoose - centralSFLooseComp));
                    histSFComp_Loose_Eff_NoHighEta->Fill(fabs(eff - EffLooseComp));
                    histSFComp_Loose_stat_NoHighEta->Fill(fabs(statupSFLoose - statupSFLooseComp));
                    histSFComp_Loose_sys_NoHighEta->Fill(fabs(upSFLoose-upSFLooseComp));

                } else if (fabs((*mu_itr)->eta()) > 2.5) {
                    histSFComp_Loose_HighEta->Fill(fabs(centralSFLoose - centralSFLooseComp));
                    histSFComp_Loose_stat_HighEta->Fill(fabs(statupSFLoose - statupSFLooseComp));
                    histSFComp_Loose_sys_HighEta->Fill(fabs(upSFLoose-upSFLooseComp));
                }
            }

            // fill the HighPt histos
            CHECK_CPSys(m_effi_corr_HighPt.applySystematicVariation(CP::SystematicSet()));
            CHECK_CPCorr(m_effi_corr_HighPt.getEfficiencyScaleFactor(**mu_itr, sf));
            float centralSFHighPt = sf;
            CHECK_CPSys(m_effi_corr_HighPt.applySystematicVariation(statup));
            CHECK_CPCorr(m_effi_corr_HighPt.getEfficiencyScaleFactor(**mu_itr, sf));
            float statupSFHighPt = sf;
            CHECK_CPSys(m_effi_corr_HighPt.applySystematicVariation(sysup));
            CHECK_CPCorr(m_effi_corr_HighPt.getEfficiencyScaleFactor(**mu_itr, sf));
            float upSFHighPt = sf;
            CHECK_CPSys(m_effi_corr_HighPt.applySystematicVariation(CP::SystematicSet()));

            histSF_HighPt->Fill(fabs(centralSFHighPt));
            if (fabs(statupSFHighPt - centralSFHighPt) > 1) std::cout << "WARNING: HighPtWP: Statistical error greater than 100% for muon with " << Form("pt=%.1f, eta=%.1f, phi=%.1f", (*mu_itr)->pt(), (*mu_itr)->eta(), (*mu_itr)->phi()) << std::endl;
            histSF_HighPt_stat->Fill(fabs(statupSFHighPt - centralSFHighPt));
            if (fabs(upSFHighPt - centralSFHighPt) > 1) std::cout << "WARNING: HighPtWP: Systematic error greater than 100% for muon with " << Form("pt=%.1f, eta=%.1f, phi=%.1f", (*mu_itr)->pt(), (*mu_itr)->eta(), (*mu_itr)->phi()) << std::endl;
            histSF_HighPt_sys->Fill(fabs(upSFHighPt - centralSFHighPt));

            if (fabs((*mu_itr)->eta()) < 2.5) {
                histSF_HighPt_NoHighEta->Fill(fabs(centralSFHighPt));
                histSF_HighPt_stat_NoHighEta->Fill(fabs(statupSFHighPt - centralSFHighPt));
                histSF_HighPt_sys_NoHighEta->Fill(fabs(upSFHighPt - centralSFHighPt));
            }
            if (fabs((*mu_itr)->eta()) > 2.5) {
                histSF_HighPt_HighEta->Fill(fabs(centralSFHighPt));
                histSF_HighPt_stat_HighEta->Fill(fabs(statupSFHighPt - centralSFHighPt));
                histSF_HighPt_sys_HighEta->Fill(fabs(upSFHighPt - centralSFHighPt));
            }

            if (doComparison) {
                CHECK_CPSys(m_effi_corr_HighPt_comp.applySystematicVariation(CP::SystematicSet()));
                CHECK_CPCorr(m_effi_corr_HighPt_comp.getEfficiencyScaleFactor(**mu_itr, sf));
                float centralSFHighPtComp = sf;
                CHECK_CPSys(m_effi_corr_HighPt_comp.applySystematicVariation(statup));
                CHECK_CPCorr(m_effi_corr_HighPt_comp.getEfficiencyScaleFactor(**mu_itr, sf));
                float statupSFHighPtComp = sf;
                CHECK_CPSys(m_effi_corr_HighPt_comp.applySystematicVariation(sysup));
                CHECK_CPCorr(m_effi_corr_HighPt_comp.getEfficiencyScaleFactor(**mu_itr, sf));
                float upSFHighPtComp = sf;
                CHECK_CPSys(m_effi_corr_HighPt_comp.applySystematicVariation(CP::SystematicSet()));

                if (fabs((*mu_itr)->eta()) < 2.5) {
                    float EffHighPtComp = 1.;
                    CHECK_CPCorr(m_effi_corr_HighPt_comp.getDataEfficiency(**mu_itr, EffHighPtComp));
                    CHECK_CPCorr(m_effi_corr_HighPt.getDataEfficiency(**mu_itr, eff));
                    histSFComp_HighPt_NoHighEta->Fill(fabs(centralSFHighPt - centralSFHighPtComp));
                    histSFComp_HighPt_Eff_NoHighEta->Fill(fabs(eff - EffHighPtComp));
                    histSFComp_HighPt_stat_NoHighEta->Fill(fabs((statupSFHighPt - statupSFHighPtComp)));
                    histSFComp_HighPt_sys_NoHighEta->Fill(fabs((upSFHighPt -upSFHighPtComp)));
                } else if (fabs((*mu_itr)->eta()) > 2.5) {
                    histSFComp_HighPt_HighEta->Fill(fabs(centralSFHighPt - centralSFHighPtComp));
                    histSFComp_HighPt_stat_HighEta->Fill(fabs(statupSFHighPt - statupSFHighPtComp));
                    histSFComp_HighPt_sys_HighEta->Fill(fabs(upSFHighPt -upSFHighPtComp));
                }
            }

            // fill the Tight histos
            CHECK_CPSys(m_effi_corr_Tight.applySystematicVariation(CP::SystematicSet()));
            CHECK_CPCorr(m_effi_corr_Tight.getEfficiencyScaleFactor(**mu_itr, sf));
            float centralSFTight = sf;
            CHECK_CPSys(m_effi_corr_Tight.applySystematicVariation(statup));
            CHECK_CPCorr(m_effi_corr_Tight.getEfficiencyScaleFactor(**mu_itr, sf));
            float statupSFTight = sf;
            CHECK_CPSys(m_effi_corr_Tight.applySystematicVariation(sysup));
            CHECK_CPCorr(m_effi_corr_Tight.getEfficiencyScaleFactor(**mu_itr, sf));
            float upSFTight = sf;
            CHECK_CPSys(m_effi_corr_Tight.applySystematicVariation(CP::SystematicSet()));

            histSF_Tight->Fill(fabs(centralSFTight));
            if (fabs(statupSFTight - centralSFTight) > 1) std::cout << "WARNING: TightWP: Statistical error greater than 100% for muon with " << Form("pt=%.1f, eta=%.1f, phi=%.1f", (*mu_itr)->pt(), (*mu_itr)->eta(), (*mu_itr)->phi()) << std::endl;
            histSF_Tight_stat->Fill(fabs(statupSFTight - centralSFTight));
            if (fabs(upSFTight - centralSFTight) > 1) std::cout << "WARNING: TightWP: Systematic error greater than 100% for muon with " << Form("pt=%.1f, eta=%.1f, phi=%.1f", (*mu_itr)->pt(), (*mu_itr)->eta(), (*mu_itr)->phi()) << std::endl;
            histSF_Tight_sys->Fill(fabs(upSFTight - centralSFTight));

            if (fabs((*mu_itr)->eta()) < 2.5) {
                histSF_Tight_NoHighEta->Fill(fabs(centralSFTight));
                histSF_Tight_stat_NoHighEta->Fill(fabs(statupSFTight - centralSFTight));
                histSF_Tight_sys_NoHighEta->Fill(fabs(upSFTight - centralSFTight));
            }
            if (fabs((*mu_itr)->eta()) > 2.5) {
                histSF_Tight_HighEta->Fill(fabs(centralSFTight));
                histSF_Tight_stat_HighEta->Fill(fabs(statupSFTight - centralSFTight));
                histSF_Tight_sys_HighEta->Fill(fabs(upSFTight - centralSFTight));
            }

            if (doComparison) {
                CHECK_CPSys(m_effi_corr_Tight_comp.applySystematicVariation(CP::SystematicSet()));
                CHECK_CPCorr(m_effi_corr_Tight_comp.getEfficiencyScaleFactor(**mu_itr, sf));
                float centralSFTightComp = sf;
                CHECK_CPSys(m_effi_corr_Tight_comp.applySystematicVariation(statup));
                CHECK_CPCorr(m_effi_corr_Tight_comp.getEfficiencyScaleFactor(**mu_itr, sf));
                float statupSFTightComp = sf;
                CHECK_CPSys(m_effi_corr_Tight_comp.applySystematicVariation(sysup));
                CHECK_CPCorr(m_effi_corr_Tight_comp.getEfficiencyScaleFactor(**mu_itr, sf));
                float upSFTightComp = sf;
                CHECK_CPSys(m_effi_corr_Tight_comp.applySystematicVariation(CP::SystematicSet()));

                if (fabs((*mu_itr)->eta()) < 2.5) {
                    float EffTightComp = 1.;
                    CHECK_CPCorr(m_effi_corr_Tight_comp.getDataEfficiency(**mu_itr, EffTightComp));
                    CHECK_CPCorr(m_effi_corr_Tight.getDataEfficiency(**mu_itr, eff));
                    histSFComp_Tight_NoHighEta->Fill(fabs(centralSFTight - centralSFTightComp));
                    histSFComp_Tight_Eff_NoHighEta->Fill(fabs(eff - EffTightComp));
                    histSFComp_Tight_stat_NoHighEta->Fill(fabs((statupSFTight -statupSFTightComp)));
                    histSFComp_Tight_sys_NoHighEta->Fill(fabs((upSFTight -upSFTightComp)));
                } else if (fabs((*mu_itr)->eta()) > 2.5) {
                    histSFComp_Tight_HighEta->Fill(fabs(centralSFTight - centralSFTightComp));
                    histSFComp_Tight_stat_HighEta->Fill(fabs(statupSFTight -statupSFTightComp));
                    histSFComp_Tight_sys_HighEta->Fill(fabs(upSFTight -upSFTightComp));
                }
            }

            // check TTVA efficiencies
            CHECK_CPSys(m_ttva_corr.applySystematicVariation(CP::SystematicSet()));
            CHECK_CPCorr(m_ttva_corr.getEfficiencyScaleFactor(**mu_itr, sf));
            float TTVAcentralSF = sf;
            CHECK_CPSys(m_ttva_corr.applySystematicVariation(statupTTVA));
            CHECK_CPCorr(m_ttva_corr.getEfficiencyScaleFactor(**mu_itr, sf));
            float TTVAstatupSF = sf;
            CHECK_CPSys(m_ttva_corr.applySystematicVariation(sysupTTVA));
            CHECK_CPCorr(m_ttva_corr.getEfficiencyScaleFactor(**mu_itr, sf));
            float TTVAupSF = sf;
            CHECK_CPSys(m_ttva_corr.applySystematicVariation(CP::SystematicSet()));

            if (((*mu_itr)->pt() > 15000.) && (fabs((*mu_itr)->eta()) < 2.5)) {
                histSF_TTVA->Fill(TTVAcentralSF);
                if (fabs(TTVAstatupSF - TTVAcentralSF) > 1) std::cout << "WARNING: TTVAWP: Statistical error greater than 100% for muon with " << Form("pt=%.1f, eta=%.1f, phi=%.1f", (*mu_itr)->pt(), (*mu_itr)->eta(), (*mu_itr)->phi()) << std::endl;
                histSF_TTVA_stat->Fill(fabs(TTVAstatupSF - TTVAcentralSF));
                if (fabs(TTVAupSF - TTVAcentralSF) > 1) std::cout << "WARNING: TTVAWP: Systematic error greater than 100% for muon with " << Form("pt=%.1f, eta=%.1f, phi=%.1f", (*mu_itr)->pt(), (*mu_itr)->eta(), (*mu_itr)->phi()) << std::endl;
                histSF_TTVA_sys->Fill(fabs(TTVAupSF - TTVAcentralSF));
                if (doComparison) {
                    CHECK_CPSys(m_ttva_corr_comp.applySystematicVariation(CP::SystematicSet()));
                    CHECK_CPCorr(m_ttva_corr_comp.getEfficiencyScaleFactor(**mu_itr, sf));
                    float centralSFTTVAComp = sf;
                    CHECK_CPSys(m_ttva_corr_comp.applySystematicVariation(statupTTVA));
                    CHECK_CPCorr(m_ttva_corr_comp.getEfficiencyScaleFactor(**mu_itr, sf));
                    float statupSFTTVAComp = sf;
                    CHECK_CPSys(m_ttva_corr_comp.applySystematicVariation(sysupTTVA));
                    CHECK_CPCorr(m_ttva_corr_comp.getEfficiencyScaleFactor(**mu_itr, sf));
                    float upSFTTVAComp = sf;
                    CHECK_CPSys(m_ttva_corr_comp.applySystematicVariation(CP::SystematicSet()));
                    float EffTTVAComp = 1.;
                    CHECK_CPCorr(m_ttva_corr_comp.getDataEfficiency(**mu_itr, EffTTVAComp));
                    CHECK_CPCorr(m_ttva_corr.getDataEfficiency(**mu_itr, eff));
                    histSFComp_TTVA->Fill(fabs(TTVAcentralSF - centralSFTTVAComp));
                    histSFComp_TTVA_Eff->Fill(fabs(eff - EffTTVAComp));
                    histSFComp_TTVA_stat->Fill(fabs(TTVAstatupSF-statupSFTTVAComp ));
                    histSFComp_TTVA_sys->Fill(fabs(TTVAupSF -upSFTTVAComp));
                }
            }

            // do the isolation part similar as reco efficinecy
            float isoeff = 0.0, isosf = 0.0;
            CHECK_CPSys(m_iso_effi_corr.applySystematicVariation(CP::SystematicSet()));
            CHECK_CPCorr(m_iso_effi_corr.getDataEfficiency(**mu_itr, isoeff));
            CHECK_CPCorr(m_iso_effi_corr.getEfficiencyScaleFactor(**mu_itr, isosf));
            float centralSFIso = isosf;
            histSF_Iso->Fill(fabs(centralSFIso));
            CHECK_CPSys(m_iso_effi_corr.applySystematicVariation(isosysup));
            CHECK_CPCorr(m_iso_effi_corr.getEfficiencyScaleFactor(**mu_itr, isosf));
            float upSFIso = isosf;
            if (fabs(upSFIso - centralSFIso) > 1) std::cout << "WARNING: Iso: Systematic error greater than 100% for muon with " << Form("pt=%.1f, eta=%.1f, phi=%.1f", (*mu_itr)->pt(), (*mu_itr)->eta(), (*mu_itr)->phi()) << std::endl;
            histSF_Iso_sys->Fill(fabs(upSFIso - centralSFIso));
            CHECK_CPSys(m_iso_effi_corr.applySystematicVariation(isostatup));
            CHECK_CPCorr(m_iso_effi_corr.getEfficiencyScaleFactor(**mu_itr, isosf));
            float statupSFIso = isosf;
            if (fabs(statupSFIso - centralSFIso) > 1) std::cout << "WARNING: Iso: Statistical error greater than 100% for muon with " << Form("pt=%.1f, eta=%.1f, phi=%.1f", (*mu_itr)->pt(), (*mu_itr)->eta(), (*mu_itr)->phi()) << std::endl;
            histSF_Iso_stat->Fill(fabs(statupSFIso - centralSFIso));
        }
    }
    double t_run = tsw.CpuTime() / entries;
    Info(APP_NAME, " MCP init took %g s", t_init);
    Info(APP_NAME, " time per event: %g s", t_run);
    
    std::cout << "Number of muons in file: " << nMuons << std::endl;

    for (auto hist : m_histos) f->WriteTObject(hist);
    f->Close();

    //get smart slimming list
    xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

    // Return gracefully:
    return 0;
}
