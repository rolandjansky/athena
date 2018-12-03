/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// Based on CPToolTester.cxx (A. Kraznahorkay) 
/// and ut_ath_checkTrigger_test.cxx (W. Buttinger)
/// Contact: jmaurer@cern.ch
{head_comments}

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
#include "AthContainers/AuxElement.h"

// stdlib include(s):
[SPLITFUNC]
#include <sstream>
[/SPLITFUNC]
#include <random>
#include <vector>
#include <array>
using std::vector;
using std::string;

[SPLITFUNC]
/// Helper function to split comma-delimited strings
namespace { vector<string> split_comma_delimited(const std::string&); }

[/SPLITFUNC]
#define MSGSOURCE "Example {example_number}"

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

[ELECTRONS]
    /* ********************************************************************** */
    
    Info(MSGSOURCE, "Configuring the electron CP tools");
    /// For property 'ElectronEfficiencyTools':
    ToolHandleArray<IAsgElectronEfficiencyCorrectionTool> electronEffTools;
    /// For property 'ElectronScaleFactorTools':
    ToolHandleArray<IAsgElectronEfficiencyCorrectionTool> electronSFTools; 
    {electron_extraproperties_declare}
    /// RAII on-the-fly creation of electron CP tools:
    vector<asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool>> factory;
[MULTITRIGGERS]
{electron_toolconfigs}
[/MULTITRIGGERS]
    const char* mapPath = "ElectronEfficiencyCorrection/2015_2017/"
            "rel21.2/Moriond_February2018_v2/map6.txt";
[MULTITRIGGERS]
    for(auto& cfg : toolConfigs) /// one instance per trigger leg x working point
[/MULTITRIGGERS]
    for(int j=0;j<2;++j) /// two instances: 0 -> MC efficiencies, 1 -> SFs
    {
        string name = "AsgElectronEfficiencyCorrectionTool/"
                + ((j? "ElTrigEff_" : "ElTrigSF_")
                + std::to_string(factory.size()/2));
        auto t = factory.emplace(factory.end(), name);
        t->setProperty("MapFilePath", mapPath).ignore();
{electron_tools_properties_set}
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
{electron_extraproperties_fill}
    }

[/ELECTRONS]
[MUONS]
    /* ********************************************************************** */
    
    Info(MSGSOURCE, "Configuring the muon CP tools");
    /// For property 'MuonTools':
    ToolHandleArray<CP::IMuonTriggerScaleFactors> muonTools;
    asg::AnaToolHandle<CP::IMuonTriggerScaleFactors> muonTool("CP::MuonTriggerScaleFactors/MuonTrigEff");
    muonTool.setProperty("CalibrationRelease", "180905_TriggerUpdate").ignore();
    muonTool.setProperty("MuonQuality", "Tight").ignore();
    muonTool.setProperty("useRel207", false).ignore();
    if(muonTool.initialize() != StatusCode::SUCCESS)
    {
        Error(MSGSOURCE, "Unable to initialize the muon CP tool!");
        return 3;
    }
    muonTools.push_back(muonTool.getHandle());
{muon_extraproperties_fill}
    
[/MUONS]
    /* ********************************************************************** */
    
    Info(MSGSOURCE, "Configuring the global trigger SF tool");
    asg::AnaToolHandle<ITrigGlobalEfficiencyCorrectionTool> myTool("TrigGlobalEfficiencyCorrectionTool/TrigGlobal");
[ELECTRONS]
    myTool.setProperty("ElectronEfficiencyTools", electronEffTools).ignore();
    myTool.setProperty("ElectronScaleFactorTools", electronSFTools).ignore();
[/ELECTRONS]
[MUONS]
    myTool.setProperty("MuonTools", muonTools).ignore();
[/MUONS]
{trigglob_properties_set}
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
[DATA2015]
        /// 2015 periods D-H, J
        276073, 278727, 279932, 280423, 281130, 282625 MAYBE_COMMA
[/DATA2015]
[DATA2016]
        /// 2016 periods A-L
        296939, 300345, 301912, 302737, 303638, 303943, 305291, 307124, 
        305359, 309311, 310015 MAYBE_COMMA
[/DATA2016]
[DATA2017]
        /// 2017 periods B-K
        325713, 329385, 330857, 332720, 334842, 335302, 336497, 336832, 
        338183 MAYBE_COMMA
[/DATA2017]
    };
    std::uniform_int_distribution<unsigned> uniformPdf(0,
            sizeof(periodRuns)/sizeof(*periodRuns) - 1);
    std::default_random_engine randomEngine;
    
    SG::AuxElement::ConstAccessor<int> truthType("truthType");
    SG::AuxElement::ConstAccessor<int> truthOrigin("truthOrigin");
    
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

{eventloop_trigcounters}        
        vector<const xAOD::Electron*> myTriggeringElectrons;
[ELECTRONS]
        const xAOD::ElectronContainer* electrons = nullptr;
        event.retrieve(electrons,"Electrons").ignore();
        for(auto electron : *electrons)
        {
            if(!electron->caloCluster()) continue;
            float eta = fabs(electron->caloCluster()->etaBE(2));
            float pt = electron->pt();
            if(pt<10e3f || eta>=2.47) continue;
            if(!truthType.isAvailable(*electron)) continue;
            if(!truthOrigin.isAvailable(*electron)) continue;
            int t = truthType(*electron), o = truthOrigin(*electron);
            if(t!=2 || !(o==10 || (o>=12 && o<=22) || o==43)) continue;
{eventloop_electron_selection}
            myTriggeringElectrons.push_back(electron);
        }

[/ELECTRONS]
        vector<const xAOD::Muon*> myTriggeringMuons;
[MUONS]
        const xAOD::MuonContainer* muons = nullptr;
        event.retrieve(muons,"Muons").ignore();
        for(auto muon : *muons)
        {
            if(runNumber >= 324320) break; // delete line once all SFs available for 2017
            float pt = muon->pt();
            if(pt<10e3f || fabs(muon->eta())>=2.5) continue;
            auto mt = muon->muonType();
            if(mt!=xAOD::Muon::Combined && mt!=xAOD::Muon::MuonStandAlone) continue;
            auto& mtp = *(muon->primaryTrackParticle());
            if(!truthType.isAvailable(mtp)) continue;
            if(!truthOrigin.isAvailable(mtp)) continue;
            int t = truthType(mtp), o = truthOrigin(mtp);
            if(t!=6 || !(o==10 || (o>=12 && o<=22) || o==43)) continue;
{eventloop_muon_selection}
            myTriggeringMuons.push_back(muon);
        }
[/MUONS]

{trigger_matching_requirements}

        /// Finally retrieve the global trigger scale factor
        double sf = 1.;
        auto cc = myTool->getEfficiencyScaleFactor(myTriggeringElectrons,
			myTriggeringMuons, sf);
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

[SPLITFUNC]
/// Split comma-delimited string
namespace
{
	inline vector<string> split_comma_delimited(const string& s)
	{
		std::stringstream ss(s);
		std::vector<std::string> tokens;
		std::string token;
		while(std::getline(ss, token, ','))
		{
			if(token.length()) tokens.push_back(token);
		}
		return tokens;
	}
}
[/SPLITFUNC]