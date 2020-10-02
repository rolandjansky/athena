/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

/*
 * This macro is meant for testing purposes of the MCP efficiency teams, it is not a minimal working example for analyzers,
 * please use MuonEfficiencyScaleFactorsTest or MuonTriggerSFRootCoreTest instead
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
#   include "AsgMessaging/Check.h"
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

typedef asg::AnaToolHandle<CP::IMuonEfficiencyScaleFactors> EffiToolInstance;

EffiToolInstance createSFTool(const std::string& WP, const std::string& CustomInput, bool uncorrelate_Syst, bool doJPsi, bool isComparison=false) {
    EffiToolInstance tool(std::string("CP::MuonEfficiencyScaleFactors/EffiTool_") + WP + (isComparison ? "_comp" : "" ) );

    tool.setProperty("WorkingPoint", WP).isSuccess();
    tool.setProperty("UncorrelateSystematics", uncorrelate_Syst).isSuccess();
    if (doJPsi) tool.setProperty("LowPtThreshold", 1.e14).isSuccess();
    else tool.setProperty("LowPtThreshold", -1.).isSuccess();
    if (!CustomInput.empty()) tool.setProperty("CustomInputFolder", CustomInput).isSuccess();
    tool.retrieve().isSuccess();

    return tool;
}

int main(int argc, char* argv[]) {

    // force strict checking of return codes
    CP::SystematicCode::enableFailure();
    xAOD::TReturnCode::enableFailure();
    StatusCode::enableFailure();

    // The application's name:
    const char* APP_NAME = argv[0];

    // this default is for MC16a -> data2016
    std::string prwFilename = "/afs/cern.ch/atlas/project/muon/mcp/PRWFiles/prwConfigFiles/mc16_FULLSIM_r9364_r9315_NTUP_PILEUP.root";
    std::string ilumiFilename = "/afs/cern.ch/atlas/project/muon/mcp/PRWFiles/ilumicalc_histograms_OflLumi-13TeV-009_data16_13TeV.periodAllYear_DetStatus-v89-pro21-01_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.root";
    std::string OutputFilename = "Applied_SFs.root";
    std::string InDir = "";
    std::string DefaultCalibRelease = "";
    std::string SFComparisonFolder = "";
    bool doJPsi = false;
    long long int nmax = -1;

    // read the config provided by the user
    for (int k = 1; k < argc - 1; ++k) {
        if (std::string(argv[k]).find("-i") == 0) {
            InDir = argv[k + 1];
        }
        if (std::string(argv[k]).find("--c1") == 0) {
            DefaultCalibRelease = argv[k + 1];
        }
        if (std::string(argv[k]).find("--c2") == 0) {
            SFComparisonFolder = argv[k + 1];
        }
        if (std::string(argv[k]).find("-n") == 0) {
            nmax = atoi(argv[k + 1]);
        }
        if (std::string(argv[k]).find("-o") == 0) {
            OutputFilename = argv[k + 1];
        }
        if (std::string(argv[k]).find("--JPsi") == 0) {
            doJPsi = true;
            if (OutputFilename == "Applied_SFs.root") OutputFilename = "Applied_SFs_JPsi.root";
        }
        if (std::string(argv[k]).find("--ilumi") == 0) {
            ilumiFilename = argv[k + 1];
        }
        if (std::string(argv[k]).find("--prw") == 0) {
            prwFilename = argv[k + 1];
        }
    }
    bool doComparison = !SFComparisonFolder.empty();

    // check( if we received a file name:
    if (argc < 3) {
        Error(APP_NAME, "No file name received!");
        Error(APP_NAME, "  Usage: %s -i [xAOD file name]", APP_NAME);
        return EXIT_FAILURE;
    }

    RETURN_CHECK(APP_NAME, xAOD::Init(APP_NAME));

    // Open the input file:
    const TString fileName = InDir;
    Info(APP_NAME, "Opening file: %s", fileName.Data());
    std::unique_ptr<TFile> ifile(TFile::Open(fileName, "READ"));
    if (!ifile.get()) {
        Error(APP_NAME, " Unable to load xAOD input file");
        return EXIT_FAILURE;
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

    const std::vector<std::string> WPs {
        // reconstruction WPs
        "Loose", "Medium", "Tight", "HighPt",
        // track-to-vertex-association WPs
        "TTVA",
        // BadMuon veto SFs
        "BadMuonVeto_HighPt",
        // isolation WPs
        "FixedCutLooseIso", "LooseTrackOnlyIso", "LooseIso", "GradientIso", "GradientLooseIso",
        "FixedCutTightTrackOnlyIso", "FixedCutHighPtTrackOnlyIso", "FixedCutTightIso"
    };
    std::vector<EffiToolInstance> EffiTools;
    std::vector<EffiToolInstance> ComparisonTools;

    for (auto& WP : WPs) {
        EffiTools.push_back(createSFTool(WP, DefaultCalibRelease, false, doJPsi));
        if (doComparison) ComparisonTools.push_back(createSFTool(WP, SFComparisonFolder, false, doJPsi, true));
    }

    TestMuonSF::MuonSFTestHelper PrimaryHelper(DefaultCalibRelease, true);
    TestMuonSF::MuonSFTestHelper ComparisonHelper(PrimaryHelper.tree_shared(), SFComparisonFolder);

    for (auto Effi : EffiTools) {
        PrimaryHelper.addTool(Effi);
        PrimaryHelper.addReplicaTool(Effi);
    }
    for (auto Effi : ComparisonTools) {
        ComparisonHelper.addTool(Effi);
        ComparisonHelper.addReplicaTool(Effi);
    }
    if (!PrimaryHelper.init()) return EXIT_FAILURE;
    if (doComparison && !ComparisonHelper.init()) return EXIT_FAILURE;
    double t_init = tsw.CpuTime();
    tsw.Reset();
    tsw.Start();

    unsigned int nMuons = 0;

    asg::AnaToolHandle < CP::IPileupReweightingTool > m_prw_tool("CP::PileupReweightingTool/myTool");
    // This is just a placeholder configuration for testing. Do not use these config files for your analysis!
    std::vector<std::string> m_ConfigFiles { prwFilename };
    std::vector<std::string> m_LumiCalcFiles { ilumiFilename };
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("ConfigFiles", m_ConfigFiles));
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("LumiCalcFiles", m_LumiCalcFiles));
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("DataScaleFactor", 1.0 / 1.09));
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("DataScaleFactorUP", 1.));
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("DataScaleFactorDOWN", 1.0 / 1.18));
    ASG_CHECK_SA(APP_NAME, m_prw_tool.initialize());

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
        for (const auto& mu : *muons) {
            if (PrimaryHelper.fill(mu) != CP::CorrectionCode::Ok) return EXIT_FAILURE;
            if (doComparison && ComparisonHelper.fill(mu) != CP::CorrectionCode::Ok) return EXIT_FAILURE;
            PrimaryHelper.fillTree();
        }
        nMuons = nMuons+muons->size();

    }
    double t_run = tsw.CpuTime() / entries;
    Info(APP_NAME, " MCP init took %g s", t_init);
    Info(APP_NAME, " time per event: %g s", t_run);

    std::cout << "Number of muons in file: " << nMuons << std::endl;

    f->cd();
    f->WriteObject(PrimaryHelper.tree(), PrimaryHelper.tree()->GetName());
    f->Close();

    //get smart slimming list
    xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

    // Return gracefully:
    return 0;
}
