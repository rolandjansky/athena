/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopCPTools/TopTriggerCPTools.h"

#include <map>
#include <string>

// Top includes
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

// PathResolver include(s):
#include "PathResolver/PathResolver.h"

// Trigger include(s):
#include "TrigConfxAOD/xAODConfigTool.h"
#include "TriggerMatchingTool/MatchingTool.h"
#include "TrigTauMatching/TrigTauMatching.h"
#include "TrigGlobalEfficiencyCorrection/TrigGlobalEfficiencyCorrectionTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"


namespace top {

TriggerCPTools::TriggerCPTools(const std::string& name) :
    asg::AsgTool(name) {
  declareProperty("config", m_config);
  declareProperty("release_series", m_release_series );

  declareProperty( "TrigConfigTool", m_trigConfTool );
  declareProperty( "TrigDecisionTool", m_trigDecisionTool );
  declareProperty( "TrigMatchTool", m_trigMatchTool );
  declareProperty( "GlobalTriggerEffTool", m_globalTriggerEffTool );
  declareProperty( "GlobalTriggerEffToolLoose", m_globalTriggerEffToolLoose );

}

StatusCode TriggerCPTools::initialize() {
  ATH_MSG_INFO("top::TriggerCPTools initialize...");

  if (m_config->makeAllCPTools()) {
    if (!m_config->isTruthDxAOD()) {
      ///-- The configuration tool --///
      const std::string trig_config_name = "TrigConf::xAODConfigTool";
      if ( asg::ToolStore::contains<TrigConf::ITrigConfigTool>(trig_config_name) ) {
        m_trigConfTool = asg::ToolStore::get<TrigConf::ITrigConfigTool>(trig_config_name);
      } else {
        TrigConf::xAODConfigTool* configTool = new TrigConf::xAODConfigTool(trig_config_name);
        top::check( configTool->initialize() , "Failed to initialize trigger config tool" );
        m_trigConfTool = configTool;
      }

      ///-- The decision tool --///
      const std::string trig_decision_name = "Trig::TrigDecisionTool";
      if ( asg::ToolStore::contains<Trig::TrigDecisionTool>(trig_decision_name) ) {
        m_trigDecisionTool = asg::ToolStore::get<Trig::TrigDecisionTool>(trig_decision_name);
      } else {
        Trig::TrigDecisionTool* trigDecTool = new Trig::TrigDecisionTool(trig_decision_name);
        top::check( trigDecTool->setProperty("ConfigTool", m_trigConfTool ) , "Failed to setProperty" );
        top::check( trigDecTool->setProperty("TrigDecisionKey", "xTrigDecision") , "Failed to setProperty" );
        top::check( trigDecTool->initialize() , "Failed to initialize trigger decision tool" );
        m_trigDecisionTool = trigDecTool;
      }

      // Trigger matching tool
      const std::string trig_match_name = "Trig::MatchingTool";
      if (asg::ToolStore::contains<Trig::IMatchingTool>(trig_match_name)) {
        m_trigMatchTool = asg::ToolStore::get<Trig::IMatchingTool>(trig_match_name);
      } else {
        Trig::MatchingTool* trigMatchTool = new Trig::MatchingTool(trig_match_name);
        top::check(trigMatchTool->setProperty("TrigDecisionTool", m_trigDecisionTool),
                  "Failed to set trigger decision tool to trigger matching tool");
        top::check(trigMatchTool->initialize(),
                  "Failed to initialize trig. matching tool");
        m_trigMatchTool = trigMatchTool;
      }

      ///-- Tau matching --///
      if( m_config->useTaus() ){
        const std::string tauMatchName = "Trig::TrigTauMatchingTool";
        if ( asg::ToolStore::contains<Trig::ITrigTauMatchingTool>(tauMatchName) ) {
          m_trigMatchTauTool = asg::ToolStore::get<Trig::ITrigTauMatchingTool>(tauMatchName);
        }
        else {
          Trig::TrigTauMatchingTool* trigMatchTauTool = new Trig::TrigTauMatchingTool(tauMatchName);
          top::check( trigMatchTauTool->setProperty("TrigDecisionTool", m_trigDecisionTool),
                      "Failed to set trig dec tool for tau matching" );
          top::check( trigMatchTauTool->initialize(),
                      "Failed to initialize tau trigger matching tool" );
          m_trigMatchTauTool = trigMatchTauTool;
        }
      }
      ///-- Trigger global efficiency corrections --///
      top::check( this->initialiseGlobalTriggerEff(),
		  "Failed to construct global trigger efficiencies" );
    }
    else {
      ATH_MSG_INFO("top::TriggerCPTools: no need to initialise anything on truth DxAOD");
    }
  }
  else {
    ATH_MSG_INFO("top::TriggerCPTools: no need to initialise anything in mini-xAOD mode");
  }

  return StatusCode::SUCCESS;
}

StatusCode TriggerCPTools::initialiseGlobalTriggerEff(){
  
  // 1. Get trigger strings
  // eg...
  std::map<std::string,std::string> triggerCombination;
  triggerCombination["2015"] = "2e12_lhloose_L12EM10VH || e17_lhloose_mu14  || mu18_mu8noL1"
    "|| e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose || mu20_iloose_L1MU15";
  triggerCombination["2016"] = "2e17_lhvloose_nod0 || e17_lhloose_nod0_mu14  || mu22_mu8noL1 "
    "|| e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0 || mu26_ivarmedium";
  std::map<std::string,std::string> legsPerKey;
  auto cc = TrigGlobalEfficiencyCorrectionTool::suggestElectronMapKeys(triggerCombination, "2015_2017/rel21.2/Moriond_February2018_v1", legsPerKey);
  
  // Get quality
  std::string electronID             = m_config->electronID();
  std::string electronIDLoose        = m_config->electronIDLoose();
  std::string electronIsolation      = m_config->electronIsolation();
  std::string electronIsolationLoose = m_config->electronIsolationLoose();
  // Tidy name
  electronID      = mapWorkingPoints(electronID);
  electronIDLoose = mapWorkingPoints(electronIDLoose);
  // Create electron / muon tools for tight
  ToolHandleArray<IAsgElectronEfficiencyCorrectionTool> electronEffTools, electronSFTools, electronEffToolsLoose, electronSFToolsLoose;
  std::vector<asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> > electronToolsFactory, electronToolsFactoryLoose; // for RAII
  std::map<std::string,std::string> legsPerTool, legsPerToolLoose;
  int nTools = 0;
  // Loop over the triggers found (electrons)
  for(auto& kv : legsPerKey){
    const std::string& trigKey = kv.first;
    for(int j=0;j<2;++j) // one tool instance for efficiencies, another for scale factors
      {
	auto t = electronToolsFactory.emplace(electronToolsFactory.end(), "AsgElectronEfficiencyCorrectionTool/ElTrigEff_"+std::to_string(nTools++));
	top::check(t->setProperty("TriggerKey", (j? trigKey : "Eff_"+trigKey)), "Failed to set TriggerKey");
	top::check(t->setProperty("IdKey", electronID), "Failed to set IdKey");
	top::check(t->setProperty("IsoKey", electronIsolation), "Failed to set IsoKey");
	top::check(t->setProperty("CorrelationModel","TOTAL"), "Failed to set CorrelationModel");
	top::check(t->setProperty("ForceDataType", (int)PATCore::ParticleDataType::Full), "Failed to set ForceDataType");
	top::check(t->setProperty("OutputLevel", MSG::DEBUG), "Failed to set OutputLevel");
	top::check(t->initialize(), "Failed to initalise");
	auto& handles = j? electronSFTools : electronEffTools;
	handles.push_back(t->getHandle());
	std::string name = handles[handles.size()-1].name();
	legsPerTool[name] = legsPerKey[trigKey];

	auto t = electronToolsFactoryLoose.emplace(electronToolsFactoryLoose.end(), "AsgElectronEfficiencyCorrectionToolLoose/ElTrigEff_"+std::to_string(nTools++));
	top::check(t->setProperty("TriggerKey", (j? trigKey : "Eff_"+trigKey)),"Failed to set TriggerKey");
	top::check(t->setProperty("IdKey", electronIDLoose),"Failed to set IdKey");
	top::check(t->setProperty("IsoKey", electronIsolationLoose),"Failed to set IsoKey");
	top::check(t->setProperty("CorrelationModel","TOTAL"), "Failed to set CorrelationModel");
	top::check(t->setProperty("ForceDataType", (int)PATCore::ParticleDataType::Full), "Failed to set ForceDataType");
	top::check(t->setProperty("OutputLevel", MSG::DEBUG), "Failed to set OutputLevel");
	top::check(t->initialize(), "Failed to initalise");
	auto& handles = j? electronSFToolsLoose : electronEffToolsLoose;
        handles.push_back(t->getHandle());
	std::string name = handles[handles.size()-1].name();
	legsPerToolLoose[name] = legsPerKey[trigKey];
      }
  }
  


  


}

std::string TriggerCPTools::mapWorkingPoints(const std::string& type) {
  std::string working_point = "";
  // ID                                                                                                                            
  if(type == "LooseAndBLayerLLH" || type == "LooseBLayer" || type == "LooseAndBLayerLH"){
    working_point = "LooseBLayer";
  }
  if(type == "MediumLLH" || type == "Medium" || type == "MediumLH"){
    working_point = "Medium";
  }
  if(type == "TightLLH" || type == "Tight" || type == "TightLH"){
    working_point = "Tight";
  }

  return working_point;
}


}  // namespace top
