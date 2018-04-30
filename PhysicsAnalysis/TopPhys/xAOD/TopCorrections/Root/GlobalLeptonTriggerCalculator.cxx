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

#include <vector>

namespace top{

  GlobalLeptonTriggerCalculator::GlobalLeptonTriggerCalculator( const std::string& name ) :
  asg::AsgTool( name ),
  m_config(nullptr),

  m_systNominal( CP::SystematicSet() ),
  m_systTrigger_ELECTRON_UP( CP::SystematicSet() ),
  m_systTrigger_ELECTRON_DOWN( CP::SystematicSet() ),
  m_systTrigger_MUON_STAT_UP( CP::SystematicSet() ),
  m_systTrigger_MUON_STAT_DOWN( CP::SystematicSet() ),
  m_systTrigger_MUON_SYST_UP( CP::SystematicSet() ),
  m_systTrigger_MUON_SYST_DOWN( CP::SystematicSet() ),

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
    
    m_systTrigger_ELECTRON_UP.insert(    CP::SystematicVariation("EL_EFF_Trigger_TOTAL_1NPCOR_PLUS_UNCOR" ,  1 ));
    m_systTrigger_ELECTRON_DOWN.insert(  CP::SystematicVariation("EL_EFF_Trigger_TOTAL_1NPCOR_PLUS_UNCOR" , -1 ));
    m_systTrigger_MUON_STAT_UP.insert(   CP::SystematicVariation("MUON_EFF_TrigStatUncertainty__1up",        1 ));
    m_systTrigger_MUON_STAT_DOWN.insert( CP::SystematicVariation("MUON_EFF_TrigStatUncertainty__1down",     -1 ));
    m_systTrigger_MUON_SYST_UP.insert(   CP::SystematicVariation("MUON_EFF_TrigSystUncertainty__1up",        1 ));
    m_systTrigger_MUON_SYST_DOWN.insert( CP::SystematicVariation("MUON_EFF_TrigSystUncertainty__1down",     -1 ));

    m_decor_triggerSF            = "AnalysisTop_Trigger_SF";
    m_decor_triggerSF_loose      = "AnalysisTop_Trigger_SF_Loose";
    m_decor_triggerEffMC         = "AnalysisTop_Trigger_EffMC";
    m_decor_triggerEffMC_loose   = "AnalysisTop_Trigger_EffMC_Loose";
    m_decor_triggerEffData       = "AnalysisTop_Trigger_EffData";
    m_decor_triggerEffData_loose = "AnalysisTop_Trigger_EffMData_Loose";


    // Retrieve the systematic names we stored
    for(auto& s : m_config->getGlobalTriggerElectronSystematics()) ATH_MSG_INFO("Electron systematics : " << s);
    for(auto& s : m_config->getGlobalTriggerMuonSystematics()) ATH_MSG_INFO("Muon systematics : " << s);
    for(auto& s : m_config->getGlobalTriggerElectronTools()) ATH_MSG_INFO("Electron tools : " << s);
    for(auto& s : m_config->getGlobalTriggerMuonTools()) ATH_MSG_INFO("Muon tools : " << s);

    
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

