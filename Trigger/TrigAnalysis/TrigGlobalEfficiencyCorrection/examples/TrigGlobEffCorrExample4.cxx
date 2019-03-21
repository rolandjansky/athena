/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Based on CPToolTester.cxx (A. Kraznahorkay) and ut_ath_checkTrigger_test.cxx (W. Buttinger)
// Contact: jmaurer@cern.ch

/*
 *   Examples of "automatic" configuration using TrigGlobalEfficiencyCorrectionTool::suggestElectronMapKeys()
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
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "PATCore/PATCoreEnums.h"
#include "AsgTools/AnaToolHandle.h"
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"
// The interface header is not sufficient here as this example makes use of a static method of TrigGlobalEfficiencyCorrectionTool
#include "TrigGlobalEfficiencyCorrection/TrigGlobalEfficiencyCorrectionTool.h"

// stdlib include(s):
#include <random>

bool quiet = false;

int main(int argc, char* argv[])
{
	const std::string flagQuiet("--quiet");
	for(int i=1;i<argc;++i)
	{
		if(argv[i] == flagQuiet) quiet = true;
	}
	
	const char* APP_NAME = argv[0];
	#ifdef XAOD_STANDALONE
		xAOD::Init(APP_NAME).ignore();
		StatusCode::enableFailure();
	#else
	   IAppMgrUI* app = POOL::Init();
	#endif
	
	/// Retrieve the list of electron map keys for the chosen trigger combination
	std::map<std::string,std::string> triggerCombination;
	triggerCombination["2015"] = "2e12_lhloose_L12EM10VH || e17_lhloose_mu14  || mu18_mu8noL1"
		"|| e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose || mu20_iloose_L1MU15";
	triggerCombination["2016"] = "2e17_lhvloose_nod0 || e17_lhloose_nod0_mu14  || mu22_mu8noL1 "
		"|| e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0 || mu26_ivarmedium";
	triggerCombination["2017"] = "2e17_lhvloose_nod0_L12EM15VHI || 2e24_lhvloose_nod0 || e17_lhloose_nod0_mu14  || mu22_mu8noL1 "
		"|| e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0 || mu26_ivarmedium";
	triggerCombination["2018"] = triggerCombination["2017"];
	std::map<std::string,std::string> legsPerKey, legsAlone;
	auto cc = TrigGlobalEfficiencyCorrectionTool::suggestElectronMapKeys(triggerCombination, "", legsAlone);
	if(cc == CP::CorrectionCode::Ok)
	{
		if(!quiet)
		{
			std::string msg = "List of trigger legs for this combination of triggers:\n";
			for(auto& kv : legsAlone)
			{
				msg += "   - " + kv.second + '\n';
			}
			Info(APP_NAME, "%s", msg.c_str());
		}
	}
	else
	{
		Error(APP_NAME, "Unable to find list of trigger legs!");
		return 1;
	}
	
	cc = TrigGlobalEfficiencyCorrectionTool::suggestElectronMapKeys(triggerCombination, "2015_2017/rel21.2/Consolidation_September2018_v1", legsPerKey);
	if(cc == CP::CorrectionCode::Ok)
	{
		if(!quiet)
		{
			std::string msg = "List of map keys necessary for this combination of triggers:\n";
			for(auto& kv : legsPerKey)
			{
				msg += "   - tool with key \"" + kv.first + "\" chosen for legs " + kv.second + '\n';
			}
			Info(APP_NAME, "%s", msg.c_str());
		}
	}
	else
	{
		Error(APP_NAME, "Unable to find list of map keys!");
		return 1;
	}
	
	/// Then create all the needed electron tools and initialize the TrigGlob tool
	/// using the information returned by suggestElectronMapKeys()

	/// Trigger efficiency/scale factor CP tools for electrons and muons
	ToolHandleArray<IAsgElectronEfficiencyCorrectionTool> electronEffTools, electronSFTools;
	std::vector<asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> > electronToolsFactory; // for RAII
	std::map<std::string,std::string> legsPerTool;
	int nTools = 0;
	for(auto& kv : legsPerKey)
	{
		const std::string& trigKey = kv.first;
		for(int j=0;j<2;++j) /// one tool instance for efficiencies, another for scale factors
		{
			auto t = electronToolsFactory.emplace(electronToolsFactory.end(), "AsgElectronEfficiencyCorrectionTool/ElTrigEff_"+std::to_string(nTools++));
			t->setProperty("MapFilePath", "ElectronEfficiencyCorrection/2015_2017/rel21.2/Consolidation_September2018_v1/map3.txt").ignore();
			t->setProperty("TriggerKey", (j? trigKey : "Eff_"+trigKey)).ignore();
			t->setProperty("IdKey","Tight").ignore();
			t->setProperty("IsoKey","FCTight").ignore();
			t->setProperty("CorrelationModel","TOTAL").ignore();
			t->setProperty("ForceDataType", (int)PATCore::ParticleDataType::Full).ignore();
			t->setProperty("OutputLevel", MSG::ERROR).ignore();
			if(t->initialize() != StatusCode::SUCCESS)
			{
				Error(APP_NAME, "Unable to initialize CP tool <%s>!", t->name().c_str());
				return 2;
			}
			auto& handles = j? electronSFTools : electronEffTools;
			handles.push_back(t->getHandle());
			/// Safer to retrieve the name from the final ToolHandle, it might be prefixed (by the parent tool name) when the handle is copied
			std::string name = handles[handles.size()-1].name();
			legsPerTool[name] = legsPerKey[trigKey];
		}
	}
	
    ToolHandleArray<CP::IMuonTriggerScaleFactors> muonTools;
    asg::AnaToolHandle<CP::IMuonTriggerScaleFactors> muonTool("CP::MuonTriggerScaleFactors/MuonTrigEff");
    muonTool.setProperty("MuonQuality", "Tight").ignore();
	muonTool.setProperty("OutputLevel", MSG::ERROR).ignore();
    if(muonTool.initialize() != StatusCode::SUCCESS)
    {
        Error(APP_NAME, "Unable to initialize the muon CP tool!");
        return 3;
    }
    muonTools.push_back(muonTool.getHandle());
	
	asg::AnaToolHandle<ITrigGlobalEfficiencyCorrectionTool> myTool("TrigGlobalEfficiencyCorrectionTool/MyTool");
	myTool.setProperty("ElectronEfficiencyTools",electronEffTools).ignore();
	myTool.setProperty("ElectronScaleFactorTools",electronSFTools).ignore();
	myTool.setProperty("MuonTools",muonTools).ignore();
	myTool.setProperty("TriggerCombination", triggerCombination).ignore();
	myTool.setProperty("ListOfLegsPerTool", legsPerTool).ignore();
	myTool.setProperty("OutputLevel", quiet? MSG::WARNING : MSG::INFO).ignore();
	if(myTool.initialize() != StatusCode::SUCCESS)
	{
		Error( APP_NAME, "Unable to initialize tool!" );
		return 4;
	}
	
	#ifndef XAOD_STANDALONE
		app->finalize();
	#endif
	return 0;
}
