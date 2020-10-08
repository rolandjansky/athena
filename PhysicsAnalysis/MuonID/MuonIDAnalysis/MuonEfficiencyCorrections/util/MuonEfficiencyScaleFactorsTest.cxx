/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

/// a simple testing macro for the MuonEfficiencyScaleFactors class
/// shamelessly stolen from CPToolTests.cxx
///
/// Usage: MuonEfficiencyScaleFactorsTest -i \<input file\>
// System include(s):
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
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "AsgMessaging/Check.h"

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"

// Local include(s):

#include "PATInterfaces/ISystematicsTool.h"

#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

#include "AsgTools/ToolHandle.h"
#include "AsgTools/AnaToolHandle.h"
#include "AsgAnalysisInterfaces/IPileupReweightingTool.h"
#include "MuonAnalysisInterfaces/IMuonEfficiencyScaleFactors.h"

#include "PATInterfaces/SystematicsUtil.h"

#define CHECK_CPCorr(Arg) \
    if (Arg.code() == CP::CorrectionCode::Error){    \
        Error(#Arg,"Correction Code 'Error' (returned in line %i) ",__LINE__); \
        return 1;   \
    }
#define CHECK_CPSys(Arg) \
    if (Arg.code() == CP::SystematicCode::Unsupported){    \
        Warning(#Arg,"Unsupported systematic (in line %i) ",__LINE__); \
    }      

/// Example of how to run the MuonEfficiencyCorrections package to obtain information from muons

typedef asg::AnaToolHandle<CP::IMuonEfficiencyScaleFactors> EffiToolInstance;

EffiToolInstance createSFTool(const std::string& WP, const std::string& CustomInput, bool uncorrelate_Syst) {
    EffiToolInstance tool(std::string("CP::MuonEfficiencyScaleFactors/EffiTool_") + WP);

    tool.setProperty("WorkingPoint", WP).isSuccess();
    tool.setProperty("UncorrelateSystematics", uncorrelate_Syst).isSuccess();
    tool.setProperty("LowPtThreshold", 15.e3).isSuccess();
    tool.retrieve().isSuccess();

    if (!CustomInput.empty()) tool.setProperty("CustomInputFolder", CustomInput).isSuccess();

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
    std::string InFile = "";
    long long int nmax = -1;
    // read the config provided by the user
    for (int k = 1; k < argc - 1; ++k) {
        if (std::string(argv[k]).find("-i") == 0) {
            InFile = argv[k + 1];
        }
        if (std::string(argv[k]).find("-n") == 0) {
            nmax = atoi(argv[k + 1]);
        }
        if (std::string(argv[k]).find("--ilumi") == 0) {
            ilumiFilename = argv[k + 1];
        }
        if (std::string(argv[k]).find("--prw") == 0) {
            prwFilename = argv[k + 1];
        }
    }
    // Initialise the application:
    RETURN_CHECK(APP_NAME, xAOD::Init(APP_NAME));

    // Open the input file:
    Info(APP_NAME, "Opening file: %s", InFile.c_str());
    std::unique_ptr<TFile> ifile(TFile::Open(InFile.c_str(), "READ"));
    if (!ifile.get()) {
        Error(APP_NAME, " Unable to load xAOD input file");
    }

    std::string DefaultCalibRelease("");
    if (argc == 3) {
        DefaultCalibRelease = argv[2];
        std::cout << "Found second argument (" << DefaultCalibRelease << "), assuming it is the CalibrationRelease..." << std::endl;
    }

    // Create a TEvent object:
    xAOD::TEvent event;
    RETURN_CHECK(APP_NAME, event.readFrom(ifile.get(), xAOD::TEvent::kClassAccess));
    Info(APP_NAME, "Number of events in the file: %i", static_cast<int>(event.getEntries()));

    Long64_t entries = event.getEntries();
    if ((nmax != -1) && (nmax<entries)) entries = nmax;

    TStopwatch tsw;
    tsw.Start();
    double t_init = tsw.CpuTime();
    tsw.Reset();

    // instantiate the PRW tool which is needed to get random runnumbers 
    asg::AnaToolHandle < CP::IPileupReweightingTool > m_prw_tool("CP::PileupReweightingTool/myTool");
    // This is just a placeholder configuration for testing. Do not use these config files for your analysis!
    std::vector<std::string> m_ConfigFiles {
        prwFilename
    };
    std::vector<std::string> m_LumiCalcFiles {
        ilumiFilename
    };
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("ConfigFiles", m_ConfigFiles));
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("LumiCalcFiles", m_LumiCalcFiles));

    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("DataScaleFactor", 1.0 / 1.09));
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("DataScaleFactorUP", 1.));
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("DataScaleFactorDOWN", 1.0 / 1.18));
    // Initialize the PRW tool
    ASG_CHECK_SA(APP_NAME, m_prw_tool.initialize());

     const std::vector<std::string> WPs {
         // reconstruction WPs
         "Loose", "Medium", "Tight", "HighPt", "LowPt", "LowPtMVA",
         // track-to-vertex-association WPs
         "TTVA",
         // BadMuon veto SFs
         "BadMuonVeto_HighPt",
         // isolation WPs
	 "PLVLooseIso", "PLVTightIso", "HighPtTrackOnlyIso",
	 "Loose_FixedRadIso", "Tight_FixedRadIso", "PflowLoose_FixedRadIso", "PflowTight_FixedRadIso", "TightTrackOnly_FixedRadIso",
	 "Loose_VarRadIso", "Tight_VarRadIso", "PflowLoose_VarRadIso", "PflowTight_VarRadIso", "TightTrackOnly_VarRadIso"
    };
   
    std::vector<EffiToolInstance> EffiTools;
    for (auto& WP : WPs) {
        EffiTools.push_back(createSFTool(WP, DefaultCalibRelease, false));
    }
    // Start looping over the events:
    tsw.Start();

    // prepare a vector to hold SF replicas
    // the size will tell the tool how many you want
    // -> we will try 50!
    std::vector<float> replicas(50);
    for (Long64_t entry = 0; entry < entries; ++entry) {

        // Tell the object which entry to look at:
        event.getEntry(entry);

        // Apply PRW to xAOD::EventInfo
        const xAOD::EventInfo* ei = 0;
        RETURN_CHECK(APP_NAME, event.retrieve(ei, "EventInfo"));
        //Apply the prwTool first before calling the efficiency correction methods
        RETURN_CHECK(APP_NAME, m_prw_tool->apply(*ei));
        Info(APP_NAME, "===>>>  start processing event #%i, run #%i %i events processed so far  <<<===", static_cast<int>(ei->eventNumber()), static_cast<int>(ei->runNumber()), static_cast<int>(entry));

        // Get the muons from the event:
        const xAOD::MuonContainer* muons = 0;
        RETURN_CHECK(APP_NAME, event.retrieve(muons, "Muons"));
        Info(APP_NAME, "Number of muons: %i", static_cast<int>(muons->size()));

        for (const auto& mu : *muons) {
            // Print some info about the selected muon:
            Info(APP_NAME, "  Selected muon: eta = %g, phi = %g, pt = %g", mu->eta(), mu->phi(), mu->pt());
            for (auto& effi : EffiTools) {
                for (auto& syst : CP::make_systematics_vector(effi->recommendedSystematics())) {
                    CHECK_CPSys(effi->applySystematicVariation(syst));
                    float nominalSF = 1.;
                    float data_Eff = 1.;
                    float mc_Eff = 1.;
                    CHECK_CPCorr(effi->getEfficiencyScaleFactor(*mu, nominalSF));
                    CHECK_CPCorr(effi->getDataEfficiency(*mu, data_Eff));
                    CHECK_CPCorr(effi->getMCEfficiency(*mu, mc_Eff));
                    std::string sysName = (syst.name().empty()) ? "Nominal" : syst.name();
                    Info(APP_NAME, "Applied %s variation. The scale-factor is  %.3f, the data-efficiency is %.3f and the mc-efficiency is %.3f", sysName.c_str(), nominalSF, data_Eff, mc_Eff);

                    CHECK_CPCorr(effi->getEfficiencyScaleFactorReplicas(*mu, replicas));
                    for (size_t t = 0; t < replicas.size(); t++) {
                        Info(APP_NAME, "       scaleFactor Replica %d = %.8f", static_cast<int>(t), replicas[t]);
                    }
                }
            }
        }
        // Close with a message:
        Info(APP_NAME, "===>>>  done processing event #%i, "
                "run #%i %i events processed so far  <<<===", static_cast<int>(ei->eventNumber()), static_cast<int>(ei->runNumber()), static_cast<int>(entry + 1));
    }
    double t_run = tsw.CpuTime() / entries;
    Info(APP_NAME, " MCP init took %gs", t_init);
    Info(APP_NAME, " time per event: %gs", t_run);
    //get smart slimming list
    xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

    // Return gracefully:
    return 0;
}