    if(nTightLeptons >= 1){
      ///-- Tell the SF tools to use the nominal systematic --///
      top::check(m_globalTriggerSF      -> applySystematicVariation( m_systNominal ),"Failed to set systematic");
      top::check(m_globalTriggerSF      -> getEfficiencyScaleFactor(selectedElectrons, selectedMuons, SF_Trigger  ) , "Failed to get SF (tight)");
      top::check(m_globalTriggerSF      -> getEfficiency(selectedElectrons, selectedMuons, EFF_Trigger_DATA, EFF_Trigger_MC), "Failed to get efficiency (tight)");
        
      ///-- Tell the SF tools to use electron systematic --///
      top::check(m_globalTriggerSF      -> applySystematicVariation( m_systTrigger_ELECTRON_UP ),"Failed to set systematic");
      top::check(m_globalTriggerSF      -> getEfficiencyScaleFactor(selectedElectrons, selectedMuons, SF_Trigger  ) , "Failed to get SF");
      top::check(m_globalTriggerSF      -> getEfficiency(selectedElectrons, selectedMuons, EFF_Trigger_DATA, EFF_Trigger_MC), "Failed to get efficiency (tight)");
      
      ///-- Tell the SF tools to use the electron systematic --///
      top::check(m_globalTriggerSF      -> applySystematicVariation( m_systTrigger_ELECTRON_DOWN ),"Failed to set systematic");
      top::check(m_globalTriggerSF      -> getEfficiencyScaleFactor(selectedElectrons, selectedMuons, SF_Trigger  ) , "Failed to get SF");
      top::check(m_globalTriggerSF      -> getEfficiency(selectedElectrons, selectedMuons, EFF_Trigger_DATA, EFF_Trigger_MC), "Failed to get efficiency (tight)");

      ///-- Tell the SF tools to use the muon systematic --///
      top::check(m_globalTriggerSF      -> applySystematicVariation( m_systTrigger_MUON_STAT_UP ),"Failed to set systematic");
      top::check(m_globalTriggerSF      -> getEfficiencyScaleFactor(selectedElectrons, selectedMuons, SF_Trigger  ) , "Failed to get SF");
      top::check(m_globalTriggerSF      -> getEfficiency(selectedElectrons, selectedMuons, EFF_Trigger_DATA, EFF_Trigger_MC), "Failed to get efficiency (tight)");
      
      ///-- Tell the SF tools to use the muon systematic --/// 
      top::check(m_globalTriggerSF      -> applySystematicVariation( m_systTrigger_MUON_STAT_DOWN ),"Failed to set systematic");
      top::check(m_globalTriggerSF      -> getEfficiencyScaleFactor(selectedElectrons, selectedMuons, SF_Trigger  ) , "Failed to get SF");
      top::check(m_globalTriggerSF      -> getEfficiency(selectedElectrons, selectedMuons, EFF_Trigger_DATA, EFF_Trigger_MC), "Failed to get efficiency (tight)");
      
      ///-- Tell the SF tools to use the muon systematic --/// 
      top::check(m_globalTriggerSF      -> applySystematicVariation( m_systTrigger_MUON_SYST_UP ),"Failed to set systematic");
      top::check(m_globalTriggerSF      -> getEfficiencyScaleFactor(selectedElectrons, selectedMuons, SF_Trigger  ) , "Failed to get SF");
      top::check(m_globalTriggerSF      -> getEfficiency(selectedElectrons, selectedMuons, EFF_Trigger_DATA, EFF_Trigger_MC), "Failed to get efficiency (tight)");
                                    
      ///-- Tell the SF tools to use the muon systematic --/// 
      top::check(m_globalTriggerSF      -> applySystematicVariation( m_systTrigger_MUON_SYST_DOWN ),"Failed to set systematic");
      top::check(m_globalTriggerSF      -> getEfficiencyScaleFactor(selectedElectrons, selectedMuons, SF_Trigger  ) , "Failed to get SF");
      top::check(m_globalTriggerSF      -> getEfficiency(selectedElectrons, selectedMuons, EFF_Trigger_DATA, EFF_Trigger_MC), "Failed to get efficiency (tight)");
    }

