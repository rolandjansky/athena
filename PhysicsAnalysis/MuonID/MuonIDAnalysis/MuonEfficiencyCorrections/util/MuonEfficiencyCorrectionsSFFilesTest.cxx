/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

/*
 * This macro is meant for testing purposes of the MCP efficiency teams, it is not a minimal working example for analyzers,
 * please use MuonEfficiencyCorrectionsRootCoreTest instead
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
#include "AsgAnalysisInterfaces/IPileupReweightingTool.h"

#include "MuonEfficiencyCorrections/MuonSFTestHelper.h"

#define CHECK_CPCorr(Arg) \
    if (Arg.code() == CP::CorrectionCode::Error){    \
        Error(#Arg,"Correction Code 'Error' (returned in line %i) ",__LINE__); \
        return 1;   \
    }

int main(int argc, char* argv[]) {

    // force strict checking of return codes
    CP::SystematicCode::enableFailure();
    xAOD::TReturnCode::enableFailure();
    StatusCode::enableFailure();

    // The application's name:
    const char* APP_NAME = argv[0];

    std::string OutputFilename = "Applied_SFs.root";
    std::string InDir = "";
    std::string DefaultCalibRelease = "";
    std::string SFComparisonFolder = "";
    long long int nmax = -1;
    double LowPtThreshold = -1;
    // read the config provided by the user
    for (int k = 1; k < argc - 1; ++k) {
        if (std::string(argv[k]).find("-i") == 0) {
            InDir = argv[k + 1];
        }
        if (std::string(argv[k]).find("-c1") == 0) {
            DefaultCalibRelease = argv[k + 1];
        }
        if (std::string(argv[k]).find("-c2") == 0) {
            SFComparisonFolder = argv[k + 1];
        }
        if (std::string(argv[k]).find("-n") == 0) {
            nmax = atoi(argv[k + 1]);
        }
        if (std::string(argv[k]).find("-o") == 0) {
            OutputFilename = argv[k + 1];
        }
        if (std::string(argv[k]).find("-JPsi") == 0) {
            LowPtThreshold = 15.e3;
            if (OutputFilename == "Applied_SFs.root") OutputFilename = "Applied_SFs_JPsi.root";
        }
    }
    bool doComparison = !SFComparisonFolder.empty();

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

    asg::AnaToolHandle < CP::IPileupReweightingTool > m_prw_tool("CP::PileupReweightingTool/myTool");
    // This is just a placeholder configuration for testing. Do not use these config files for your analysis!
    std::vector<std::string> m_ConfigFiles { "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root" };
    std::vector<std::string> m_LumiCalcFiles { "/afs/cern.ch/atlas/project/muon/mcp/PRWFiles/ilumicalc_histograms_OflLumi-13TeV-009_data16_13TeV.periodAllYear_DetStatus-v89-pro21-01_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.root" };
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("ConfigFiles", m_ConfigFiles));
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("LumiCalcFiles", m_LumiCalcFiles));

    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("DataScaleFactor", 1.0 / 1.09));
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("DataScaleFactorUP", 1.));
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("DataScaleFactorDOWN", 1.0 / 1.18));
    ASG_CHECK_SA(APP_NAME, m_prw_tool.initialize());

    TestMuonSF::MuonSFTestHelper m_effi_corr_medium("RecoMedium");
    ASG_CHECK_SA(APP_NAME, m_effi_corr_medium.setProperty("WorkingPoint", "Medium"));
    ASG_CHECK_SA(APP_NAME, m_effi_corr_medium.setProperty("LowPtThreshold", LowPtThreshold));
    if (!DefaultCalibRelease.empty()) ASG_CHECK_SA(APP_NAME, m_effi_corr_medium.setProperty("CustomInputFolder", DefaultCalibRelease));
    TestMuonSF::MuonSFTestHelper m_effi_corr_loose("RecoLoose");
    ASG_CHECK_SA(APP_NAME, m_effi_corr_loose.setProperty("WorkingPoint", "Loose"));
    ASG_CHECK_SA(APP_NAME, m_effi_corr_loose.setProperty("LowPtThreshold", LowPtThreshold));
    if (!DefaultCalibRelease.empty()) ASG_CHECK_SA(APP_NAME, m_effi_corr_loose.setProperty("CustomInputFolder", DefaultCalibRelease));
    TestMuonSF::MuonSFTestHelper m_effi_corr_tight("RecoTight");
    ASG_CHECK_SA(APP_NAME, m_effi_corr_tight.setProperty("WorkingPoint", "Tight"));
    ASG_CHECK_SA(APP_NAME, m_effi_corr_tight.setProperty("LowPtThreshold", LowPtThreshold));
    if (!DefaultCalibRelease.empty()) ASG_CHECK_SA(APP_NAME, m_effi_corr_tight.setProperty("CustomInputFolder", DefaultCalibRelease));
    TestMuonSF::MuonSFTestHelper m_effi_corr_HighPt("RecoHighPt");
    ASG_CHECK_SA(APP_NAME, m_effi_corr_HighPt.setProperty("WorkingPoint", "HighPt"));
    if (!DefaultCalibRelease.empty()) ASG_CHECK_SA(APP_NAME, m_effi_corr_HighPt.setProperty("CustomInputFolder", DefaultCalibRelease));

    TestMuonSF::MuonSFTestHelper m_effi_corr_ttva("TTVALoose");
    ASG_CHECK_SA(APP_NAME, m_effi_corr_ttva.setProperty("WorkingPoint", "TTVA"));
    if (!DefaultCalibRelease.empty()) ASG_CHECK_SA(APP_NAME, m_effi_corr_ttva.setProperty("CustomInputFolder", DefaultCalibRelease));

    TestMuonSF::MuonSFTestHelper m_effi_corr_BadMuonVeto("BadMuonVetoHighPt");
    ASG_CHECK_SA(APP_NAME, m_effi_corr_BadMuonVeto.setProperty("WorkingPoint", "BadMuonVeto"));
    if (!DefaultCalibRelease.empty()) ASG_CHECK_SA(APP_NAME, m_effi_corr_BadMuonVeto.setProperty("CustomInputFolder", DefaultCalibRelease));

    TestMuonSF::MuonSFTestHelper m_effi_corr_iso("IsoGradientIso");
    ASG_CHECK_SA(APP_NAME, m_effi_corr_iso.setProperty("WorkingPoint", "GradientIso"));
    if (!DefaultCalibRelease.empty()) ASG_CHECK_SA(APP_NAME, m_effi_corr_iso.setProperty("CustomInputFolder", DefaultCalibRelease));

    TestMuonSF::MuonSFTestHelper m_effi_corr_medium_comp("CompMediumSFs");
    ASG_CHECK_SA(APP_NAME, m_effi_corr_medium_comp.setProperty("WorkingPoint", "Medium"));
    ASG_CHECK_SA(APP_NAME, m_effi_corr_medium_comp.setProperty("LowPtThreshold", LowPtThreshold));
    ASG_CHECK_SA(APP_NAME, m_effi_corr_medium_comp.setProperty("CustomInputFolder", SFComparisonFolder));
    TestMuonSF::MuonSFTestHelper m_effi_corr_loose_comp("CompLooseSFs");
    ASG_CHECK_SA(APP_NAME, m_effi_corr_loose_comp.setProperty("WorkingPoint", "Loose"));
    ASG_CHECK_SA(APP_NAME, m_effi_corr_loose_comp.setProperty("LowPtThreshold", LowPtThreshold));
    ASG_CHECK_SA(APP_NAME, m_effi_corr_loose_comp.setProperty("CustomInputFolder", SFComparisonFolder));
    TestMuonSF::MuonSFTestHelper m_effi_corr_tight_comp("CompTightSFs");
    ASG_CHECK_SA(APP_NAME, m_effi_corr_tight_comp.setProperty("WorkingPoint", "Tight"));
    ASG_CHECK_SA(APP_NAME, m_effi_corr_tight_comp.setProperty("LowPtThreshold", LowPtThreshold));
    ASG_CHECK_SA(APP_NAME, m_effi_corr_tight_comp.setProperty("CustomInputFolder", SFComparisonFolder));
    TestMuonSF::MuonSFTestHelper m_effi_corr_HighPt_comp("CompHighPtSFs");
    ASG_CHECK_SA(APP_NAME, m_effi_corr_HighPt_comp.setProperty("WorkingPoint", "HighPt"));
    ASG_CHECK_SA(APP_NAME, m_effi_corr_HighPt_comp.setProperty("CustomInputFolder", SFComparisonFolder));

    TestMuonSF::MuonSFTestHelper m_effi_corr_ttva_comp("CompTTVASFs");
    ASG_CHECK_SA(APP_NAME, m_effi_corr_ttva_comp.setProperty("WorkingPoint", "TTVA"));
    ASG_CHECK_SA(APP_NAME, m_effi_corr_ttva_comp.setProperty("CustomInputFolder", SFComparisonFolder));
    TestMuonSF::MuonSFTestHelper m_effi_corr_BadMuonVeto_comp("CompBadMuonVetoSFs");
    ASG_CHECK_SA(APP_NAME, m_effi_corr_BadMuonVeto_comp.setProperty("WorkingPoint", "BadMuonVeto"));
    ASG_CHECK_SA(APP_NAME, m_effi_corr_BadMuonVeto_comp.setProperty("CustomInputFolder", SFComparisonFolder));
    TestMuonSF::MuonSFTestHelper m_iso_corr_comp("CompIsoSF");
    ASG_CHECK_SA(APP_NAME, m_iso_corr_comp.setProperty("WorkingPoint", "GradientIso"));
    ASG_CHECK_SA(APP_NAME, m_iso_corr_comp.setProperty("CustomInputFolder", SFComparisonFolder));

    TestMuonSF::MuonSFReleaseComparer m_compare_medium(m_effi_corr_medium, m_effi_corr_medium_comp);
    TestMuonSF::MuonSFReleaseComparer m_compare_loose(m_effi_corr_loose, m_effi_corr_loose_comp);
    TestMuonSF::MuonSFReleaseComparer m_compare_tight(m_effi_corr_tight, m_effi_corr_tight_comp);
    TestMuonSF::MuonSFReleaseComparer m_compare_HighPt(m_effi_corr_HighPt, m_effi_corr_HighPt_comp);
    TestMuonSF::MuonSFReleaseComparer m_compare_ttva(m_effi_corr_ttva, m_effi_corr_ttva_comp);
    TestMuonSF::MuonSFReleaseComparer m_compare_BadMuonVeto(m_effi_corr_BadMuonVeto, m_effi_corr_BadMuonVeto_comp);
    TestMuonSF::MuonSFReleaseComparer m_compare_iso(m_effi_corr_iso, m_iso_corr_comp);

    if (doComparison) {
        ASG_CHECK_SA(APP_NAME, m_compare_medium.initialize());
        ASG_CHECK_SA(APP_NAME, m_compare_loose.initialize());
        ASG_CHECK_SA(APP_NAME, m_compare_tight.initialize());
        ASG_CHECK_SA(APP_NAME, m_compare_HighPt.initialize());
        ASG_CHECK_SA(APP_NAME, m_compare_ttva.initialize());
        ASG_CHECK_SA(APP_NAME, m_compare_BadMuonVeto.initialize());
        ASG_CHECK_SA(APP_NAME, m_compare_iso.initialize());
    } else {
        ASG_CHECK_SA(APP_NAME, m_effi_corr_medium.initialize());
        ASG_CHECK_SA(APP_NAME, m_effi_corr_loose.initialize());
        ASG_CHECK_SA(APP_NAME, m_effi_corr_tight.initialize());
        ASG_CHECK_SA(APP_NAME, m_effi_corr_HighPt.initialize());
        ASG_CHECK_SA(APP_NAME, m_effi_corr_ttva.initialize());
        ASG_CHECK_SA(APP_NAME, m_effi_corr_BadMuonVeto.initialize());
        ASG_CHECK_SA(APP_NAME, m_effi_corr_iso.initialize());
    }

    // write a root file containing the SFs, stat and sys errors
    TFile *f = new TFile(OutputFilename.c_str(), "RECREATE");

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

        for (const auto& imuon : *muons) {

            if (fabs(imuon->eta()) > 2.7) continue;
            if (fabs(imuon->pt()) < 4000.) continue;

            //ignore calo-tag muons:
            if (imuon->author() == xAOD::Muon::CaloTag) continue;

            ++nMuons;

            if (doComparison) {
                CHECK_CPCorr(m_compare_medium.TestSF(*imuon));
                CHECK_CPCorr(m_compare_loose.TestSF(*imuon));
                CHECK_CPCorr(m_compare_tight.TestSF(*imuon));
                CHECK_CPCorr(m_compare_HighPt.TestSF(*imuon));
                CHECK_CPCorr(m_compare_ttva.TestSF(*imuon));
                CHECK_CPCorr(m_compare_BadMuonVeto.TestSF(*imuon));
                CHECK_CPCorr(m_compare_iso.TestSF(*imuon));
            } else {
                CHECK_CPCorr(m_effi_corr_loose.TestSF(*imuon));
                CHECK_CPCorr(m_effi_corr_medium.TestSF(*imuon));
                CHECK_CPCorr(m_effi_corr_tight.TestSF(*imuon));
                CHECK_CPCorr(m_effi_corr_HighPt.TestSF(*imuon));
                CHECK_CPCorr(m_effi_corr_ttva.TestSF(*imuon));
                CHECK_CPCorr(m_effi_corr_BadMuonVeto.TestSF(*imuon));
                CHECK_CPCorr(m_effi_corr_iso.TestSF(*imuon));
            }
        }
    }
    double t_run = tsw.CpuTime() / entries;
    Info(APP_NAME, " MCP init took %g s", t_init);
    Info(APP_NAME, " time per event: %g s", t_run);

    std::cout << "Number of muons in file: " << nMuons << std::endl;

    m_compare_loose.WriteHistosToFile(f);
    m_compare_medium.WriteHistosToFile(f);
    m_compare_tight.WriteHistosToFile(f);
    m_compare_HighPt.WriteHistosToFile(f);
    m_compare_ttva.WriteHistosToFile(f);
    m_effi_corr_BadMuonVeto.WriteHistosToFile(f);
    m_compare_iso.WriteHistosToFile(f);

    f->Close();

    //get smart slimming list
    xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

    // Return gracefully:
    return 0;
}
