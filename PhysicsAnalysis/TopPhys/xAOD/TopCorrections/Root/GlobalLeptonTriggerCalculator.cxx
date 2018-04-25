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
    m_systTrigger_UP( CP::SystematicSet() ),
    m_systTrigger_DOWN( CP::SystematicSet() ),

    m_globalTriggerSF("TrigGlobalEfficiencyCorrectionTool::TrigGlobal"),
    m_globalTriggerSFLoose("TrigGlobalEfficiencyCorrectionTool::TrigGlobalLoose"),

    m_decor_triggerEff("SetMe"),
    m_decor_triggerEff_loose("SetMe"),
    m_decor_triggerSF("SetMe"),
    m_decor_triggerSF_loose("SetMe")
  {
    declareProperty( "config" , m_config );
  }

  StatusCode GlobalLeptonTriggerCalculator::initialize()
  {
    ATH_MSG_INFO(" top::GlobalLeptonTriggerCalculator initialize" );

    top::check( m_globalTriggerSF.retrieve(), "Failed to retrieve global trigger SF tool");
    top::check( m_globalTriggerSFLoose.retrieve(), "Failed to retrieve global trigger SF tool");

    m_systTrigger_UP.insert( CP::SystematicVariation("EL_EFF_Trigger_TOTAL_1NPCOR_PLUS_UNCOR" , 1 ));
    m_systTrigger_DOWN.insert( CP::SystematicVariation("EL_EFF_Trigger_TOTAL_1NPCOR_PLUS_UNCOR" , -1 ));

    m_decor_triggerEff       = "EFF_Trigger";
    m_decor_triggerEff_loose = "EFF_Trigger";
    m_decor_triggerSF        = "SF_Trigger";
    m_decor_triggerSF_loose  = "SF_Trigger";

    return StatusCode::SUCCESS;
  }


  StatusCode GlobalLeptonTriggerCalculator::execute()
  {
    // Retrieve nominal muons, for use with the systematic electrons
    const xAOD::MuonContainer* muons(nullptr);
    const xAOD::ElectronContainer* electrons(nullptr);
    std::vector<const xAOD::Muon*> selectedMuons;
    std::vector<const xAOD::Muon*> selectedMuonsLoose;
    std::vector<const xAOD::Electron*> selectedElectrons;
    std::vector<const xAOD::Electron*> selectedElectronsLoose;
    for (auto currentSystematic : *m_config->systSgKeyMapElectrons()) {
      ATH_MSG_INFO(currentSystematic.second);
    }

    top::check(evtStore()->retrieve(muons, "Muons_"), "Failed to retrieve Nominal muons");
    // Put into a vector
    selectedMuons.clear();
    selectedMuonsLoose.clear();
    for(const auto muonPtr: *muons){
      if (muonPtr->isAvailable<char>("passPreORSelection")) {
	if (muonPtr->auxdataConst<char>("passPreORSelection") == 1) {
	  selectedMuons.push_back(muonPtr);
	}
      }
      if (muonPtr->isAvailable<char>("passPreORSelectionLoose")) {
	if (muonPtr->auxdataConst<char>("passPreORSelectionLoose") == 1) {
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
        if (electronPtr->isAvailable<char>("passPreORSelection")) {
          if (electronPtr->auxdataConst<char>("passPreORSelection") == 1) {
	    selectedElectrons.push_back(electronPtr);
          }
        }
        if (electronPtr->isAvailable<char>("passPreORSelectionLoose")) {
          if (electronPtr->auxdataConst<char>("passPreORSelectionLoose") == 1) {
	    selectedElectronsLoose.push_back(electronPtr);
          }
        }
      }
      

      double SF_Trigger(1.0), SF_TriggerLoose(1.0);
      double EFF_Trigger_MC(1.0), EFF_Trigger_DATA(1.0);
      double EFF_TriggerLoose_MC(1.0), EFF_TriggerLoose_DATA(1.0);
      ///-- Tell the SF tools to use the nominal systematic --///
      top::check(m_globalTriggerSF      -> applySystematicVariation( m_systNominal ),"Failed to set systematic");
      top::check(m_globalTriggerSFLoose -> applySystematicVariation( m_systNominal ),"Failed to set systematic");

      ///-- Get scale factor --///
      top::check(m_globalTriggerSF      -> getEfficiencyScaleFactor(selectedElectrons, selectedMuons, SF_Trigger  ) ,      "Failed to get SF");
      top::check(m_globalTriggerSFLoose -> getEfficiencyScaleFactor(selectedElectrons, selectedMuons, SF_TriggerLoose  ) , "Failed to get SF");
      
      ///-- Get Scale Factors --///
      top::check(m_globalTriggerSF      -> getEfficiency(selectedElectrons, selectedMuons, EFF_Trigger_DATA, EFF_Trigger_MC), "");
      top::check(m_globalTriggerSFLoose -> getEfficiency(selectedElectrons, selectedMuons, EFF_Trigger_DATA, EFF_Trigger_MC), "");
      
      ATH_MSG_INFO("Trig SF  (Tight) " << currentSystematic.second << " : " << SF_Trigger);
      ATH_MSG_INFO("Trig SF  (Loose) " << currentSystematic.second << " : " << SF_TriggerLoose);
      ATH_MSG_INFO("Eff MC   (Tight) " << currentSystematic.second << " : " << EFF_Trigger_MC);
      ATH_MSG_INFO("Eff MC   (Loose) " << currentSystematic.second << " : " << EFF_TriggerLoose_MC);
      ATH_MSG_INFO("Eff Data (Tight) " << currentSystematic.second << " : " << EFF_Trigger_DATA);
      ATH_MSG_INFO("Eff Data (Loose) " << currentSystematic.second << " : " << EFF_TriggerLoose_DATA);

    }
    
    top::check(evtStore()->retrieve(electrons, "Electrons_"), "Failed to retrieve Nominal electrons");

    
    return StatusCode::SUCCESS;
  }
  
}