    if(nLooseLeptons >= 1){
      ///-- Tell the SF tools to use the nominal systematic --///
      top::check(m_globalTriggerSFLoose -> applySystematicVariation( m_systNominal ),"Failed to set systematic");
      top::check(m_globalTriggerSFLoose -> getEfficiencyScaleFactor(selectedElectronsLoose, selectedMuonsLoose, SF_TriggerLoose  ) , "Failed to get SF");
      top::check(m_globalTriggerSFLoose -> getEfficiency(selectedElectronsLoose, selectedMuonsLoose, EFF_TriggerLoose_DATA, EFF_TriggerLoose_MC), "Failed to get efficiency (tight)");

      ///-- Tell the SF tools to use electron systematic --/// 
      top::check(m_globalTriggerSFLoose -> applySystematicVariation( m_systTrigger_ELECTRON_UP ),"Failed to set systematic");
      top::check(m_globalTriggerSFLoose -> getEfficiencyScaleFactor(selectedElectronsLoose, selectedMuonsLoose, SF_TriggerLoose  ) , "Failed to get SF");
      top::check(m_globalTriggerSFLoose -> getEfficiency(selectedElectronsLoose, selectedMuonsLoose, EFF_TriggerLoose_DATA, EFF_TriggerLoose_MC), "Failed to get efficiency (tight)");

      ///-- Tell the SF tools to use the electron systematic --///
      top::check(m_globalTriggerSFLoose -> applySystematicVariation( m_systTrigger_ELECTRON_DOWN ),"Failed to set systematic");
      top::check(m_globalTriggerSFLoose -> getEfficiencyScaleFactor(selectedElectronsLoose, selectedMuonsLoose, SF_TriggerLoose  ) , "Failed to get SF");
      top::check(m_globalTriggerSFLoose -> getEfficiency(selectedElectronsLoose, selectedMuonsLoose, EFF_TriggerLoose_DATA, EFF_TriggerLoose_MC), "Failed to get efficiency (tight)");

      ///-- Tell the SF tools to use the muon systematic --/// 
      top::check(m_globalTriggerSFLoose -> applySystematicVariation( m_systTrigger_MUON_STAT_UP ),"Failed to set systematic");
      top::check(m_globalTriggerSFLoose -> getEfficiencyScaleFactor(selectedElectronsLoose, selectedMuonsLoose, SF_TriggerLoose  ) , "Failed to get SF");
      top::check(m_globalTriggerSFLoose -> getEfficiency(selectedElectronsLoose, selectedMuonsLoose, EFF_TriggerLoose_DATA, EFF_TriggerLoose_MC), "Failed to get efficiency (tight)");

      ///-- Tell the SF tools to use the muon systematic --/// 
      top::check(m_globalTriggerSFLoose -> applySystematicVariation( m_systTrigger_MUON_STAT_DOWN ),"Failed to set systematic");
      top::check(m_globalTriggerSFLoose -> getEfficiencyScaleFactor(selectedElectronsLoose, selectedMuonsLoose, SF_TriggerLoose  ) , "Failed to get SF");
      top::check(m_globalTriggerSFLoose -> getEfficiency(selectedElectronsLoose, selectedMuonsLoose, EFF_TriggerLoose_DATA, EFF_TriggerLoose_MC), "Failed to get efficiency (tight)");

      ///-- Tell the SF tools to use the muon systematic --/// 
      top::check(m_globalTriggerSFLoose -> applySystematicVariation( m_systTrigger_MUON_SYST_UP ),"Failed to set systematic");
      top::check(m_globalTriggerSFLoose -> getEfficiencyScaleFactor(selectedElectronsLoose, selectedMuonsLoose, SF_TriggerLoose  ) , "Failed to get SF");
      top::check(m_globalTriggerSFLoose -> getEfficiency(selectedElectronsLoose, selectedMuonsLoose, EFF_TriggerLoose_DATA, EFF_TriggerLoose_MC), "Failed to get efficiency (tight)");

      ///-- Tell the SF tools to use the muon systematic --/// 
      top::check(m_globalTriggerSFLoose -> applySystematicVariation( m_systTrigger_MUON_SYST_DOWN ),"Failed to set systematic");
      top::check(m_globalTriggerSFLoose -> getEfficiencyScaleFactor(selectedElectronsLoose, selectedMuonsLoose, SF_TriggerLoose  ) , "Failed to get SF");
      top::check(m_globalTriggerSFLoose -> getEfficiency(selectedElectronsLoose, selectedMuonsLoose, EFF_TriggerLoose_DATA, EFF_TriggerLoose_MC), "Failed to get efficiency (tight)");
    }
     
