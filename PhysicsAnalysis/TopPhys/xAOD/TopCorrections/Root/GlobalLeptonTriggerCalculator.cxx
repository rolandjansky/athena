/*
Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TopCorrections/GlobalLeptonTriggerCalculator.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODEventInfo/EventInfo.h"
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"
#include "MuonAnalysisInterfaces/IMuonTriggerScaleFactors.h"

#include <vector>

namespace top{

  GlobalLeptonTriggerCalculator::GlobalLeptonTriggerCalculator( const std::string& name ) :
  asg::AsgTool( name ),
  m_config(nullptr),

  m_systNominal( CP::SystematicSet() ),

  m_globalTriggerSF("TrigGlobalEfficiencyCorrectionTool::TrigGlobal"),
  m_globalTriggerSFLoose("TrigGlobalEfficiencyCorrectionTool::TrigGlobalLoose"),

  m_decor_triggerSF("SetMe"),
  m_decor_triggerSF_loose("SetMe"),
  m_decor_triggerEffMC("SetMe"),  
  m_decor_triggerEffMC_loose("SetMe"),
  m_decor_triggerEffData("SetMe"),
  m_decor_triggerEffData_loose("SetMe"),

  m_selectedLepton("passPreORSelection"),
  m_selectedLeptonLoose("passPreORSelectionLoose")
  {
    declareProperty( "config" , m_config );
  }

  StatusCode GlobalLeptonTriggerCalculator::initialize()
  {
    ATH_MSG_INFO(" top::GlobalLeptonTriggerCalculator initialize" );

    top::check( m_globalTriggerSF.retrieve(),      "Failed to retrieve global trigger SF tool");
    top::check( m_globalTriggerSFLoose.retrieve(), "Failed to retrieve global trigger SF tool");
    
    m_decor_triggerSF            = "AnalysisTop_Trigger_SF";
    m_decor_triggerSF_loose      = "AnalysisTop_Trigger_SF_Loose";
    m_decor_triggerEffMC         = "AnalysisTop_Trigger_EffMC";
    m_decor_triggerEffMC_loose   = "AnalysisTop_Trigger_EffMC_Loose";
    m_decor_triggerEffData       = "AnalysisTop_Trigger_EffData";
    m_decor_triggerEffData_loose = "AnalysisTop_Trigger_EffMData_Loose";


    // Retrieve the systematic names we stored
    for(auto& s : m_config->getGlobalTriggerElectronSystematics()) ATH_MSG_INFO(" - Electron systematics : " << s);
    for(auto& s : m_config->getGlobalTriggerMuonSystematics())     ATH_MSG_INFO(" - Muon systematics : " << s);
    for(auto& s : m_config->getGlobalTriggerElectronTools())       ATH_MSG_INFO(" - Electron tools : " << s);
    for(auto& s : m_config->getGlobalTriggerMuonTools())           ATH_MSG_INFO(" - Muon tools : " << s);

    
    CP::SystematicSet sysSet = m_globalTriggerSF->affectingSystematics();
    std::set<std::string> base_names = sysSet.getBaseNames();

    return StatusCode::SUCCESS;
  }


  StatusCode GlobalLeptonTriggerCalculator::execute()
  {
    top::check(executeNominalVariations(),   "Failed to GlobalLeptonTriggerCalculator::executeNominalVariations()");;
    top::check(executeElectronSystematics(), "Failed to GlobalLeptonTriggerCalculator::executeElectronSystematics()");
    top::check(executeMuonSystematics(),     "Failed to GlobalLeptonTriggerCalculator::executeMuonSystematics()");
    return StatusCode::SUCCESS;
  }

  StatusCode GlobalLeptonTriggerCalculator::setElectronSystematic(std::string systematicName, int sig, std::vector<std::string> toolNames){
    // This function is to set the SystematicSet on the electron trigger tools before getting scale factors/ efficiencies
    ATH_MSG_DEBUG("Setting systematic variation " << systematicName );
    for(auto& s: toolNames){
      ToolHandle<IAsgElectronEfficiencyCorrectionTool> electronTool (s);
      top::check(electronTool.retrieve(), "Failed to retrieve (electron tool) "+s);
      CP::SystematicSet systSet;
      if(systematicName != ""){
	systSet.insert( CP::SystematicVariation(systematicName, sig) );
	m_activeSystVariation = systSet.name();
      }
      else{
	m_activeSystVariation = "";
      }
      
      top::check( electronTool->applySystematicVariation(systSet), "Failed to apply systematic "+systematicName );
    }
    return StatusCode::SUCCESS;   
  }

  StatusCode GlobalLeptonTriggerCalculator::setMuonSystematic(std::string systematicName, int sig, std::vector<std::string> toolNames){
    // This function is to set the SystematicSet on the electron trigger tools before getting scale factors/ efficiencies   
    ATH_MSG_DEBUG("Setting systematic variation " << systematicName );
    for(auto& s: toolNames){
      ToolHandle<CP::IMuonTriggerScaleFactors> muonTool (s);
      top::check(muonTool.retrieve(), "Failed to retrieve (muon tool) "+s);
      CP::SystematicSet systSet;
      if(systematicName != ""){
	systSet.insert( CP::SystematicVariation(systematicName, sig) );
	m_activeSystVariation = systSet.name();
      }
      else{
	m_activeSystVariation = "";
      }
      top::check( muonTool->applySystematicVariation(systSet), "Failed to apply systematic "+systematicName );
    }
    return StatusCode::SUCCESS;
  }

  StatusCode GlobalLeptonTriggerCalculator::executeNominalVariations()
  {
    // Retrieve nominal muons, retrieve nominal electrons
    const xAOD::MuonContainer* muons(nullptr);
    const xAOD::ElectronContainer* electrons(nullptr);
    std::vector<const xAOD::Muon*> selectedMuons;
    std::vector<const xAOD::Muon*> selectedMuonsLoose;
    std::vector<const xAOD::Electron*> selectedElectrons;
    std::vector<const xAOD::Electron*> selectedElectronsLoose;

    top::check(evtStore()->retrieve(muons, "Muons_"), "Failed to retrieve Nominal muons");
    // Put into a vector
    selectedMuons.clear();
    selectedMuonsLoose.clear();
    for(const auto muonPtr: *muons){
      if (m_selectedLepton.isAvailable(*muonPtr)){
        if (m_selectedLepton(*muonPtr) == 1) {
          selectedMuons.push_back(muonPtr);
        }
      }
      if (m_selectedLeptonLoose.isAvailable(*muonPtr)){
        if (m_selectedLeptonLoose(*muonPtr) == 1){
          selectedMuonsLoose.push_back(muonPtr);
        }
      }
    }

    top::check(evtStore()->retrieve(electrons, "Electrons_"), "Failed to retrieve Nominal muons");
    // Put into a vector
    selectedElectrons.clear();
    selectedElectronsLoose.clear();
    for(const auto electronPtr: *electrons){
      if (m_selectedLepton.isAvailable(*electronPtr)){
        if (m_selectedLepton(*electronPtr) == 1){
          selectedElectrons.push_back(electronPtr);
        }
      }
      if (m_selectedLeptonLoose.isAvailable(*electronPtr)){
        if (m_selectedLeptonLoose(*electronPtr) == 1){
          selectedElectronsLoose.push_back(electronPtr);
        }
      }
    }

    int nTightLeptons = selectedMuons.size()      + selectedElectrons.size();
    int nLooseLeptons = selectedMuonsLoose.size() + selectedElectronsLoose.size();

    double SF_Trigger(1.0), SF_TriggerLoose(1.0);
    double EFF_Trigger_MC(0.0), EFF_Trigger_DATA(0.0);
    double EFF_TriggerLoose_MC(0.0), EFF_TriggerLoose_DATA(0.0);   

    ///-- Set the nominal --///
    top::check( this->setElectronSystematic("", 1,  m_config->getGlobalTriggerElectronTools()), "Failed to apply electron nominal" );
    top::check( this->setMuonSystematic(    "", 1,  m_config->getGlobalTriggerMuonTools()), "Failed to apply muon nominal" );

    if(nTightLeptons >= 1){
      top::check(m_globalTriggerSF      -> getEfficiencyScaleFactor(selectedElectrons, selectedMuons, SF_Trigger  ) , "Failed to get SF (tight)");
      top::check(m_globalTriggerSF      -> getEfficiency(selectedElectrons, selectedMuons, EFF_Trigger_DATA, EFF_Trigger_MC), "Failed to get efficiency (tight)");
      Print("Tight "+m_activeSystVariation, SF_Trigger, EFF_Trigger_DATA, EFF_Trigger_MC);
    }

    if(nLooseLeptons >= 1){
      top::check(m_globalTriggerSFLoose -> getEfficiencyScaleFactor(selectedElectronsLoose, selectedMuonsLoose, SF_TriggerLoose  ) , "Failed to get SF");
      top::check(m_globalTriggerSFLoose -> getEfficiency(selectedElectronsLoose, selectedMuonsLoose, EFF_TriggerLoose_DATA, EFF_TriggerLoose_MC), "Failed to get efficiency (tight)");
      Print("Loose "+m_activeSystVariation, SF_Trigger,EFF_Trigger_DATA, EFF_Trigger_MC);
    }
    
    ///-- Apply and calculate variations on nominal --///
    if(nTightLeptons >= 1){
      ///-- Set nominal muon for electron systematics --///
      top::check( this->setMuonSystematic(    "", 1,  m_config->getGlobalTriggerMuonTools()), "Failed to apply muon nominal" );
      for(auto& s : m_config->getGlobalTriggerElectronSystematics()){
	top::check( this->setElectronSystematic(s, 1,  m_config->getGlobalTriggerElectronTools()), "Failed to apply electron up systematic" );
	top::check(m_globalTriggerSF      -> getEfficiencyScaleFactor(selectedElectrons, selectedMuons, SF_Trigger  ) , "Failed to get SF");
	top::check(m_globalTriggerSF      -> getEfficiency(selectedElectrons, selectedMuons, EFF_Trigger_DATA, EFF_Trigger_MC), "Failed to get efficiency (tight)");
	Print("Tight "+m_activeSystVariation, SF_Trigger,EFF_Trigger_DATA, EFF_Trigger_MC);

	top::check( this->setElectronSystematic(s, -1, m_config->getGlobalTriggerElectronTools()), "Failed to apply electron down systematic" );
	top::check(m_globalTriggerSF      -> getEfficiencyScaleFactor(selectedElectrons, selectedMuons, SF_Trigger  ) , "Failed to get SF");
	top::check(m_globalTriggerSF      -> getEfficiency(selectedElectrons, selectedMuons, EFF_Trigger_DATA, EFF_Trigger_MC), "Failed to get efficiency (tight)");
	Print("Tight "+m_activeSystVariation, SF_Trigger,EFF_Trigger_DATA, EFF_Trigger_MC);
      }
      ///-- Set nominal electron for muon systematics --///
      top::check( this->setElectronSystematic("", 1,  m_config->getGlobalTriggerElectronTools()), "Failed to apply electron nominal" );
      for(auto& s : m_config->getGlobalTriggerMuonSystematics()){
	top::check( this->setMuonSystematic(s, 1,  m_config->getGlobalTriggerMuonTools()), "Failed to apply muon up systematic" );
	top::check(m_globalTriggerSF      -> getEfficiencyScaleFactor(selectedElectrons, selectedMuons, SF_Trigger  ) , "Failed to get SF");
	top::check(m_globalTriggerSF      -> getEfficiency(selectedElectrons, selectedMuons, EFF_Trigger_DATA, EFF_Trigger_MC), "Failed to get efficiency (tight)");
	Print("Tight "+m_activeSystVariation, SF_Trigger,EFF_Trigger_DATA, EFF_Trigger_MC);

	top::check( this->setMuonSystematic(s, -1, m_config->getGlobalTriggerMuonTools()), "Failed to apply muon down systematic" );
	top::check(m_globalTriggerSF      -> getEfficiencyScaleFactor(selectedElectrons, selectedMuons, SF_Trigger  ) , "Failed to get SF");
	top::check(m_globalTriggerSF      -> getEfficiency(selectedElectrons, selectedMuons, EFF_Trigger_DATA, EFF_Trigger_MC), "Failed to get efficiency (tight)");
	Print("Tight "+m_activeSystVariation, SF_Trigger,EFF_Trigger_DATA, EFF_Trigger_MC);	
      }
        
    }
    
    ///-- Apply and calculate variations on nominal --///
    if(nLooseLeptons >= 1){
      ///-- Set nominal muon for electron systematic --///
      top::check( this->setMuonSystematic(    "", 1,  m_config->getGlobalTriggerMuonTools()), "Failed to apply muon nominal" );
      for(auto& s : m_config->getGlobalTriggerElectronSystematics()){
	top::check( this->setElectronSystematic(s, 1,  m_config->getGlobalTriggerElectronTools()), "Failed to apply electron up systematic" );
	top::check(m_globalTriggerSFLoose -> getEfficiencyScaleFactor(selectedElectronsLoose, selectedMuonsLoose, SF_TriggerLoose  ) , "Failed to get SF");
	top::check(m_globalTriggerSFLoose -> getEfficiency(selectedElectronsLoose, selectedMuonsLoose, EFF_TriggerLoose_DATA, EFF_TriggerLoose_MC), "Failed to get efficiency (tight)");	
	Print("Loose "+m_activeSystVariation, SF_TriggerLoose, EFF_TriggerLoose_DATA, EFF_TriggerLoose_MC);

	top::check( this->setElectronSystematic(s, -1, m_config->getGlobalTriggerElectronTools()), "Failed to apply electron down systematic" );
	top::check(m_globalTriggerSFLoose -> getEfficiencyScaleFactor(selectedElectronsLoose, selectedMuonsLoose, SF_TriggerLoose  ) , "Failed to get SF");
	top::check(m_globalTriggerSFLoose -> getEfficiency(selectedElectronsLoose, selectedMuonsLoose, EFF_TriggerLoose_DATA, EFF_TriggerLoose_MC), "Failed to get efficiency (tight)");
	Print("Loose "+m_activeSystVariation, SF_TriggerLoose,EFF_TriggerLoose_DATA, EFF_TriggerLoose_MC);
      }
      ///-- Set nominal electron for muon systematic --///
      top::check( this->setElectronSystematic("", 1,  m_config->getGlobalTriggerElectronTools()), "Failed to apply electron nominal" );
      for(auto& s : m_config->getGlobalTriggerMuonSystematics()){
	top::check( this->setMuonSystematic(s, 1,  m_config->getGlobalTriggerMuonTools()), "Failed to apply muon up systematic" );
	top::check(m_globalTriggerSFLoose -> getEfficiencyScaleFactor(selectedElectronsLoose, selectedMuonsLoose, SF_TriggerLoose  ) , "Failed to get SF");
	top::check(m_globalTriggerSFLoose -> getEfficiency(selectedElectronsLoose, selectedMuonsLoose, EFF_TriggerLoose_DATA, EFF_TriggerLoose_MC), "Failed to get efficiency (tight)");
	Print("Loose "+m_activeSystVariation, SF_TriggerLoose,EFF_TriggerLoose_DATA, EFF_TriggerLoose_MC);

	top::check( this->setMuonSystematic(s, -1, m_config->getGlobalTriggerMuonTools()), "Failed to apply muon down systematic" );
	top::check(m_globalTriggerSFLoose -> getEfficiencyScaleFactor(selectedElectronsLoose, selectedMuonsLoose, SF_TriggerLoose  ) , "Failed to get SF");
	top::check(m_globalTriggerSFLoose -> getEfficiency(selectedElectronsLoose, selectedMuonsLoose, EFF_TriggerLoose_DATA, EFF_TriggerLoose_MC), "Failed to get efficiency (tight)");
	Print("Loose "+m_activeSystVariation, SF_TriggerLoose,EFF_TriggerLoose_DATA, EFF_TriggerLoose_MC);
      }
    }
     
    return StatusCode::SUCCESS;
  }

  void GlobalLeptonTriggerCalculator::Print(std::string info, double SF, double EFF_MC, double EFF_DATA){
    ATH_MSG_INFO("Trig SF  ("+info+")  " << SF);
    ATH_MSG_INFO("Eff MC   ("+info+")  " << EFF_MC);
    ATH_MSG_INFO("Eff Data ("+info+")  " << EFF_DATA);
    return;
  }

  StatusCode GlobalLeptonTriggerCalculator::executeElectronSystematics()
  {
    // Retrieve nominal muons, for use with the systematic electrons
    const xAOD::MuonContainer* muons(nullptr);
    const xAOD::ElectronContainer* electrons(nullptr);
    std::vector<const xAOD::Muon*> selectedMuons;
    std::vector<const xAOD::Muon*> selectedMuonsLoose;
    std::vector<const xAOD::Electron*> selectedElectrons;
    std::vector<const xAOD::Electron*> selectedElectronsLoose;

    ///-- Nominal muons + systematic electrons
    top::check(evtStore()->retrieve(muons, "Muons_"), "Failed to retrieve Nominal muons");
    // Put into a vector
    selectedMuons.clear();
    selectedMuonsLoose.clear();

    for(const auto muonPtr: *muons){
      if (m_selectedLepton.isAvailable(*muonPtr)){
        if (m_selectedLepton(*muonPtr) == 1) {
          selectedMuons.push_back(muonPtr);
        }
      }
      if (m_selectedLeptonLoose.isAvailable(*muonPtr)){
        if (m_selectedLeptonLoose(*muonPtr) == 1){
          selectedMuonsLoose.push_back(muonPtr);
        }
      }
    }

    ///-- Set electron and muon tools to nominal calculation --///
    top::check( this->setElectronSystematic("", 1,  m_config->getGlobalTriggerElectronTools()), "Failed to apply electron nominal" );
    top::check( this->setMuonSystematic(    "", 1,  m_config->getGlobalTriggerMuonTools()), "Failed to apply muon nominal" );


    ///-- Loop over all electron collections --///
    for (auto currentSystematic : *m_config->systSgKeyMapElectrons()) {
      top::check(evtStore()->retrieve(electrons, currentSystematic.second), "failed to retrieve electrons");
      // Put into the vectors
      selectedElectrons.clear();
      selectedElectronsLoose.clear();

      for(const auto electronPtr: *electrons){
	if (m_selectedLepton.isAvailable(*electronPtr)){
	  if (m_selectedLepton(*electronPtr) == 1){
	    selectedElectrons.push_back(electronPtr);
	  }
	}
	if (m_selectedLeptonLoose.isAvailable(*electronPtr)){
	  if (m_selectedLeptonLoose(*electronPtr) == 1){
	    selectedElectronsLoose.push_back(electronPtr);
	  }
	}
      }

      int nTightLeptons = selectedMuons.size() + selectedElectrons.size();
      int nLooseLeptons = selectedMuonsLoose.size() + selectedElectronsLoose.size();

      if(nTightLeptons == 0 && nLooseLeptons == 0) continue;

      double SF_Trigger(1.0), SF_TriggerLoose(1.0);
      double EFF_Trigger_MC(0.0), EFF_Trigger_DATA(0.0);
      double EFF_TriggerLoose_MC(0.0), EFF_TriggerLoose_DATA(0.0);

      if(nTightLeptons >= 1){
	top::check(m_globalTriggerSF      -> getEfficiencyScaleFactor(selectedElectrons, selectedMuons, SF_Trigger  ) ,      "Failed to get SF");
	top::check(m_globalTriggerSF      -> getEfficiency(selectedElectrons,      selectedMuons,      EFF_Trigger_DATA,      EFF_Trigger_MC),      "");
      }
      if(nLooseLeptons >= 1){
	top::check(m_globalTriggerSFLoose -> getEfficiencyScaleFactor(selectedElectronsLoose, selectedMuonsLoose, SF_TriggerLoose  ) , "Failed to get SF");
	top::check(m_globalTriggerSFLoose -> getEfficiency(selectedElectronsLoose, selectedMuonsLoose, EFF_TriggerLoose_DATA, EFF_TriggerLoose_MC), "");
      }

    }

    return StatusCode::SUCCESS;
  }

  StatusCode GlobalLeptonTriggerCalculator::executeMuonSystematics()
  {
    // Retrieve nominal electrons, for use with the systematic muons
    const xAOD::MuonContainer* muons(nullptr);
    const xAOD::ElectronContainer* electrons(nullptr);
    std::vector<const xAOD::Muon*> selectedMuons;
    std::vector<const xAOD::Muon*> selectedMuonsLoose;
    std::vector<const xAOD::Electron*> selectedElectrons;
    std::vector<const xAOD::Electron*> selectedElectronsLoose;

    ///--- Nominal electrons + systematics muons
    top::check(evtStore()->retrieve(electrons, "Electrons_"), "Failed to retrieve Nominal electrons");
    // Put into a vector
    selectedElectrons.clear();
    selectedElectronsLoose.clear();

    for(const auto electronPtr: *electrons){
      if (m_selectedLepton.isAvailable(*electronPtr)){
	if (m_selectedLepton(*electronPtr) == 1){
	  selectedElectrons.push_back(electronPtr);
	}
      }
      if (m_selectedLeptonLoose.isAvailable(*electronPtr)){
	if (m_selectedLeptonLoose(*electronPtr) == 1){
	  selectedElectronsLoose.push_back(electronPtr);
	}
      }
    }

    ///-- Set electron and muon tools to nominal calculation --///                                                                   
    top::check( this->setElectronSystematic("", 1,  m_config->getGlobalTriggerElectronTools()), "Failed to apply electron nominal" );
    top::check( this->setMuonSystematic(    "", 1,  m_config->getGlobalTriggerMuonTools()), "Failed to apply muon nominal" );

    ///-- Loop over all muon collections --///
    for (auto currentSystematic : *m_config->systSgKeyMapMuons()) {
      top::check(evtStore()->retrieve(muons, currentSystematic.second), "failed to retrieve muons");
      // Put into the vectors
      selectedMuons.clear();
      selectedMuonsLoose.clear();

      for(const auto muonPtr: *muons){
	if (m_selectedLepton.isAvailable(*muonPtr)){
	  if (m_selectedLepton(*muonPtr) == 1) {
	    selectedMuons.push_back(muonPtr);
	  }
	}
	if (m_selectedLeptonLoose.isAvailable(*muonPtr)){
	  if (m_selectedLeptonLoose(*muonPtr) == 1){
	    selectedMuonsLoose.push_back(muonPtr);
	  }
	}
      }

      int nTightLeptons = selectedMuons.size()      + selectedElectrons.size();
      int nLooseLeptons = selectedMuonsLoose.size() + selectedElectronsLoose.size();

      if(nTightLeptons == 0 && nLooseLeptons == 0) continue;

      double SF_Trigger(1.0), SF_TriggerLoose(1.0);
      double EFF_Trigger_MC(1.0), EFF_Trigger_DATA(1.0);
      double EFF_TriggerLoose_MC(1.0), EFF_TriggerLoose_DATA(1.0);
      
      if(nTightLeptons >= 1){
	top::check(m_globalTriggerSF      -> getEfficiencyScaleFactor(selectedElectrons, selectedMuons, SF_Trigger  ) ,      "Failed to get SF");
	top::check(m_globalTriggerSF      -> getEfficiency(selectedElectrons,      selectedMuons,      EFF_Trigger_DATA,      EFF_Trigger_MC),      "");
      }
      if(nLooseLeptons >= 1){
	top::check(m_globalTriggerSFLoose -> getEfficiencyScaleFactor(selectedElectronsLoose, selectedMuonsLoose, SF_TriggerLoose  ) , "Failed to get SF");
	top::check(m_globalTriggerSFLoose -> getEfficiency(selectedElectronsLoose, selectedMuonsLoose, EFF_TriggerLoose_DATA, EFF_TriggerLoose_MC), "");
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode GlobalLeptonTriggerCalculator::decorateEventInfo(std::string decor, double value){
    
    return StatusCode::SUCCESS;
  }


}
