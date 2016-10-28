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
//     ATH_CHECK( xAOD::Init( APP_NAME ) );
    RETURN_CHECK(APP_NAME, xAOD::Init(APP_NAME));

    // Open the input file:
    const TString fileName = argv[1];
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
    if (argc > 2) {
        const Long64_t e = atoll(argv[2]);
        if (e < entries) {
            entries = e;
        }
    }

    TStopwatch tsw;
    tsw.Start();
    double t_init = tsw.CpuTime();
    tsw.Reset();

    // instantiate the PRW tool which is needed to get random runnumbers 
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
    // Initialize the PRW tool
    ASG_CHECK_SA(APP_NAME, m_prw_tool.initialize());

    // for testing SF files:
//     std::string CustomInputFolder = "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/MuonEfficiencyCorrections/160928_PostICHEP/";

    // instantiate the MCP tool
    CP::MuonEfficiencyScaleFactors m_effi_corr("TestSFClass");
    // set a working point
    ASG_CHECK_SA(APP_NAME, asg::setProperty(m_effi_corr, "WorkingPoint", "Medium"));
    // turn on audit trail functionality
    ASG_CHECK_SA(APP_NAME, asg::setProperty(m_effi_corr, "doAudit", true));
    // setting a custom input folder containing SF files: this is NOT recommended!
//     ASG_CHECK_SA( APP_NAME,asg::setProperty( m_effi_corr, "CustomInputFolder", CustomInputFolder));

    // Initialize the tool
    ASG_CHECK_SA(APP_NAME, m_effi_corr.initialize());

// test if the tool is robust against nonexistent properties being set
//    m_effi_corr.setProperty("Foo","Bar");