    return StatusCode::SUCCESS;
  }

  void GlobalLeptonTriggerCalculator::Print(double SF, double EFF_MC, double EFF_DATA){
    ATH_MSG_INFO("Trig SF  (Tight)  " << SF);
    ATH_MSG_INFO("Eff MC   (Tight)  " << EFF_MC);
    ATH_MSG_INFO("Eff Data (Tight)  " << EFF_DATA);
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

      ///-- Tell the SF tools to use the nominal systematic --///
      top::check(m_globalTriggerSF      -> applySystematicVariation( m_systNominal ),"Failed to set systematic");
      top::check(m_globalTriggerSFLoose -> applySystematicVariation( m_systNominal ),"Failed to set systematic");

      ///-- Get scale factor --///
      top::check(m_globalTriggerSF      -> getEfficiencyScaleFactor(selectedElectrons, selectedMuons, SF_Trigger  ) ,      "Failed to get SF");
      top::check(m_globalTriggerSFLoose -> getEfficiencyScaleFactor(selectedElectronsLoose, selectedMuonsLoose, SF_TriggerLoose  ) , "Failed to get SF");

      ///-- Get Scale Factors --///
      top::check(m_globalTriggerSF      -> getEfficiency(selectedElectrons,      selectedMuons,      EFF_Trigger_DATA,      EFF_Trigger_MC),      "");
      top::check(m_globalTriggerSFLoose -> getEfficiency(selectedElectronsLoose, selectedMuonsLoose, EFF_TriggerLoose_DATA, EFF_TriggerLoose_MC), "");

      ATH_MSG_INFO("Trig SF  (Tight) " << currentSystematic.second << " : " << SF_Trigger);
      ATH_MSG_INFO("Trig SF  (Loose) " << currentSystematic.second << " : " << SF_TriggerLoose);
      ATH_MSG_INFO("Eff MC   (Tight) " << currentSystematic.second << " : " << EFF_Trigger_MC);
      ATH_MSG_INFO("Eff MC   (Loose) " << currentSystematic.second << " : " << EFF_TriggerLoose_MC);
      ATH_MSG_INFO("Eff Data (Tight) " << currentSystematic.second << " : " << EFF_Trigger_DATA);
      ATH_MSG_INFO("Eff Data (Loose) " << currentSystematic.second << " : " << EFF_TriggerLoose_DATA);

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
      ///-- Tell the SF tools to use the nominal systematic --///
      top::check(m_globalTriggerSF      -> applySystematicVariation( m_systNominal ),"Failed to set systematic");
      top::check(m_globalTriggerSFLoose -> applySystematicVariation( m_systNominal ),"Failed to set systematic");
      ///-- Get scale factor --///
      top::check(m_globalTriggerSF      -> getEfficiencyScaleFactor(selectedElectrons, selectedMuons, SF_Trigger  ) ,      "Failed to get SF");
      top::check(m_globalTriggerSFLoose -> getEfficiencyScaleFactor(selectedElectronsLoose, selectedMuonsLoose, SF_TriggerLoose  ) , "Failed to get SF");
      ///-- Get Scale Factors --///
      top::check(m_globalTriggerSF      -> getEfficiency(selectedElectrons,      selectedMuons,      EFF_Trigger_DATA,      EFF_Trigger_MC),      "");
      top::check(m_globalTriggerSFLoose -> getEfficiency(selectedElectronsLoose, selectedMuonsLoose, EFF_TriggerLoose_DATA, EFF_TriggerLoose_MC), "");
      ATH_MSG_INFO("Trig SF  (Tight) " << currentSystematic.second << " : " << SF_Trigger);
      ATH_MSG_INFO("Trig SF  (Loose) " << currentSystematic.second << " : " << SF_TriggerLoose);
      ATH_MSG_INFO("Eff MC   (Tight) " << currentSystematic.second << " : " << EFF_Trigger_MC);
      ATH_MSG_INFO("Eff MC   (Loose) " << currentSystematic.second << " : " << EFF_TriggerLoose_MC);
      ATH_MSG_INFO("Eff Data (Tight) " << currentSystematic.second << " : " << EFF_Trigger_DATA);
      ATH_MSG_INFO("Eff Data (Loose) " << currentSystematic.second << " : " << EFF_TriggerLoose_DATA);
    }

    return StatusCode::SUCCESS;
  }

  StatusCode GlobalLeptonTriggerCalculator::decorateEventInfo(std::string decor, double value){
    
    return StatusCode::SUCCESS;
  }


}
