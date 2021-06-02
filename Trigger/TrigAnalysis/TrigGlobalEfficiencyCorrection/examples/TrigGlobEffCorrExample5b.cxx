/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// Based on CPToolTester.cxx (A. Kraznahorkay) 
/// and ut_ath_checkTrigger_test.cxx (W. Buttinger)
/// Contact: jmaurer@cern.ch
/*
 *    Another example showing how to configure the tool for photon triggers. 
 *    This time for a more complex configuration (asymmetric diphoton trigger);
 *    see also the explanations in the example 1 as the configuration for photon triggers 
 *    is very similar to that for electron triggers. 
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
#include "EgammaAnalysisInterfaces/IAsgPhotonEfficiencyCorrectionTool.h"
#include "TriggerAnalysisInterfaces/ITrigGlobalEfficiencyCorrectionTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/PhotonContainer.h"
#include "PATCore/PATCoreEnums.h"

// stdlib include(s):
#include <random>
#include <vector>
#include <array>
using std::vector;
using std::string;

#define MSGSOURCE "Example 5b"

int main(int argc, char* argv[])
{
    const char* filename = nullptr;
    bool debug = false, cmdline_error = false, toys = false;
    for(int i=1;i<argc;++i)
    {
        if(string(argv[i]) == "--debug") debug = true;
        else if(string(argv[i]) == "--toys") toys = true;
        else if(!filename && *argv[i]!='-') filename = argv[i];
        else cmdline_error = true;
    }
    if(!filename || cmdline_error)
    {
        Error(MSGSOURCE, "No file name received!");
        Error(MSGSOURCE, "  Usage: %s [--debug] [--toys] [DxAOD file name]", argv[0]);
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
    
    Info(MSGSOURCE, "Configuring the photon CP tools");
    /// For property 'PhotonEfficiencyTools':
    ToolHandleArray<IAsgPhotonEfficiencyCorrectionTool> photonEffTools;
    /// For property 'PhotonScaleFactorTools':
    ToolHandleArray<IAsgPhotonEfficiencyCorrectionTool> photonSFTools; 
    /// For property 'ListOfLegsPerTool':
    std::map<string,string> legsPerTool;

    /// RAII on-the-fly creation of photon CP tools:
    vector<asg::AnaToolHandle<IAsgPhotonEfficiencyCorrectionTool>> factory;
    enum{ cLEGS, cKEY };
    vector<std::array<string,2>> toolConfigs = {
         /// {<list of trigger legs>, <key in map file>}
        {"g35_loose, g35_medium_L1EM20VH", "DI_PH_2015_g35_loose_2016_g35_loose_2017_g35_medium_L1EM20VH_2018_g35_medium_L1EM20VH"},
        {"g25_loose, g25_medium_L1EM20VH", "DI_PH_2015_g25_loose_2016_g25_loose_2017_g25_medium_L1EM20VH_2018_g25_medium_L1EM20VH"}
     };

    const char* mapPath = "PhotonEfficiencyCorrection/2015_2018/"
            "rel21.2/Summer2018_Rec_v1/map1.txt";
    for(auto& cfg : toolConfigs) /// one instance per trigger leg x working point
    for(int j=0;j<2;++j) /// two instances: 0 -> MC efficiencies, 1 -> SFs
    {
        string name = "AsgPhotonEfficiencyCorrectionTool/"
                + ((j? "PhTrigEff_" : "PhTrigSF_")
                + std::to_string(factory.size()/2));
        auto t = factory.emplace(factory.end(), name);
        t->setProperty("MapFilePath", mapPath).ignore();
        t->setProperty("TriggerKey", string(j?"":"Eff_") + cfg[cKEY]).ignore();
        t->setProperty("IsoKey", "Loose").ignore();
        t->setProperty("ForceDataType", (int)PATCore::ParticleDataType::Full).ignore();
        if(t->initialize() != StatusCode::SUCCESS)
        {
            Error(MSGSOURCE, "Unable to initialize the photon CP tool <%s>!",
                    t->name().c_str());
            return 3;
        }
        auto& handles = (j? photonSFTools : photonEffTools);
        handles.push_back(t->getHandle());
        /// Safer to retrieve the name from the final ToolHandle, it might be
        /// prefixed (by the parent tool name) when the handle is copied
        name = handles[handles.size()-1].name();
        legsPerTool[name] = cfg[cLEGS];
    }
    
    /* ********************************************************************** */
    
    Info(MSGSOURCE, "Configuring the global trigger SF tool");
    asg::AnaToolHandle<ITrigGlobalEfficiencyCorrectionTool> myTool("TrigGlobalEfficiencyCorrectionTool/TrigGlobal");
    myTool.setProperty("PhotonEfficiencyTools", photonEffTools).ignore();
    myTool.setProperty("PhotonScaleFactorTools", photonSFTools).ignore();
    myTool.setProperty("TriggerCombination2015", "g35_loose_g25_loose").ignore();
    myTool.setProperty("TriggerCombination2016", "g35_loose_g25_loose").ignore();
    myTool.setProperty("TriggerCombination2017", "g35_medium_g25_medium_L12EM20VH").ignore();
    myTool.setProperty("TriggerCombination2018", "g35_medium_g25_medium_L12EM20VH").ignore();
    myTool.setProperty("ListOfLegsPerTool", legsPerTool).ignore();

    if(debug) myTool.setProperty("OutputLevel", MSG::DEBUG).ignore();
    if(toys) myTool.setProperty("NumberOfToys", 1000).ignore();
    if(myTool.initialize() != StatusCode::SUCCESS)
    {
        Error(MSGSOURCE, "Unable to initialize the TrigGlob tool!");
        return 3;
    }

    /// Uniform random run number generation spanning the target dataset.
    /// In real life, use the PileupReweightingTool instead!
    const unsigned periodRuns[] = {
        /// 2015 periods D3-H, J
        276262, 278727, 279932, 280423, 281130, 282625,
        /// 2016 periods A3-L
        297730, 300345, 301912, 302737, 303638, 303943, 305291, 307124, 
        305359, 309311, 310015,
        /// 2017 periods B-K
        325713, 329385, 330857, 332720, 334842, 336497, 336832, 338183,
        /// 2018 periods B-M
        348885, 349534, 350310, 352274, 354107, 354826, 355261, 355331,
        355529, 357050, 359191
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

        vector<const xAOD::Photon*> myTriggeringPhotons;
        const xAOD::PhotonContainer* photons = nullptr;
        event.retrieve(photons,"Photons").ignore();
        unsigned n36 = 0;
        for(auto photon : *photons)
        {
            if(!photon->caloCluster()) continue;
            float eta = fabs(photon->caloCluster()->etaBE(2));
            float pt = photon->pt();
            if(pt<10e3f || eta>=2.37 || (eta>1.37 && eta<1.52)) continue;
            int t = photon->auxdata<int>("truthType");
            if(t!=14) continue;
            /// photon must be above trigger threshold for the softest leg:
            if(pt < 30e3f) continue;
            myTriggeringPhotons.push_back(photon);
			if(pt > 40e3f) ++n36; /// also counting those suitable for the highest-pT leg
        }

        /// Events must contain enough photons to trigger
        if(myTriggeringPhotons.size()<2 || n36<1) continue;

        /// Finally retrieve the global trigger scale factor
        double sf = 1.;
        auto cc = myTool->getEfficiencyScaleFactor(myTriggeringPhotons, sf);
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