// try out systematics support - define a few sets to run
    CP::SystematicSet statup;
    statup.insert(CP::SystematicVariation("MUON_EFF_STAT", 1));
    // make sure the tool is not affected by other unsupported systematics in the same set
    statup.insert(CP::SystematicVariation("THETRAIN", 1));
    // running two variations affecting the MCP tool in the same set should result in a useful error message
    //  statup.insert (CP::SystematicVariation ("MUONSFSTAT", -1));  // uncomment to test (will cause abort at first muon)

    CP::SystematicSet statdown;
    statdown.insert(CP::SystematicVariation("MUON_EFF_STAT", -1));

    CP::SystematicSet sysup;
    sysup.insert(CP::SystematicVariation("MUON_EFF_SYS", 1));

    CP::SystematicSet sysdown;
    sysdown.insert(CP::SystematicVariation("MUON_EFF_SYS", -1));

    // instantiate an MCP tool for testing TTVA scale factors
    CP::MuonEfficiencyScaleFactors m_ttva_corr("TestTTVAClass");
    ASG_CHECK_SA(APP_NAME, asg::setProperty(m_ttva_corr, "WorkingPoint", "TTVA"));
    ASG_CHECK_SA(APP_NAME, asg::setProperty(m_ttva_corr, "doAudit", true));
    // setting a custom input folder containing SF files: this is NOT recommended!
    // ASG_CHECK_SA( APP_NAME,asg::setProperty( m_ttva_corr, "CustomInputFolder", CustomInputFolder));
    ASG_CHECK_SA(APP_NAME, m_ttva_corr.initialize());

    CP::SystematicSet TTVAstatup;
    TTVAstatup.insert(CP::SystematicVariation("MUON_TTVA_STAT", 1));
    CP::SystematicSet TTVAsysup;
    TTVAsysup.insert(CP::SystematicVariation("MUON_TTVA_SYS", 1));
    CP::SystematicSet TTVAstatdown;
    TTVAstatdown.insert(CP::SystematicVariation("MUON_TTVA_STAT", -1));
    CP::SystematicSet TTVAsysdown;
    TTVAsysdown.insert(CP::SystematicVariation("MUON_TTVA_SYS", -1));

    // instance for isolation scale factor
    CP::MuonEfficiencyScaleFactors m_iso_effi_corr("TestIsolationSF");
    ASG_CHECK_SA(APP_NAME, m_iso_effi_corr.setProperty("WorkingPoint", "GradientIso"));
    // setting a custom input folder containing SF files: this is NOT recommended!
    // ASG_CHECK_SA( APP_NAME,asg::setProperty( m_iso_effi_corr, "CustomInputFolder", CustomInputFolder));
    ASG_CHECK_SA(APP_NAME, m_iso_effi_corr.initialize());
    CP::SystematicSet isosysup;
    isosysup.insert(CP::SystematicVariation("MUON_ISO_SYS", 1));
    CP::SystematicSet isobothup;
    // isobothup.insert (CP::SystematicVariation ("MUON_ISO_SYS", 1));
    isobothup.insert(CP::SystematicVariation("MUON_ISO_STAT", 1));

    // Loop over the events:
    tsw.Start();

    // prepare a vector to hold SF replicas
    // the size will tell the tool how many you want
    // -> we will try 50!
    std::vector<float> replicas(50);
    for (Long64_t entry = 0; entry < entries; ++entry) {

        // Tell the object which entry to look at:
        event.getEntry(entry);

        // Print some event information for fun:
        const xAOD::EventInfo* ei = 0;
        RETURN_CHECK(APP_NAME, event.retrieve(ei, "EventInfo"));
        //Apply the prwTool first before calling the efficiency correction methods
        RETURN_CHECK(APP_NAME,m_prw_tool->apply(*ei) );
        Info(APP_NAME, "===>>>  start processing event #%i, run #%i %i events processed so far  <<<===", static_cast<int>(ei->eventNumber()), static_cast<int>(ei->runNumber()), static_cast<int>(entry));

        // Get the Muons from the event:
        const xAOD::MuonContainer* muons = 0;
        RETURN_CHECK(APP_NAME, event.retrieve(muons, "Muons"));
        Info(APP_NAME, "Number of muons: %i", static_cast<int>(muons->size()));

        // Print their properties, using the tools:
        xAOD::MuonContainer::const_iterator mu_itr = muons->begin();
        xAOD::MuonContainer::const_iterator mu_end = muons->end();

        for (; mu_itr != mu_end; ++mu_itr) {

            // Print some info about the selected muon:
            Info(APP_NAME, "  Selected muon: eta = %g, phi = %g, pt = %g", (*mu_itr)->eta(), (*mu_itr)->phi(), (*mu_itr)->pt());

            // Use the "simple interface" of the tool(s):
            float eff = 0.0, sf = 0.0;

            // directly obtain some efficiencies and SF

            CHECK_CPCorr(m_effi_corr.getDataEfficiency(**mu_itr, eff));
            Info(APP_NAME, "        efficiency = %g", eff);

            CHECK_CPSys(m_effi_corr.applySystematicVariation(CP::SystematicSet()));
            CHECK_CPCorr(m_effi_corr.getEfficiencyScaleFactor(**mu_itr, sf));
            Info(APP_NAME, "       Central scaleFactor = %g", sf);
            // if in audit mode, this should return a true
            Info(APP_NAME, "    Already applied this SF: %d", m_effi_corr.AlreadyApplied(**mu_itr));
            CHECK_CPSys(m_effi_corr.applySystematicVariation(statup));
            // and this a false (since we are looking at a different systematic)
            Info(APP_NAME, "    Already applied this SF: %d", m_effi_corr.AlreadyApplied(**mu_itr));
            //         }
            CHECK_CPCorr(m_effi_corr.getEfficiencyScaleFactor(**mu_itr, sf));
            Info(APP_NAME, "           Stat Up scaleFactor = %g", sf);
            CHECK_CPSys(m_effi_corr.applySystematicVariation(statdown));
            CHECK_CPCorr(m_effi_corr.getEfficiencyScaleFactor(**mu_itr, sf));
            Info(APP_NAME, "           Stat Down scaleFactor = %g", sf);
            CHECK_CPSys(m_effi_corr.applySystematicVariation(sysup));
            CHECK_CPCorr(m_effi_corr.getEfficiencyScaleFactor(**mu_itr, sf));
            Info(APP_NAME, "           Sys Up scaleFactor = %g", sf);
            CHECK_CPSys(m_effi_corr.applySystematicVariation(sysdown));
            CHECK_CPCorr(m_effi_corr.getEfficiencyScaleFactor(**mu_itr, sf));
            Info(APP_NAME, "           Sys Down scaleFactor = %g", sf);
            CHECK_CPSys(m_effi_corr.applySystematicVariation(CP::SystematicSet()));

            // uncomment to try out replica genration (commented as it produces a lot of text)
            CHECK_CPCorr(m_effi_corr.getEfficiencyScaleFactorReplicas(**mu_itr, replicas));
            //
            //
            for (size_t t = 0; t < replicas.size(); t++) {
                Info(APP_NAME, "       scaleFactor Replica %d = %.8f", static_cast<int>(t), replicas[t]);
            }

            // also run in decorating mode... Note that here we use our non-const copy created further up
            CHECK_CPCorr(m_effi_corr.applyEfficiencyScaleFactor(**mu_itr));
            CHECK_CPCorr(m_effi_corr.applyDataEfficiency(**mu_itr));
            // now we can retrieve the info from the muon directly:
            Info(APP_NAME, "       efficiency from decorated muon = %g", (**mu_itr).auxdataConst<float>("Efficiency"));
            Info(APP_NAME, "       SF from decorated muon = %g", (**mu_itr).auxdataConst<float>("EfficiencyScaleFactor"));

            CHECK_CPCorr(m_ttva_corr.getDataEfficiency(**mu_itr, eff));
            Info(APP_NAME, "        TTVA efficiency = %g", eff);

            CHECK_CPSys(m_ttva_corr.applySystematicVariation(CP::SystematicSet()));
            CHECK_CPCorr(m_ttva_corr.getEfficiencyScaleFactor(**mu_itr, sf));
            Info(APP_NAME, "       TTVA Central scaleFactor = %g", sf);
            // if in audit mode, this should return a true
            Info(APP_NAME, "    TTVA Already applied this SF: %d", m_ttva_corr.AlreadyApplied(**mu_itr));
            CHECK_CPSys(m_ttva_corr.applySystematicVariation(TTVAstatup));
            // and this a false (since we are looking at a different systematic)
            Info(APP_NAME, "    TTVA Already applied this SF: %d", m_ttva_corr.AlreadyApplied(**mu_itr));
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

            // if we run in audit trail mode, we get some info
            //             Info( APP_NAME,"    Muon Audit info: MuonEfficiencyCorrections = %d, MuonEfficiencyCorrectionsVersion = %s, AppliedCorrections = %s",
            //												 (**mu_itr).auxdataConst< bool >( "MuonEfficiencyCorrections" ),
            //												 (**mu_itr).auxdataConst< std::string >( "MuonEfficiencyCorrectionsVersion" ).c_str(),
            //												 (**mu_itr).auxdataConst< std::string >( "AppliedCorrections" ).c_str());

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

            CHECK_CPSys(m_iso_effi_corr.applySystematicVariation(isobothup));
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
    Info(APP_NAME, " MCP init took %g s", t_init);
    Info(APP_NAME, " time per event: %g s", t_run);

    //get smart slimming list
    xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

    // Return gracefully:
    return 0;
}
