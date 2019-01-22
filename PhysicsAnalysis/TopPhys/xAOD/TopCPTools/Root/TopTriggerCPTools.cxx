/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

  m_muonTool      = asg::AnaToolHandle<CP::IMuonTriggerScaleFactors>("CP::MuonTriggerScaleFactors/MuonTrigEff");
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
      if(m_config->useGlobalTrigger()){
	top::check( this->initialiseGlobalTriggerEff(),
		    "Failed to construct global trigger efficiencies" );
      }
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
  
  // Get trigger strings from configuration
  std::map<std::string,std::string> triggerCombination, triggerCombinationLoose;
  std::vector<std::string> electronSystematics, muonSystematics, electronToolNames, muonToolNames;
  std::unordered_map<std::string, std::vector<std::string>> const emptymap;
  std::unordered_map<std::string, std::vector<std::string>> const &
       electronTriggerCombination = (m_config->doTightEvents() ? m_config->getGlobalTriggerElectronTriggers() : emptymap),
       muonTriggerCombination = (m_config->doTightEvents() ? m_config->getGlobalTriggerMuonTriggers() : emptymap),
       electronTriggerCombinationLoose = (m_config->doLooseEvents() ? m_config->getGlobalTriggerElectronTriggersLoose() : emptymap),
       muonTriggerCombinationLoose = (m_config->doLooseEvents() ? m_config->getGlobalTriggerMuonTriggersLoose() : emptymap);

  // Get quality
  std::string electronID, electronIDLoose, electronIsolation, electronIsolationLoose, muonQuality, muonQualityLoose;
  if(m_config->doTightEvents()){
    electronID             = m_config->electronID();
    electronIsolation      = m_config->electronIsolationSF();
    muonQuality            = m_config->muonQuality();
  }
  if(m_config->doLooseEvents()){
    electronIDLoose        = m_config->electronIDLoose();
    electronIsolationLoose = m_config->electronIsolationSFLoose();
    muonQualityLoose       = m_config->muonQualityLoose();
  }
  
  // Tidy name for e/gamma
  electronID      = mapWorkingPoints(electronID);
  electronIDLoose = mapWorkingPoints(electronIDLoose);

  // Create electron trigger SF and Eff tools
  ToolHandleArray<IAsgElectronEfficiencyCorrectionTool> electronEffTools, electronSFTools, electronEffToolsLoose, electronSFToolsLoose;
  std::map<std::string,std::string> legsPerTool, legsPerToolLoose;
  int nTools = 0;

  // Loop over the triggers found (electrons - tight)
  for(auto& y_t : electronTriggerCombination){
    std::string year = y_t.first;
    for(auto& trigKey : y_t.second){
      nTools++;
      for(int j=0;j<2;++j) // one tool instance for efficiencies, another for scale factors
	{
	  ATH_MSG_INFO("TIGHT " << year << " " << trigKey << " " << electronID << " " << electronIsolation );
	  auto t = m_electronToolsFactory.emplace(m_electronToolsFactory.end(), "AsgElectronEfficiencyCorrectionTool/ElTrigEff_"+std::to_string(j)+"_"+std::to_string(nTools));
	  top::check(t->setProperty("MapFilePath", "ElectronEfficiencyCorrection/2015_2017/rel21.2/Consolidation_September2018_v1/map3.txt"), "Fail");
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
	  legsPerTool[name] = trigKey + " [" + year + "]";
	  ATH_MSG_INFO("TIGHT " << name << " -> " << trigKey);
	  electronToolNames.push_back(name);
	  // Special - Record the systematic names from the efficiency tool (not SF tool)
	  if(electronSystematics.size() == 0 && j == 1){
	    for(auto& s : handles[handles.size()-1]->recommendedSystematics().getBaseNames() ) electronSystematics.push_back(s);
	  }
	}
    }
  }

  // Loop over the triggers found (electrons - loose) 
  nTools = 0;
  for(auto& y_t : electronTriggerCombinationLoose){
    std::string year = y_t.first;
    for(auto& trigKey : y_t.second){
      nTools++;
      for(int j=0;j<2;++j) // one tool instance for efficiencies, another for scale factors
	{	  
	  ATH_MSG_INFO("LOOSE " << year << " " << trigKey << " " << electronIDLoose << " " << electronIsolationLoose );
	  auto tLoose = m_electronToolsFactoryLoose.emplace(m_electronToolsFactoryLoose.end(), "AsgElectronEfficiencyCorrectionTool/ElTrigEffLoose_"+std::to_string(j)+"_"+std::to_string(nTools));
	  top::check(tLoose->setProperty("MapFilePath", "ElectronEfficiencyCorrection/2015_2017/rel21.2/Consolidation_September2018_v1/map3.txt"), "Fail");
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
	  std::string name = handlesLoose[handlesLoose.size()-1].name();
	  legsPerToolLoose[name] = trigKey + " [" + year + "]";
          ATH_MSG_INFO("LOOSE " << name << " -> " << trigKey);
	  electronToolNames.push_back(name);
	  // Special - Record the systematic names from the efficiency tool (not SF tool)                                       
	  if(electronSystematics.size() == 0 && j == 1){
	    for(auto& s : handlesLoose[handlesLoose.size()-1]->recommendedSystematics().getBaseNames() ) electronSystematics.push_back(s);
	  }	  
	}
    }
  }

  // Create muon trigger SF tool 
  ToolHandleArray<CP::IMuonTriggerScaleFactors> muonTools;
  ToolHandleArray<CP::IMuonTriggerScaleFactors> muonToolsLoose;
 
  if(m_config->doTightEvents()){
    if(muonQuality != "None")
      top::check(m_muonTool.setProperty("MuonQuality", muonQuality), "Failed to set MuonQuality");
    top::check(m_muonTool.setProperty("AllowZeroSF", true), "Failed to set AllowZeroSF"); 
    top::check(m_muonTool.initialize(), "Failed to initialise");
    muonTools.push_back(m_muonTool.getHandle());
    ATH_MSG_INFO("Muon tool name (tight) " << muonTools[muonTools.size()-1].name());
    muonToolNames.push_back(muonTools[muonTools.size()-1].name());
    // Special - Get muon systematics
    if(muonSystematics.size() == 0){
      for(auto& s: muonTools[muonTools.size()-1]->recommendedSystematics().getBaseNames()) muonSystematics.push_back(s);
    }
  }
 
  if(m_config->doLooseEvents()){
    if(muonQualityLoose != "None")
      top::check(m_muonToolLoose.setProperty("MuonQuality", muonQualityLoose), "Failed to set MuonQuality");
    top::check(m_muonToolLoose.setProperty("AllowZeroSF", true), "Failed to set AllowZeroSF");
    top::check(m_muonToolLoose.initialize(), "Failed to initialise");
    muonToolsLoose.push_back(m_muonToolLoose.getHandle());
    ATH_MSG_INFO("Muon tool name (loose) " << muonToolsLoose[muonToolsLoose.size()-1].name());
    muonToolNames.push_back(muonToolsLoose[muonToolsLoose.size()-1].name());
    // Special - Get muon systematics                                                                                   
    if(muonSystematics.size() == 0){
      for(auto& s: muonToolsLoose[muonToolsLoose.size()-1]->recommendedSystematics().getBaseNames()) muonSystematics.push_back(s);
    }
  }

  // Construct the trigger combinations from  the electron and muon trigger combinations
  // Electron triggers (tight)
  for(auto& key : electronTriggerCombination){
    if(triggerCombination.find(key.first) == triggerCombination.end()){
      triggerCombination[key.first] = "";
    }
    else{
      triggerCombination[key.first] += " || ";
    }
    triggerCombination[key.first] += boost::algorithm::join(key.second, " || ");
  }
  // Muon triggers (tight)
  for(auto& key : muonTriggerCombination){
    if(triggerCombination.find(key.first) == triggerCombination.end()){
      triggerCombination[key.first] = "";
    }
    else{
      triggerCombination[key.first] += " || ";
    }
    triggerCombination[key.first] += boost::algorithm::join(key.second, " || ");
  }

  // Electron triggers (loose)                                                       
  for(auto& key : electronTriggerCombinationLoose){
    if(triggerCombinationLoose.find(key.first) == triggerCombinationLoose.end()){
      triggerCombinationLoose[key.first] = "";
    }
    else{
      triggerCombinationLoose[key.first] += " || ";
    }
    triggerCombinationLoose[key.first] += boost::algorithm::join(key.second, " || ");
  }
  // Muon triggers (loose)                                                   
  for(auto& key : muonTriggerCombinationLoose){
    if(triggerCombinationLoose.find(key.first) == triggerCombinationLoose.end()){
      triggerCombinationLoose[key.first] = "";
    }
    else{
      triggerCombinationLoose[key.first] += " || ";
    }
    triggerCombinationLoose[key.first] += boost::algorithm::join(key.second, " || ");
  }

  // Print out what we configured
  for(auto kv: triggerCombination)      ATH_MSG_DEBUG("TRIG (TIGHT): " << kv.first << " -> " << kv.second);
  for(auto kv: triggerCombinationLoose) ATH_MSG_DEBUG("TRIG (LOOSE): " << kv.first << " -> " << kv.second);

  // Make the global trigger tool
  if(m_config->doTightEvents()){
    TrigGlobalEfficiencyCorrectionTool* globalTriggerEffTool =  new TrigGlobalEfficiencyCorrectionTool("TrigGlobalEfficiencyCorrectionTool::TrigGlobal");
    top::check(globalTriggerEffTool->setProperty("ElectronEfficiencyTools", electronEffTools), "Failed to attach electron efficiency tools");
    top::check(globalTriggerEffTool->setProperty("ElectronScaleFactorTools", electronSFTools), "Failed to attach electron scale factor tools");
    top::check(globalTriggerEffTool->setProperty("MuonTools", muonTools), "Failed to attach muon tools");
    top::check(globalTriggerEffTool->setProperty("ListOfLegsPerTool", legsPerTool), "Failed to define list of legs per tool");
    top::check(globalTriggerEffTool->setProperty("TriggerCombination", triggerCombination), "Failed to define trigger combination");
    top::check(globalTriggerEffTool->setProperty("TriggerMatchingTool", m_trigMatchTool), "Failed to set TriggerMatchingTool");
    // Setting MSG::ERROR to avoid flooding output with invalid efficiency warnings before event selection is complete
    top::check(globalTriggerEffTool->setProperty("OutputLevel", MSG::ERROR), "Failed to set message level");
    top::check(globalTriggerEffTool->initialize(), "Failed to initalise");
    m_globalTriggerEffTool = globalTriggerEffTool;
  }
  if(m_config->doLooseEvents()){
    TrigGlobalEfficiencyCorrectionTool* globalTriggerEffToolLoose =  new TrigGlobalEfficiencyCorrectionTool("TrigGlobalEfficiencyCorrectionTool::TrigGlobalLoose");
    top::check(globalTriggerEffToolLoose->setProperty("ElectronEfficiencyTools", electronEffToolsLoose), "Failed to attach electron efficiency tools");
    top::check(globalTriggerEffToolLoose->setProperty("ElectronScaleFactorTools", electronSFToolsLoose), "Failed to attach electron scale factor tools");
    top::check(globalTriggerEffToolLoose->setProperty("MuonTools", muonToolsLoose), "Failed to attach muon tools");
    top::check(globalTriggerEffToolLoose->setProperty("ListOfLegsPerTool", legsPerToolLoose), "Failed to define list of legs per tool");
    top::check(globalTriggerEffToolLoose->setProperty("TriggerCombination", triggerCombinationLoose), "Failed to define trigger combination");
    top::check(globalTriggerEffToolLoose->setProperty("TriggerMatchingTool", m_trigMatchTool), "Failed to set TriggerMatchingTool");
    // Setting MSG::ERROR to avoid flooding output with invalid efficiency warnings before event selection is complete
    top::check(globalTriggerEffToolLoose->setProperty("OutputLevel", MSG::ERROR), "Failed to set message level");
    top::check(globalTriggerEffToolLoose->initialize(), "Failed to initalise");
    m_globalTriggerEffToolLoose = globalTriggerEffToolLoose;
  }

  // Set information about systematics inside TopConfig
  m_config->setGlobalTriggerConfiguration(electronSystematics, muonSystematics, electronToolNames, muonToolNames);

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
