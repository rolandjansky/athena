/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

/// a simple testing macro for the MuonEfficiencyCorrections_xAOD package in RC
/// shamelessly stolen from CPToolTests.cxx
///
/// Usage: MuonEfficiencyCorrectionsRootCoreTest <input file>
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
//#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#   include "xAODRootAccess/tools/ReturnCheck.h"
#   include "AsgTools/Check.h"
//#endif // ROOTCORE

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

int main(int argc, char* argv[]) {

    // force strict checking of return codes
    CP::SystematicCode::enableFailure();
    xAOD::TReturnCode::enableFailure();
    StatusCode::enableFailure();

    // The application's name:
    const char* APP_NAME = argv[0];

    // check( if we received a file name:
    if (argc < 2) {
        Error(APP_NAME, "No file name received!");
        Error(APP_NAME, "  Usage: %s [xAOD file name]", APP_NAME);
        return 1;
    }

    // Initialise the application:
    RETURN_CHECK(APP_NAME, xAOD::Init(APP_NAME));

    // Open the input file:
    const TString fileName = argv[1];
    Info(APP_NAME, "Opening file: %s", fileName.Data());
    std::auto_ptr<TFile> ifile(TFile::Open(fileName, "READ"));
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

    TStopwatch tsw;
    tsw.Start();
    double t_init = tsw.CpuTime();
    tsw.Reset();

    // instantiate the PRW tool which is needed to get random runnumbers 
    asg::AnaToolHandle < CP::IPileupReweightingTool > m_prw_tool("CP::PileupReweightingTool/myTool");
    // This is just a placeholder configuration for testing. Do not use these config files for your analysis!
    std::vector<std::string> m_ConfigFiles { "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root" };
    std::vector<std::string> m_LumiCalcFiles { "/afs/cern.ch/atlas/project/muon/mcp/PRWFiles/ilumicalc_histograms_data15_13TeV.periodAllYear_DetStatus-v79-repro20-02_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.root", "/afs/cern.ch/atlas/project/muon/mcp/PRWFiles/ilumicalc_histograms_data16_13TeV.periodAllYear_DetStatus-v83-pro20-15_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.root" };
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("ConfigFiles", m_ConfigFiles));
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("LumiCalcFiles", m_LumiCalcFiles));

    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("DataScaleFactor", 1.0 / 1.09));
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("DataScaleFactorUP", 1.));
    ASG_CHECK_SA(APP_NAME, m_prw_tool.setProperty("DataScaleFactorDOWN", 1.0 / 1.18));
    // Initialize the PRW tool
    ASG_CHECK_SA(APP_NAME, m_prw_tool.initialize());

    // instantiate a MuonEfficiencyScaleFactors tool used for the retrieval of Medium muon reconstruction SFs
    CP::MuonEfficiencyScaleFactors m_effi_corr("RecoMediumSFTestClass");
    // set the working point
    ASG_CHECK_SA(APP_NAME, asg::setProperty(m_effi_corr, "WorkingPoint", "Medium"));

    //This option unfolds all the statistical systematics per bin of the SFs. Please only activate
    //this *if* you know what you're doing. The world is gonna implode by that
    bool doUncorrelateSystematicsForMedium = true;
    ASG_CHECK_SA(APP_NAME, asg::setProperty(m_effi_corr, "UncorrelateSystematics", doUncorrelateSystematicsForMedium));

    // setting a custom input folder containing SF files: this is NOT recommended!
    if (!DefaultCalibRelease.empty()) ASG_CHECK_SA(APP_NAME, asg::setProperty(m_effi_corr, "CustomInputFolder", DefaultCalibRelease));

    // Initialize the tool
    ASG_CHECK_SA(APP_NAME, m_effi_corr.initialize());

    // get the list of systematics from the SystematicStore (the tool has to be initialized before)
    std::vector<CP::SystematicSet> m_SystMedium;
    for (const auto& set : CP::make_systematics_vector(m_effi_corr.recommendedSystematics())) {
        m_SystMedium.push_back(CP::SystematicSet(set));
    }

    // instance for TTVA scale factors
    CP::MuonEfficiencyScaleFactors m_ttva_corr("TTVASFTestClass");
    ASG_CHECK_SA(APP_NAME, asg::setProperty(m_ttva_corr, "WorkingPoint", "TTVA"));
    ASG_CHECK_SA(APP_NAME, asg::setProperty(m_ttva_corr, "UncorrelateSystematics", doUncorrelateSystematicsForMedium));

    // setting a custom input folder containing SF files: this is NOT recommended!
    if (!DefaultCalibRelease.empty()) ASG_CHECK_SA(APP_NAME, asg::setProperty(m_ttva_corr, "CustomInputFolder", DefaultCalibRelease));
    ASG_CHECK_SA(APP_NAME, m_ttva_corr.initialize());

    // for people not using the SystematicStore, they need to know the systematic names by heart -> not recommended!
    CP::SystematicSet TTVAstatup;
    TTVAstatup.insert(CP::SystematicVariation("MUON_EFF_TTVA_STAT", 1));
    CP::SystematicSet TTVAsysup;
    TTVAsysup.insert(CP::SystematicVariation("MUON_EFF_TTVA_SYS", 1));
    CP::SystematicSet TTVAstatdown;
    TTVAstatdown.insert(CP::SystematicVariation("MUON_EFF_TTVA_STAT", -1));
    CP::SystematicSet TTVAsysdown;
    TTVAsysdown.insert(CP::SystematicVariation("MUON_EFF_TTVA_SYS", -1));

    // instance for isolation scale factors
    CP::MuonEfficiencyScaleFactors m_iso_effi_corr("GradientIsoSFTestClass");
    ASG_CHECK_SA(APP_NAME, asg::setProperty(m_iso_effi_corr, "UncorrelateSystematics", doUncorrelateSystematicsForMedium));

    ASG_CHECK_SA(APP_NAME, m_iso_effi_corr.setProperty("WorkingPoint", "GradientIso"));
    // setting a custom input folder containing SF files: this is NOT recommended!
    if (!DefaultCalibRelease.empty()) ASG_CHECK_SA(APP_NAME, asg::setProperty(m_iso_effi_corr, "CustomInputFolder", DefaultCalibRelease));
    ASG_CHECK_SA(APP_NAME, m_iso_effi_corr.initialize());
    // for people not using the SystematicStore, they need to know the systematic names by heart -> not recommended!
    CP::SystematicSet isosysup;
    isosysup.insert(CP::SystematicVariation("MUON_EFF_ISO_SYS", 1));
    CP::SystematicSet isostatup;
    isostatup.insert(CP::SystematicVariation("MUON_EFF_ISO_STAT", 1));

    CP::MuonEfficiencyScaleFactors m_badboys_effi_corr("HighPtBadMuonVetoSFTestClass");
    ASG_CHECK_SA(APP_NAME, m_badboys_effi_corr.setProperty("WorkingPoint", "BadMuonHighPt"));
    ASG_CHECK_SA(APP_NAME, asg::setProperty(m_badboys_effi_corr, "UncorrelateSystematics", doUncorrelateSystematicsForMedium));

    // setting a custom input folder containing SF files: this is NOT recommended!
    if (!DefaultCalibRelease.empty()) ASG_CHECK_SA(APP_NAME, asg::setProperty(m_badboys_effi_corr, "CustomInputFolder", DefaultCalibRelease));
    ASG_CHECK_SA(APP_NAME, m_badboys_effi_corr.initialize());
    // for people not using the SystematicStore, they need to know the systematic names by heart -> not recommended!
    CP::SystematicSet BADMUONstatup;
    BADMUONstatup.insert(CP::SystematicVariation("MUON_EFF_BADMUON_STAT", 1));
    CP::SystematicSet BADMUONsysup;
    BADMUONsysup.insert(CP::SystematicVariation("MUON_EFF_BADMUON_SYS", 1));
    CP::SystematicSet BADMUONstatdown;
    BADMUONstatdown.insert(CP::SystematicVariation("MUON_EFF_BADMUON_STAT", -1));
    CP::SystematicSet BADMUONsysdown;
    BADMUONsysdown.insert(CP::SystematicVariation("MUON_EFF_BADMUON_SYS", -1));

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

        xAOD::MuonContainer::const_iterator mu_itr = muons->begin();
        xAOD::MuonContainer::const_iterator mu_end = muons->end();

        for (; mu_itr != mu_end; ++mu_itr) {
            bool IsHighEta = fabs((*mu_itr)->eta()) > 2.5;
            // Print some info about the selected muon:
            Info(APP_NAME, "  Selected muon: eta = %g, phi = %g, pt = %g", (*mu_itr)->eta(), (*mu_itr)->phi(), (*mu_itr)->pt());

            CHECK_CPSys(m_effi_corr.applySystematicVariation(CP::SystematicSet()));
            // directly obtain some efficiencies and SF
            float eff = 0.0, sf = 0.0;
            if (!IsHighEta) {
                CHECK_CPCorr(m_effi_corr.getDataEfficiency(**mu_itr, eff));
                Info(APP_NAME, "        data efficiency = %g", eff);
                CHECK_CPCorr(m_effi_corr.getMCEfficiency(**mu_itr, eff));
                Info(APP_NAME, "        MC efficiency = %g", eff);
            }

            float nominalSF = 1.;
            CHECK_CPCorr(m_effi_corr.getEfficiencyScaleFactor(**mu_itr, nominalSF));
            if (doUncorrelateSystematicsForMedium) std::cout << "nominal SF " << nominalSF << std::endl;
            for (const auto &sysMedium : m_SystMedium) {
                CHECK_CPSys(m_effi_corr.applySystematicVariation(sysMedium));
                CHECK_CPCorr(m_effi_corr.getEfficiencyScaleFactor(**mu_itr, sf));
                std::string sysMediumName = (sysMedium.name().empty()) ? "Nominal" : sysMedium.name();
                // only print the systematic variation in case it is the relevant one when UncorrelateSystematics is enabled
                if (!doUncorrelateSystematicsForMedium || nominalSF != sf) std::cout << sysMediumName << " scaleFactor = " << sf << std::endl;
            }
            CHECK_CPSys(m_effi_corr.applySystematicVariation(CP::SystematicSet()));

            CHECK_CPCorr(m_effi_corr.getEfficiencyScaleFactorReplicas(**mu_itr, replicas));
            for (size_t t = 0; t < replicas.size(); t++) {
                Info(APP_NAME, "       scaleFactor Replica %d = %.8f", static_cast<int>(t), replicas[t]);
            }

            // also run in decorating mode... Note that here we use our non-const copy created further up
            CHECK_CPCorr(m_effi_corr.applyEfficiencyScaleFactor(**mu_itr));
            if (!IsHighEta) {
                CHECK_CPCorr(m_effi_corr.applyDataEfficiency(**mu_itr));
                CHECK_CPCorr(m_effi_corr.applyMCEfficiency(**mu_itr));
                // now we can retrieve the info from the muon directly:
                Info(APP_NAME, "       data efficiency from decorated muon = %g", (**mu_itr).auxdataConst<float>("Efficiency"));
                Info(APP_NAME, "       MC efficiency from decorated muon = %g", (**mu_itr).auxdataConst<float>("mcEfficiency"));
                Info(APP_NAME, "       SF from decorated muon = %g", (**mu_itr).auxdataConst<float>("EfficiencyScaleFactor"));

                CHECK_CPCorr(m_badboys_effi_corr.applyEfficiencyScaleFactor(**mu_itr));
                CHECK_CPCorr(m_badboys_effi_corr.applyDataEfficiency(**mu_itr));
                CHECK_CPCorr(m_badboys_effi_corr.applyMCEfficiency(**mu_itr));

                Info(APP_NAME, "         Bad muons, come for you sf = %g", (**mu_itr).auxdataConst<float>("BADMUONEfficiencyScaleFactor"));
                Info(APP_NAME, "         Bad muons, look for data efficiency = %g", (**mu_itr).auxdataConst<float>("BADMUONEfficiency"));
                Info(APP_NAME, "         Bad muons, what you gonna do if they come for you mcEff = %g", (**mu_itr).auxdataConst<float>("BADMUONmcEfficiency"));

                CHECK_CPSys(m_badboys_effi_corr.applySystematicVariation(BADMUONstatup));
                CHECK_CPCorr(m_badboys_effi_corr.getEfficiencyScaleFactor(**mu_itr, sf));
                Info(APP_NAME, "           BADMUON Stat Up scaleFactor = %g", sf);
                CHECK_CPSys(m_badboys_effi_corr.applySystematicVariation(BADMUONstatdown));
                CHECK_CPCorr(m_badboys_effi_corr.getEfficiencyScaleFactor(**mu_itr, sf));
                Info(APP_NAME, "           BADMUON Stat Down scaleFactor = %g", sf);
                CHECK_CPSys(m_badboys_effi_corr.applySystematicVariation(BADMUONsysup));
                CHECK_CPCorr(m_badboys_effi_corr.getEfficiencyScaleFactor(**mu_itr, sf));
                Info(APP_NAME, "           BADMUON Sys Up scaleFactor = %g", sf);
                CHECK_CPSys(m_badboys_effi_corr.applySystematicVariation(BADMUONsysdown));
                CHECK_CPCorr(m_badboys_effi_corr.getEfficiencyScaleFactor(**mu_itr, sf));
                Info(APP_NAME, "           BADMUON Sys Down scaleFactor = %g", sf);
                CHECK_CPSys(m_badboys_effi_corr.applySystematicVariation(CP::SystematicSet()));

            }

            CHECK_CPCorr(m_ttva_corr.getDataEfficiency(**mu_itr, eff));
            Info(APP_NAME, "        TTVA efficiency = %g", eff);

            CHECK_CPSys(m_ttva_corr.applySystematicVariation(CP::SystematicSet()));

            CHECK_CPCorr(m_ttva_corr.getEfficiencyScaleFactor(**mu_itr, sf));
            Info(APP_NAME, "       TTVA Central scaleFactor = %g", sf);
            if (!IsHighEta) {
                CHECK_CPCorr(m_ttva_corr.applyDataEfficiency(**mu_itr));
                CHECK_CPCorr(m_ttva_corr.applyMCEfficiency(**mu_itr));
                // now we can retrieve the info from the muon directly:
                Info(APP_NAME, "       data efficiency from decorated muon = %g", (**mu_itr).auxdataConst<float>("TTVAEfficiency"));
                Info(APP_NAME, "       MC efficiency from decorated muon = %g", (**mu_itr).auxdataConst<float>("TTVAmcEfficiency"));
            } // if in audit mode, this should return a true
            CHECK_CPSys(m_ttva_corr.applySystematicVariation(TTVAstatup));
            // and this a false (since we are looking at a different systematic)
            //         }
            CHECK_CPCorr(m_ttva_corr.getEfficiencyScaleFactor(**mu_itr, sf));
            Info(APP_NAME, "           TTVA Stat Up scaleFactor = %g", sf);
            CHECK_CPSys(m_ttva_corr.applySystematicVariation(TTVAstatdown));
            CHECK_CPCorr(m_ttva_corr.getEfficiencyScaleFactor(**mu_itr, sf));
            Info(APP_NAME, "           TTVA Stat Down scaleFactor = %g", sf);
            CHECK_CPSys(m_ttva_corr.applySystematicVariation(TTVAsysup));
            CHECK_CPCorr(m_ttva_corr.getEfficiencyScaleFactor(**mu_itr, sf));
            Info(APP_NAME, "           TTVA Sys Up scaleFactor = %g", sf);
            CHECK_CPSys(m_ttva_corr.applySystematicVariation(TTVAsysdown));
            CHECK_CPCorr(m_ttva_corr.getEfficiencyScaleFactor(**mu_itr, sf));
            Info(APP_NAME, "           TTVA Sys Down scaleFactor = %g", sf);
            CHECK_CPSys(m_ttva_corr.applySystematicVariation(CP::SystematicSet()));

            // do the isolation part similar as reco efficinecy
            float isoeff = 0.0, isosf = 0.0;
            CHECK_CPSys(m_iso_effi_corr.applySystematicVariation(CP::SystematicSet()));
            CHECK_CPCorr(m_iso_effi_corr.getDataEfficiency(**mu_itr, isoeff));
            Info(APP_NAME, "       isolation efficiency = %g", isoeff);
            CHECK_CPCorr(m_iso_effi_corr.getEfficiencyScaleFactor(**mu_itr, isosf));
            Info(APP_NAME, "       isolation scaleFactor = %g", isosf);
            CHECK_CPSys(m_iso_effi_corr.applySystematicVariation(isosysup));
            CHECK_CPCorr(m_iso_effi_corr.getEfficiencyScaleFactor(**mu_itr, isosf));
            Info(APP_NAME, "           Sys Up isolation scaleFactor = %g", isosf);

            CHECK_CPSys(m_iso_effi_corr.applySystematicVariation(isostatup));
            CHECK_CPCorr(m_iso_effi_corr.getEfficiencyScaleFactor(**mu_itr, isosf));
            Info(APP_NAME, "           Sys Up isolation scaleFactor2 = %g", isosf);
            CHECK_CPCorr(m_iso_effi_corr.applyEfficiencyScaleFactor(**mu_itr));
            CHECK_CPCorr(m_iso_effi_corr.applyDataEfficiency(**mu_itr));
            // now we can retrieve the info from the muon directly:
            Info(APP_NAME, "       isolation efficiency from decorated muon = %g", (**mu_itr).auxdataConst<float>("ISOEfficiency"));
            Info(APP_NAME, "       isolation SF from decorated muon = %g", (**mu_itr).auxdataConst<float>("ISOEfficiencyScaleFactor"));
            /// Isolation test done
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
