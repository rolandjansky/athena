/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// Based on CPToolTester.cxx (A. Kraznahorkay) 
/// and ut_ath_checkTrigger_test.cxx (W. Buttinger)
/// Contact: jmaurer@cern.ch
/*
 *    Trigger matching example: one can use the same configuration as in other examples;
 *    the only addition is the (mandatory) creation of a Trig::MatchingTool instance 
 *    for the "TriggerMatchingTool" property.
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

#include "AsgMessaging/MessageCheck.h"
// EDM include(s):
#include "AsgTools/AnaToolHandle.h"
#include "TriggerAnalysisInterfaces/ITrigGlobalEfficiencyCorrectionTool.h"
#include "TrigConfInterfaces/ITrigConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TriggerMatchingTool/IMatchingTool.h"
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

#define MSGSOURCE "Example 6"
// messaging
ANA_MSG_HEADER(Test)
ANA_MSG_SOURCE(Test, MSGSOURCE)
using namespace Test;
int main(int argc, char* argv[])
{
    ANA_CHECK_SET_TYPE(bool)
    const char* filename = nullptr;
    bool debug = false, fast = false, cmdline_error = false;
    for(int i=1;i<argc;++i)
    {
        if(string(argv[i]) == "--debug") debug = true;
        if(string(argv[i]) == "--fast") fast = true;
        else if(!filename && *argv[i]!='-') filename = argv[i];
        else cmdline_error = true;
    }
    if(!filename || cmdline_error)
    {
        Error(MSGSOURCE, "No file name received!");
        Error(MSGSOURCE, "  Usage: %s [--debug] [--fast] [DxAOD file name]", argv[0]);
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
    if(fast) entries = std::min(entries, 1000LL);

    /* ********************************************************************** */
    
    asg::AnaToolHandle<TrigConf::ITrigConfigTool> trigConfTool("TrigConf::xAODConfigTool/TrigConfig");
    if(trigConfTool.initialize() != StatusCode::SUCCESS)
    {
        Error(MSGSOURCE, "Unable to initialize the trigger config tool!");
        return 3;
    }
    asg::AnaToolHandle<Trig::TrigDecisionTool> trigDecTool("Trig::TrigDecisionTool/TrigDecision");
    trigDecTool.setProperty("ConfigTool", trigConfTool.getHandle()).ignore();
    trigDecTool.setProperty("TrigDecisionKey", "xTrigDecision").ignore();
    if(trigDecTool.initialize() != StatusCode::SUCCESS)
    {
        Error(MSGSOURCE, "Unable to initialize the trigger matching tool!");
        return 3;
    }
    asg::AnaToolHandle<Trig::IMatchingTool> trigMatchTool("Trig::MatchingTool/TrigMatch");
    trigMatchTool.setProperty("TrigDecisionTool", trigDecTool.getHandle()).ignore();
    if(trigMatchTool.initialize() != StatusCode::SUCCESS)
    {
        Error(MSGSOURCE, "Unable to initialize the trigger matching tool!");
        return 3;
    }
    
    /* ********************************************************************** */
    
    Info(MSGSOURCE, "Configuring the global trigger SF tool");
    asg::AnaToolHandle<ITrigGlobalEfficiencyCorrectionTool> myTool("TrigGlobalEfficiencyCorrectionTool/TrigGlobal");
    const char* triggers2015 = 
        "mu20_iloose_L1MU15_OR_mu50"
        "|| mu18_mu8noL1"
        "|| e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose"
        "|| 2e12_lhloose_L12EM10VH";
    myTool.setProperty("TriggerCombination2015", triggers2015).ignore();
    const char* triggers2016 = 
        "mu26_ivarmedium_OR_mu50"
        "|| mu22_mu8noL1"
        "|| e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0"
        "|| 2e17_lhvloose_nod0";
    myTool.setProperty("TriggerCombination2016", triggers2016).ignore();
    myTool.setProperty("TriggerMatchingTool", trigMatchTool.getHandle()).ignore();
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
    double nSuitableEvents = 0., nMatched = 0.;
    for(Long64_t entry = 0; entry < entries; ++entry)
    {
        event.getEntry(entry);
        
        /// Get a random run number, and decorate the event info
        const xAOD::EventInfo* eventInfo = nullptr;
        event.retrieve(eventInfo,"EventInfo").ignore();
        unsigned runNumber = periodRuns[uniformPdf(randomEngine)];
        eventInfo->auxdecor<unsigned>("RandomRunNumber") = runNumber;
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
            myTriggeringMuons.push_back(muon);
        }

        /// Events must contain at least one lepton above trigger threshold
        if(myTriggeringElectrons.size()+myTriggeringMuons.size() < 1) continue;
        nSuitableEvents += 1;
        
        bool matched = false;
        if(myTool->checkTriggerMatching(matched, myTriggeringElectrons, myTriggeringMuons) != CP::CorrectionCode::Ok)
        {
            Error(MSGSOURCE, "trigger matching could not be checked, interrupting execution");
            ++errors;
            break;
        }
            
        if(matched) nMatched += 1;
    }
    
    if(errors < nSuitableEvents)
    {
        Info(MSGSOURCE, "Fraction of trigger-matched events: %f (over %ld events)",
            nMatched / nSuitableEvents, long(nSuitableEvents));
    }
    
    #ifndef XAOD_STANDALONE
		ANA_CHECK(app->finalize())
    #endif
    return errors? 4 : 0;
}

