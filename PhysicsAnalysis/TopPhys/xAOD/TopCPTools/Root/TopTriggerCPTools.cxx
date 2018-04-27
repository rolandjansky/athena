/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopCPTools/TopTriggerCPTools.h"

#include <map>
#include <string>
#include <vector>

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
#include "MuonAnalysisInterfaces/IMuonTriggerScaleFactors.h"
#include "MuonEfficiencyCorrections/MuonTriggerScaleFactors.h"
#include "PATCore/PATCoreEnums.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/join.hpp>
#include "TopConfiguration/ConfigurationSettings.h"

namespace top {

TriggerCPTools::TriggerCPTools(const std::string& name) :
  asg::AsgTool(name){
  declareProperty("config", m_config);
  declareProperty("release_series", m_release_series );

  declareProperty( "TrigConfigTool", m_trigConfTool );
  declareProperty( "TrigDecisionTool", m_trigDecisionTool );
  declareProperty( "TrigMatchTool", m_trigMatchTool );
  declareProperty( "GlobalTriggerEffTool", m_globalTriggerEffTool );
  declareProperty( "GlobalTriggerEffToolLoose", m_globalTriggerEffToolLoose );

  m_muonTool = asg::AnaToolHandle<CP::IMuonTriggerScaleFactors>("CP::MuonTriggerScaleFactors/MuonTrigEff");
  m_muonToolLoose = asg::AnaToolHandle<CP::IMuonTriggerScaleFactors>("CP::MuonTriggerScaleFactors/MuonTrigEffLoose");

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
  std::map<std::string,std::string> legsPerKey;

  top::ConfigurationSettings* configSettings = top::ConfigurationSettings::get();
  std::string electron_triggers = configSettings->value("ElectronTriggers");
  std::string muon_triggers     = configSettings->value("MuonTriggers");
  std::vector<std::string> e_trigs, m_trigs;
  boost::split(e_trigs, electron_triggers, boost::is_any_of(" "));
  boost::split(m_trigs, muon_triggers,     boost::is_any_of(" "));

  std::map<std::string, std::vector<std::string> > electronTriggerCombination, muonTriggerCombination;
  for(auto& t : e_trigs){
    std::vector<std::string> tmp, tmp1;
    boost::split(tmp, t, boost::is_any_of("@"));
    boost::split(tmp1, tmp.at(1), boost::is_any_of(","));
    electronTriggerCombination[tmp.at(0)] = tmp1;
  }

  for(auto& t : m_trigs){
    std::vector<std::string> tmp, tmp1;
    boost::split(tmp, t, boost::is_any_of("@"));
    boost::split(tmp1, tmp.at(1), boost::is_any_of(","));
    muonTriggerCombination[tmp.at(0)] = tmp1;
  }

  // Get quality
  std::string electronID             = m_config->electronID();
  std::string electronIDLoose        = m_config->electronIDLoose();
  std::string electronIsolation      = m_config->electronIsolation();
  std::string electronIsolationLoose = m_config->electronIsolationLoose();
  std::string muonQuality            = m_config->muonQuality();
  std::string muonQualityLoose       = m_config->muonQualityLoose();
  // Tidy name for e/gamma
  electronID      = mapWorkingPoints(electronID);
  electronIDLoose = mapWorkingPoints(electronIDLoose);

  // Create electron trigger SF and Eff tools
  ToolHandleArray<IAsgElectronEfficiencyCorrectionTool> electronEffTools, electronSFTools, electronEffToolsLoose, electronSFToolsLoose;
  std::map<std::string,std::string> legsPerTool, legsPerToolLoose;
  int nTools = 0;
  // Loop over the triggers found (electrons)
  //  for(auto& trigKey : e_trigs){
  for(auto& y_t : electronTriggerCombination){
    std::string year = y_t.first;
    for(auto& trigKey : y_t.second){
      nTools++;
      for(int j=0;j<2;++j) // one tool instance for efficiencies, another for scale factors
	{
	  ATH_MSG_INFO("TIGHT " << year << " " << trigKey << " " << electronID << " " << electronIsolation );
	  auto t = m_electronToolsFactory.emplace(m_electronToolsFactory.end(), "AsgElectronEfficiencyCorrectionTool/ElTrigEff_"+std::to_string(j)+"_"+std::to_string(nTools));
	  top::check(t->setProperty("MapFilePath", "ElectronEfficiencyCorrection/2015_2016/rel20.7/Moriond_February2017_v3/map1.txt"), "Fail");
	  top::check(t->setProperty("TriggerKey", (j? year+"_"+trigKey : "Eff_"+year+"_"+trigKey)), "Failed to set TriggerKey");
	  if (electronID != "None")
	    top::check(t->setProperty("IdKey", electronID), "Failed to set IdKey");
	  if (electronIsolation != "None")
	    top::check(t->setProperty("IsoKey", electronIsolation), "Failed to set IsoKey");
	  top::check(t->setProperty("CorrelationModel","TOTAL"), "Failed to set CorrelationModel");
	  top::check(t->setProperty("ForceDataType", (int)PATCore::ParticleDataType::Full), "Failed to set ForceDataType");
	  top::check(t->setProperty("OutputLevel", MSG::INFO), "Failed to set OutputLevel");
	  top::check(t->initialize(), "Failed to initalise");
	  // Using syntax from examples
	  auto& handles = j? electronSFTools : electronEffTools;
	  handles.push_back(t->getHandle());
	  std::string name = handles[handles.size()-1].name();
	  legsPerTool[name] = trigKey;
	  ATH_MSG_INFO("TIGHT " << name << " -> " << trigKey);
	  
	  ATH_MSG_INFO("LOOSE " << year << " " << trigKey << " " << electronIDLoose << " " << electronIsolationLoose );
	  auto tLoose = m_electronToolsFactoryLoose.emplace(m_electronToolsFactoryLoose.end(), "AsgElectronEfficiencyCorrectionTool/ElTrigEffLoose_"+std::to_string(j)+"_"+std::to_string(nTools));
	  top::check(tLoose->setProperty("MapFilePath", "ElectronEfficiencyCorrection/2015_2016/rel20.7/Moriond_February2017_v3/map1.txt"), "Fail");
	  top::check(tLoose->setProperty("TriggerKey", (j? year+"_"+trigKey : "Eff_"+year+"_"+trigKey)),"Failed to set TriggerKey");
	  if (electronIDLoose != "None")
	    top::check(tLoose->setProperty("IdKey", electronIDLoose),"Failed to set IdKey");
	  if (electronIsolationLoose != "None")
	    top::check(tLoose->setProperty("IsoKey", electronIsolationLoose),"Failed to set IsoKey");
	  top::check(tLoose->setProperty("CorrelationModel","TOTAL"), "Failed to set CorrelationModel");
	  top::check(tLoose->setProperty("ForceDataType", (int)PATCore::ParticleDataType::Full), "Failed to set ForceDataType");
	  top::check(tLoose->setProperty("OutputLevel", MSG::INFO), "Failed to set OutputLevel");
	  top::check(tLoose->initialize(), "Failed to initalise");
	  // Using syntax from examples
	  auto& handlesLoose = j? electronSFToolsLoose : electronEffToolsLoose;
	  handlesLoose.push_back(tLoose->getHandle());
	  name = handlesLoose[handlesLoose.size()-1].name();
	  legsPerToolLoose[name] = trigKey;
          ATH_MSG_INFO("LOOSE " << name << " -> " << trigKey);
	  
	}
    }
  } 

  // Create muon trigger SF tool 
  ToolHandleArray<CP::IMuonTriggerScaleFactors> muonTools;
  ToolHandleArray<CP::IMuonTriggerScaleFactors> muonToolsLoose;
 
  if(muonQuality != "None")
    top::check(m_muonTool.setProperty("MuonQuality", muonQuality), "Failed to set MuonQuality");
  top::check(m_muonTool.setProperty("AllowZeroSF", true), "Failed to set AllowZeroSF"); 
  top::check(m_muonTool.initialize(), "Failed to initialise");
  ATH_MSG_INFO(muonTools.size());
  muonTools.push_back(m_muonTool.getHandle());
  ATH_MSG_INFO("Muon tool name (tight) " << muonTools[muonTools.size()-1].name());
  ATH_MSG_INFO(muonTools.size());
 
  if(muonQualityLoose != "None")
    top::check(m_muonToolLoose.setProperty("MuonQuality", muonQualityLoose), "Failed to set MuonQuality");
  top::check(m_muonToolLoose.setProperty("AllowZeroSF", true), "Failed to set AllowZeroSF");
  top::check(m_muonToolLoose.initialize(), "Failed to initialise");
  ATH_MSG_INFO(muonToolsLoose.size());
  muonToolsLoose.push_back(m_muonToolLoose.getHandle());
  ATH_MSG_INFO(muonToolsLoose.size());
  ATH_MSG_INFO("Muon tool name (loose) " << muonToolsLoose[muonToolsLoose.size()-1].name());
  
  // Construct the trigger combinations from  the electron and muon trigger combinations
  // Electron triggers
  for(auto& key : electronTriggerCombination){
    if(triggerCombination.find(key.first) == triggerCombination.end()){
      triggerCombination[key.first] = "";
    }
    else{
      triggerCombination[key.first] += " || ";
    }
    triggerCombination[key.first] += boost::algorithm::join(key.second, " || ");
  }
  // Muon triggers
  for(auto& key : muonTriggerCombination){
    if(triggerCombination.find(key.first) == triggerCombination.end()){
      triggerCombination[key.first] = "";
    }
    else{
      triggerCombination[key.first] += " || ";
    }
    triggerCombination[key.first] += boost::algorithm::join(key.second, " || ");
  }

  for(auto kv: triggerCombination) ATH_MSG_INFO("TRIG: " << kv.first << " -> " << kv.second);

  // Make the global trigger tool
  //asg::AnaToolHandle<ITrigGlobalEfficiencyCorrectionTool> globalTriggerEffTool("TrigGlobalEfficiencyCorrectionTool/TrigGlobal");
  TrigGlobalEfficiencyCorrectionTool* globalTriggerEffTool =  new TrigGlobalEfficiencyCorrectionTool("TrigGlobalEfficiencyCorrectionTool::TrigGlobal");
  top::check(globalTriggerEffTool->setProperty("ElectronEfficiencyTools", electronEffTools), "");
  top::check(globalTriggerEffTool->setProperty("ElectronScaleFactorTools", electronSFTools), "");
  top::check(globalTriggerEffTool->setProperty("MuonTools", muonTools), "");
  top::check(globalTriggerEffTool->setProperty("ListOfLegsPerTool", legsPerTool), "");
  top::check(globalTriggerEffTool->setProperty("TriggerCombination", triggerCombination), "");
  top::check(globalTriggerEffTool->setProperty("OutputLevel", MSG::DEBUG), "");
  top::check(globalTriggerEffTool->initialize(), "Failed to initalise");
  m_globalTriggerEffTool = globalTriggerEffTool;

  //asg::AnaToolHandle<ITrigGlobalEfficiencyCorrectionTool> globalTriggerEffToolLoose("TrigGlobalEfficiencyCorrectionTool/TrigGlobalLoose");
  TrigGlobalEfficiencyCorrectionTool* globalTriggerEffToolLoose =  new TrigGlobalEfficiencyCorrectionTool("TrigGlobalEfficiencyCorrectionTool::TrigGlobalLoose");
  top::check(globalTriggerEffToolLoose->setProperty("ElectronEfficiencyTools", electronEffToolsLoose), "");
  top::check(globalTriggerEffToolLoose->setProperty("ElectronScaleFactorTools", electronSFToolsLoose), "");
  top::check(globalTriggerEffToolLoose->setProperty("MuonTools", muonToolsLoose), "");
  top::check(globalTriggerEffToolLoose->setProperty("ListOfLegsPerTool", legsPerToolLoose), "");
  top::check(globalTriggerEffToolLoose->setProperty("TriggerCombination", triggerCombination), "");
  top::check(globalTriggerEffToolLoose->setProperty("OutputLevel", MSG::DEBUG), "");
  top::check(globalTriggerEffToolLoose->initialize(), "Failed to initalise");
  m_globalTriggerEffToolLoose = globalTriggerEffToolLoose;
  return StatusCode::SUCCESS;
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
