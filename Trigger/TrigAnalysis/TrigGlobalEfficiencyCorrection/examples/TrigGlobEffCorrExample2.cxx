/// Based on CPToolTester.cxx (A. Kraznahorkay) 
/// and ut_ath_checkTrigger_test.cxx (W. Buttinger)
/// Contact: jmaurer@cern.ch
/*
 *    Similar to example 1 (combination of single-lepton, dielectron and dimuon
 * triggers), but showing how to use different triggers in different data-taking
 * periods, and how to modify some of the trigger pT thresholds.
 * 
 */

// ROOT include(s):
#include <TFile.h>
#include <TError.h>

// Infrastructure include(s):
#ifdef XAOD_STANDALONE
    #include "xAODRootAccess/Init.h"
    #include "xAODRootAccess/TEvent.h"
    #include "xAODRootAccess/TStore.h"
#else
    #include "AthAnalysisBaseComps/AthAnalysisHelper.h"
    #include "POOLRootAccess/TEvent.h"
#endif

// EDM include(s):
#include "AsgTools/AnaToolHandle.h"
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"
#include "MuonAnalysisInterfaces/IMuonTriggerScaleFactors.h"
#include "TriggerAnalysisInterfaces/ITrigGlobalEfficiencyCorrectionTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "PATCore/PATCoreEnums.h"

// stdlib include(s):
#include <random>
#include <vector>
#include <array>
using std::vector;
using std::string;

#define MSGSOURCE "Example 2"

int main(int argc, char* argv[])
{
    const char* filename = nullptr;
    bool debug = false, cmdline_error = false;
    for(int i=1;i<argc;++i)
    {
        if(string(argv[i]) == "--debug") debug = true;
        else if(!filename && *argv[i]!='-') filename = argv[i];
        else cmdline_error = true;
    }
    if(!filename || cmdline_error)
    {
        Error(MSGSOURCE, "No file name received!");
        Error(MSGSOURCE, "  Usage: %s [--debug] [DxAOD file name]", argv[0]);
        return 1;
    }
    #ifdef XAOD_STANDALONE
        xAOD::Init(MSGSOURCE).ignore();
        TFile* file = TFile::Open(filename, "READ");
        if(!file)
        {
            Error(MSGSOURCE, "Unable to open file!");
            return 2;
        }
        xAOD::TEvent event(xAOD::TEvent::kClassAccess);
        xAOD::TStore store;
        StatusCode::enableFailure();
    #else
       IAppMgrUI* app = POOL::Init();
       POOL::TEvent event(POOL::TEvent::kClassAccess);
       TString file(filename);
    #endif
    event.readFrom(file).ignore();
    Long64_t entries = event.getEntries();
    Info(MSGSOURCE, "Number of events in the file: %lli", entries);

    /* ********************************************************************** */
    
    Info(MSGSOURCE, "Configuring the electron CP tools");
    /// For property 'ElectronEfficiencyTools':
    ToolHandleArray<IAsgElectronEfficiencyCorrectionTool> electronEffTools;
    /// For property 'ElectronScaleFactorTools':
    ToolHandleArray<IAsgElectronEfficiencyCorrectionTool> electronSFTools; 
        /// For property 'ListOfLegsPerTool':
    std::map<string,string> legsPerTool;

    /// RAII on-the-fly creation of electron CP tools:
    vector<asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool>> factory;
    enum{ cLEGS, cKEY };
    vector<std::array<string,2>> toolConfigs = {
         /// {<list of trigger legs>, <key in map file>}
         /// Single-electron trigger (same tool instance for 2015 and 2016):
        {"e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose, e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0",
            "SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0"}, 
        /// Dielectron trigger (same tool instance for 2015 and 2016):
        {"e12_lhloose_L1EM10VH, e17_lhvloose_nod0", 
            "DI_E_2015_e12_lhloose_L1EM10VH_2016_e17_lhvloose_nod0"}
     };

    const char* mapPath = "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/"
            "ElectronEfficiencyCorrection/2015_2016/"
            "rel20.7/Moriond_February2017_v3/map1.txt";
    for(auto& cfg : toolConfigs) /// one instance per trigger leg x working point
    for(int j=0;j<2;++j) /// two instances: 0 -> MC efficiencies, 1 -> SFs
    {
        string name = "AsgElectronEfficiencyCorrectionTool/"
                + ((j? "ElTrigEff_" : "ElTrigSF_")
                + std::to_string(factory.size()/2));
        auto t = factory.emplace(factory.end(), name);
        t->setProperty("MapFilePath", mapPath).ignore();
        t->setProperty("TriggerKey", string(j?"":"Eff_") + cfg[cKEY]).ignore();
        t->setProperty("IdKey", "Tight").ignore();
        t->setProperty("IsoKey", "Tight").ignore();

        t->setProperty("CorrelationModel", "TOTAL").ignore();
        t->setProperty("ForceDataType", (int)PATCore::ParticleDataType::Full).ignore();
        if(t->initialize() != StatusCode::SUCCESS)
        {
            Error(MSGSOURCE, "Unable to initialize the electron CP tool <%s>!",
                    t->name().c_str());
            return 3;
        }
        auto& handles = (j? electronSFTools : electronEffTools);
        handles.push_back(t->getHandle());
        /// Safer to retrieve the name from the final ToolHandle, it might be
        /// prefixed (by the parent tool name) when the handle is copied
        name = handles[handles.size()-1].name();
        legsPerTool[name] = cfg[cLEGS];

    }

    /* ********************************************************************** */
    
    Info(MSGSOURCE, "Configuring the muon CP tools");
    /// For property 'MuonTools':
    ToolHandleArray<CP::IMuonTriggerScaleFactors> muonTools;
    asg::AnaToolHandle<CP::IMuonTriggerScaleFactors> muonTool("CP::MuonTriggerScaleFactors/MuonTrigEff");
    muonTool.setProperty("CalibrationRelease", "170916_Rel21PreRec").ignore();
    muonTool.setProperty("MuonQuality", "Tight").ignore();
    // muonTool.setProperty("Isolation", "GradientLoose").ignore();
    if(muonTool.initialize() != StatusCode::SUCCESS)
    {
        Error(MSGSOURCE, "Unable to initialize the muon CP tool!");
        return 3;
    }
    muonTools.push_back(muonTool.getHandle());
    
    /* ********************************************************************** */
    
    Info(MSGSOURCE, "Configuring the global trigger SF tool");
    asg::AnaToolHandle<ITrigGlobalEfficiencyCorrectionTool> myTool("TrigGlobalEfficiencyCorrectionTool/TrigGlobal");
    myTool.setProperty("ElectronEfficiencyTools", electronEffTools).ignore();
    myTool.setProperty("ElectronScaleFactorTools", electronSFTools).ignore();
    myTool.setProperty("MuonTools", muonTools).ignore();
    /// Fill a map with the trigger combinations
    std::map<string, string> triggers;
    triggers["2015"] = 
        "mu20_iloose_L1MU15_OR_mu50"
        "|| mu18_mu8noL1"
        "|| e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose"
        "|| 2e12_lhloose_L12EM10VH";
    triggers["2016:A-2016:D"] = 
        "mu26_ivarmedium_OR_mu50"
        "|| mu22_mu8noL1"
        "|| e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0"
        "|| 2e17_lhvloose_nod0";
    triggers["303638-311481"] = /// (that's 2016 periods E-L)
        "mu22_mu8noL1"
        "|| 2e17_lhvloose_nod0";
    myTool.setProperty("TriggerCombination", triggers).ignore();
    myTool.setProperty("ListOfLegsPerTool", legsPerTool).ignore();
    /// Change pT thresholds used by the tool for mu18 and mu22
    std::map<string, string> thresholds;
    thresholds["mu18"] = "25e3";
    thresholds["mu22"] = "25e3";
    myTool.setProperty("OverrideThresholds", thresholds).ignore();

    if(debug) myTool.setProperty("OutputLevel", MSG::DEBUG).ignore();
    if(myTool.initialize() != StatusCode::SUCCESS)
    {
        Error(MSGSOURCE, "Unable to initialize the TrigGlob tool!");
        return 3;
    }
    
    /// Uniform random run number generation spanning the target dataset.
    /// In real life, use the PileupReweightingTool instead!
    const unsigned periodRuns[] = {
        276073, 278727, 279932, 280423, 281130, 282625, /// 2015 periods D-H, J
        296939, 300345, 301912, 302737, 303638, 303943, 305291, 307124, 
        305359, 309311, 310015 /// 2016 periods A-L
    };
    std::uniform_int_distribution<unsigned> uniformPdf(0,
            sizeof(periodRuns)/sizeof(*periodRuns) - 1);
    std::default_random_engine randomEngine;
    
    /* ********************************************************************** */
    
    Info(MSGSOURCE, "Starting the event loop");
    unsigned errors = 0;
    double nSuitableEvents = 0., sumW = 0.;
    for(Long64_t entry = 0; entry < entries; ++entry)
    {
        event.getEntry(entry);
        
        /// Get a random run number, and decorate the event info
        const xAOD::EventInfo* eventInfo = nullptr;
        event.retrieve(eventInfo,"EventInfo").ignore();
        unsigned runNumber = periodRuns[uniformPdf(randomEngine)];
        eventInfo->auxdecor<unsigned>("RandomRunNumber") = runNumber;

        unsigned nTrig1L = 0, nTrig2mu = 0;
        
        vector<const xAOD::Electron*> myTriggeringElectrons;
        const xAOD::ElectronContainer* electrons = nullptr;
        event.retrieve(electrons,"Electrons").ignore();
        for(auto electron : *electrons)
        {
            if(!electron->caloCluster()) continue;
            float eta = fabs(electron->caloCluster()->etaBE(2));
            float pt = electron->pt();
            if(pt<10e3f || eta>=2.47) continue;
            int t = electron->auxdata<int>("truthType");
            int o = electron->auxdata<int>("truthOrigin");
            if(t!=2 || !(o==10 || (o>=12 && o<=22) || o==43)) continue;
            /// lepton must be above softest trigger threshold (2e12/2e17 here):
            if(pt < (runNumber>290000? 18e3f : 13e3f)) continue;
            /// also count leptons above single-lepton trigger threshold
            if(pt >= (runNumber>290000? 27e3f : 25e3f)) ++nTrig1L;

            myTriggeringElectrons.push_back(electron);
        }

        vector<const xAOD::Muon*> myTriggeringMuons;
        const xAOD::MuonContainer* muons = nullptr;
        event.retrieve(muons,"Muons").ignore();
        for(auto muon : *muons)
        {
            float pt = muon->pt();
            if(pt<10e3f || fabs(muon->eta())>=2.5) continue;
            auto mt = muon->muonType();
            if(mt!=xAOD::Muon::Combined && mt!=xAOD::Muon::MuonStandAlone) continue;
            int t = muon->primaryTrackParticle()->auxdata<int>("truthType");
            int o = muon->primaryTrackParticle()->auxdata<int>("truthOrigin");
            if(t!=6 || !(o==10 || (o>=12 && o<=22) || o==43)) continue;
            /// lepton must be above softest trigger threshold (mu8noL1 here):
            if(pt < 10e3f) continue;
            /// also count leptons above single-lepton trigger threshold
            if(pt >= (runNumber>290000? 27.3e3f : 21e3f)) ++nTrig1L;
            // and count muons suitable for the hard leg of the dimuon trigger
            if(pt >= 25e3f) ++nTrig2mu;

            myTriggeringMuons.push_back(muon);
        }

        /// Events must contain enough leptons to trigger
        if((nTrig1L==0 || runNumber>=303638) /// single-lepton trigger (if used)
            && myTriggeringElectrons.size()<2 /// dielectron
            && (nTrig2mu==0 || myTriggeringMuons.size()<2)) /// dimuon
        {
            continue;
        }


        /// Finally retrieve the global trigger scale factor
        double sf = 1.;
        auto cc = myTool->getEfficiencyScaleFactor(runNumber,
                myTriggeringElectrons, myTriggeringMuons, sf);
        if(cc==CP::CorrectionCode::Ok)
        {
            nSuitableEvents += 1;
            sumW += sf;
        }
        else
        {
            Warning(MSGSOURCE, "Scale factor evaluation failed");
            ++errors;
        }
        if(errors>10)
        {
            Error(MSGSOURCE, "Too many errors reported!");
            break;
        }
    }
    Info(MSGSOURCE, "Average scale factor: %f (over %ld events)",
            sumW / nSuitableEvents, long(nSuitableEvents));
    #ifndef XAOD_STANDALONE
        app->finalize();
    #endif
    return errors? 4 : 0;
}

