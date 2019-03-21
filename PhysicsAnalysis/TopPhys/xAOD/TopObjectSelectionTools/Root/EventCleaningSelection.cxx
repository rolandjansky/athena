/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TopObjectSelectionTools/EventCleaningSelection.h"
#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "TrigGlobalEfficiencyCorrection/ImportData.h"


#include <algorithm>
#include <sstream>
#include <list>
#include <boost/algorithm/string.hpp>

namespace top {

  EventCleaningSelection::EventCleaningSelection( const std::string& name ) :
    asg::AsgTool( name ),
    m_config(nullptr),

    m_grlTool("GoodRunsListSelectionTool"),

    m_trigConfTool("TrigConf::xAODConfigTool"),
    m_trigDecisionTool("Trig::TrigDecisionTool"),
    m_trigMatchTool("Trig::MatchingTool"),
    m_trigMatchTauTool("Trig::TrigTauMatchingTool"),
    m_globalTriggerSF("TrigGlobalEfficiencyCorrectionTool::TrigGlobal"),
    m_globalTriggerSFLoose("TrigGlobalEfficiencyCorrectionTool::TrigGlobalLoose"),

    m_vetoEventsTrigger(false),
    m_vetoEventsGRL(false),
    m_vetoEventsGoodCalo(false),
    m_vetoEventsPriVtx(false) {
    declareProperty( "config" , m_config );

    declareProperty( "GRLTool" , m_grlTool );

    declareProperty( "TrigConfigTool", m_trigConfTool );
    declareProperty( "TrigDecisionTool", m_trigDecisionTool );
    declareProperty( "TrigMatchTool", m_trigMatchTool );
    declareProperty( "TrigMatchTauTool" , m_trigMatchTauTool );
  }

  StatusCode EventCleaningSelection::initialize()
  {
    // If running on a Truth DxAOD we don't need anything so
    // just return successfully...
    if (m_config->isTruthDxAOD()) return StatusCode::SUCCESS;

    top::check( m_trigConfTool.retrieve() , "Failed to retrieve TrigConfTool" );
    top::check( m_trigDecisionTool.retrieve() , "Failed to retrieve TrigDecisionTool" );
    top::check( m_trigMatchTool.retrieve(),
                "Failed to retrieve trigger matching tool");
    if (m_config->useTaus())
      top::check(m_trigMatchTauTool.retrieve(),
                 "Failed to retrieve TrigMatchTauTool" );

    if (!m_config->isMC()) {
      top::check( m_grlTool.retrieve() , "Failed to retrieve TrigDecisionTool" );
    }

    return StatusCode::SUCCESS;
  }

  void EventCleaningSelection::setEventSelections( const std::vector<top::SelectionConfigurationData>& selections )
  {
    std::unordered_set<std::string> tmpAllTriggers_Tight;
    std::unordered_set<std::string> tmpAllTriggers_Loose;

    // get full list of global triggers
    std::vector<std::string> globalTriggers_Tight;
    std::vector<std::string> globalTriggers_Loose;
    std::vector<std::string> globalElectronTriggers_Tight;
    std::vector<std::string> globalElectronTriggers_Loose;
    std::vector<std::string> globalMuonTriggers_Tight;
    std::vector<std::string> globalMuonTriggers_Loose;
    if (m_config->useGlobalTrigger()) {
      std::set<std::string> tmp;
      for (auto const & triggermap : {m_config->getGlobalTriggers()}) {
        for (auto const & pair : triggermap) {
          auto const & triggers = getIndividualFromGlobalTriggers(pair.second);
          tmp.insert(triggers.begin(), triggers.end());
        }
      }
      globalTriggers_Tight.assign(tmp.begin(), tmp.end());
      for (std::string const & trigger : globalTriggers_Tight) {
        if (isElectronTrigger(trigger))
          globalElectronTriggers_Tight.push_back(trigger);
        if (isMuonTrigger(trigger))
          globalMuonTriggers_Tight.push_back(trigger);
      }
      tmp.clear();
      // and the usual copy-paste-s/Tight/Loose/g story:
      for (auto const & triggermap : {m_config->getGlobalTriggersLoose()}) {
        for (auto const & pair : triggermap) {
          auto const & triggers = getIndividualFromGlobalTriggers(pair.second);
          tmp.insert(triggers.begin(), triggers.end());
        }
      }
      globalTriggers_Loose.assign(tmp.begin(), tmp.end());
      for (std::string const & trigger : globalTriggers_Loose) {
        if (isElectronTrigger(trigger))
          globalElectronTriggers_Loose.push_back(trigger);
        if (isMuonTrigger(trigger))
          globalMuonTriggers_Loose.push_back(trigger);
      }
    }

    m_allTriggers_Tight.clear();
    m_electronTriggers_Tight.clear();
    m_muonTriggers_Tight.clear();
    m_tauTriggers_Tight.clear();
    m_allTriggers_Loose.clear();
    m_electronTriggers_Loose.clear();
    m_muonTriggers_Loose.clear();
    m_tauTriggers_Loose.clear();

    // Trigger maps for TopConfig - to be used by individual selectors
    std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> allTriggers_perSelector_Tight
      ( new std::unordered_map<std::string,std::vector<std::string>> );
    std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> electronTriggers_perSelector_Tight
      ( new std::unordered_map<std::string,std::vector<std::string>> );
    std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> muonTriggers_perSelector_Tight
      ( new std::unordered_map<std::string,std::vector<std::string>> );
    std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> tauTriggers_perSelector_Tight
      ( new std::unordered_map<std::string,std::vector<std::string>> );
    std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> allTriggers_perSelector_Loose
      ( new std::unordered_map<std::string,std::vector<std::string>> );
    std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> electronTriggers_perSelector_Loose
      ( new std::unordered_map<std::string,std::vector<std::string>> );
    std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> muonTriggers_perSelector_Loose
      ( new std::unordered_map<std::string,std::vector<std::string>> );
    std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> tauTriggers_perSelector_Loose
      ( new std::unordered_map<std::string,std::vector<std::string>> );


    // Loop over all selections
    m_vetoEventsTrigger = true;
    m_vetoEventsGRL = true;
    m_vetoEventsGoodCalo = true;
    m_vetoEventsPriVtx = true;

    for (auto sel : selections) {

      std::list<std::string> listAllTriggers_thisSelector_Tight;
      std::vector<std::string> allTriggers_thisSelector_Tight;
      std::vector<std::string> electronTriggers_thisSelector_Tight;
      std::vector<std::string> muonTriggers_thisSelector_Tight;
      std::vector<std::string> tauTriggers_thisSelector_Tight;
      std::list<std::string> listAllTriggers_thisSelector_Loose;
      std::vector<std::string> allTriggers_thisSelector_Loose;
      std::vector<std::string> electronTriggers_thisSelector_Loose;
      std::vector<std::string> muonTriggers_thisSelector_Loose;
      std::vector<std::string> tauTriggers_thisSelector_Loose;

      // Loop over cut names and look for TRIGDEC, GRL, GOODCALO, PRIVTX
      bool selectionHasTriggerCut(false);
      bool selectionHasTriggerCut_Tight(false);
      bool selectionHasTriggerCut_Loose(false);
      bool selectionHasGRLCut(false);
      bool selectionHasGOODCALOCut(false);
      bool selectionHasPRIVTXCut(false);
      for (std::string cut : sel.m_cutnames) {
        using boost::algorithm::starts_with;
        cut.append(" ");

        if (starts_with(cut, "GRL ")) {
          selectionHasGRLCut = true;
        }

        if (starts_with(cut, "GOODCALO ")) {
          selectionHasGOODCALOCut = true;
        }

        if (starts_with(cut, "PRIVTX ")) {
          selectionHasPRIVTXCut = true;
        }

        if (starts_with(cut, "GTRIGDEC ")) {
          if (selectionHasTriggerCut || selectionHasTriggerCut_Loose || selectionHasTriggerCut_Tight) {
            ATH_MSG_ERROR("A *TRIGDEC* selector has already been used for selection "<<sel.m_name<<" - you can't have two.");
            ATH_MSG_ERROR("Exiting...");
            exit(1);
          }
          if (!m_config->useGlobalTrigger()) {
            ATH_MSG_ERROR("The GTRIGDEC selector cannot be used without UseGlobalLeptonTriggerSF.");
            ATH_MSG_ERROR("Exiting...");
            exit(1);
          }
          selectionHasTriggerCut = true;
          allTriggers_perSelector_Tight->insert(std::make_pair(sel.m_name,
              std::vector<std::string>(globalTriggers_Tight.begin(), globalTriggers_Tight.end())));
          allTriggers_perSelector_Loose->insert(std::make_pair(sel.m_name,
              std::vector<std::string>(globalTriggers_Loose.begin(), globalTriggers_Loose.end())));
          electronTriggers_perSelector_Tight->insert(std::make_pair(sel.m_name,
              std::vector<std::string>(globalElectronTriggers_Tight.begin(), globalElectronTriggers_Tight.end())));
          electronTriggers_perSelector_Loose->insert(std::make_pair(sel.m_name,
              std::vector<std::string>(globalElectronTriggers_Loose.begin(), globalElectronTriggers_Loose.end())));
          muonTriggers_perSelector_Tight->insert(std::make_pair(sel.m_name,
              std::vector<std::string>(globalMuonTriggers_Tight.begin(), globalMuonTriggers_Tight.end())));
          muonTriggers_perSelector_Loose->insert(std::make_pair(sel.m_name,
              std::vector<std::string>(globalMuonTriggers_Loose.begin(), globalMuonTriggers_Loose.end())));
        }

        if (starts_with(cut, "TRIGDEC_TIGHT ")) {
          if (selectionHasTriggerCut_Tight) {
            ATH_MSG_ERROR("TRIGDEC_TIGHT has already been used for selection "<<sel.m_name<<" - you can't use it twice.");
            ATH_MSG_ERROR("Exiting...");
            exit(1);
          }
          if (selectionHasTriggerCut) {
            ATH_MSG_ERROR("TRIGDEC_TIGHT is used but TRIGDEC is also been used for selection"<<sel.m_name<<" - you can't use both at the same time.");
            ATH_MSG_ERROR("Exiting...");
            exit(1);
          }
          selectionHasTriggerCut_Tight = true;
          ATH_MSG_INFO("Tight Triggers for Selection \t"<<sel.m_name<<"\tare "<<cut);

          //split the trigger string at spaces
          std::stringstream ss(cut);
          std::string item;
          char delim = ' ';
          while (std::getline(ss, item, delim)) {
            if (item.size() > 0 && item.find("TRIGDEC_TIGHT") == std::string::npos) {
              tmpAllTriggers_Tight.insert(item);
              listAllTriggers_thisSelector_Tight.push_back(item);
            }
          }
          listAllTriggers_thisSelector_Tight.sort();
          listAllTriggers_thisSelector_Tight.unique();

          // Turn list into vector
          for (auto trigger : listAllTriggers_thisSelector_Tight) {
            allTriggers_thisSelector_Tight.push_back( trigger );
          }

          // Split triggers into electron, muon and tau
          for (const auto& trigger : allTriggers_thisSelector_Tight) {
            if (isElectronTrigger(trigger)) {
              electronTriggers_thisSelector_Tight.push_back(trigger);
            }
            if (isMuonTrigger(trigger)) {
              muonTriggers_thisSelector_Tight.push_back(trigger);
            }
            if ( (trigger.find("_tau") != std::string::npos) ) {
              tauTriggers_thisSelector_Tight.push_back(trigger);
            }
          }

          allTriggers_perSelector_Tight->insert( std::make_pair( sel.m_name , allTriggers_thisSelector_Tight ) );
          electronTriggers_perSelector_Tight->insert( std::make_pair( sel.m_name , electronTriggers_thisSelector_Tight ) );
          muonTriggers_perSelector_Tight->insert( std::make_pair( sel.m_name , muonTriggers_thisSelector_Tight ) );
          tauTriggers_perSelector_Tight->insert( std::make_pair( sel.m_name , tauTriggers_thisSelector_Tight ) );
        } // Cut requested is TRIGDEC_TIGHT
        else if (starts_with(cut, "TRIGDEC_LOOSE ")) {
          if (selectionHasTriggerCut_Loose) {
            ATH_MSG_ERROR("TRIGDEC_LOOSE has already been used for selection "<<sel.m_name<<" - you can't use it twice.");
            ATH_MSG_ERROR("Exiting...");
            exit(1);
          }
          if (selectionHasTriggerCut) {
            ATH_MSG_ERROR("TRIGDEC_LOOSE is used but TRIGDEC is also been used for selection"<<sel.m_name<<" - you can't use both at the same time.");
            ATH_MSG_ERROR("Exiting...");
            exit(1);
          }
          selectionHasTriggerCut_Loose = true;
          ATH_MSG_INFO("Loose Triggers for Selection \t"<<sel.m_name<<"\tare "<<cut);

          //split the trigger string at spaces
          std::stringstream ss(cut);
          std::string item;
          char delim = ' ';
          while (std::getline(ss, item, delim)) {
            if (item.size() > 0 && item.find("TRIGDEC_LOOSE") == std::string::npos) {
              tmpAllTriggers_Loose.insert(item);
              listAllTriggers_thisSelector_Loose.push_back(item);
            }
          }
          listAllTriggers_thisSelector_Loose.sort();
          listAllTriggers_thisSelector_Loose.unique();

          // Turn list into vector
          for (auto trigger : listAllTriggers_thisSelector_Loose) {
            allTriggers_thisSelector_Loose.push_back( trigger );
          }

          // Split triggers into electron, muon and tau
          for (const auto& trigger : allTriggers_thisSelector_Loose) {
            if (isElectronTrigger(trigger)) {
              electronTriggers_thisSelector_Loose.push_back(trigger);
            }
            if (isMuonTrigger(trigger)) {
              muonTriggers_thisSelector_Loose.push_back(trigger);
            }
            if ( (trigger.find("_tau") != std::string::npos) ) {
              tauTriggers_thisSelector_Loose.push_back(trigger);
            }
          }

          allTriggers_perSelector_Loose->insert( std::make_pair( sel.m_name , allTriggers_thisSelector_Loose ) );
          electronTriggers_perSelector_Loose->insert( std::make_pair( sel.m_name , electronTriggers_thisSelector_Loose ) );
          muonTriggers_perSelector_Loose->insert( std::make_pair( sel.m_name , muonTriggers_thisSelector_Loose ) );
          tauTriggers_perSelector_Loose->insert( std::make_pair( sel.m_name , tauTriggers_thisSelector_Loose ) );
        } // Cut requested is TRIGDEC_LOOSE
        else if (starts_with(cut, "TRIGDEC ")) {
          if (selectionHasTriggerCut) {
            ATH_MSG_ERROR("TRIGDEC/GTRIGDEC has already been used for selection "<<sel.m_name<<" - you can't use it twice.");
            ATH_MSG_ERROR("Exiting...");
            exit(1);
          }
          if (selectionHasTriggerCut_Tight) {
            ATH_MSG_ERROR("TRIGDEC is used but TRIGDEC_TIGHT is also been used for selection"<<sel.m_name<<" - you can't use both at the same time.");
            ATH_MSG_ERROR("Exiting...");
            exit(1);
          }
          if (selectionHasTriggerCut_Loose) {
            ATH_MSG_ERROR("TRIGDEC is used but TRIGDEC_LOOSE is also been used for selection"<<sel.m_name<<" - you can't use both at the same time.");
            ATH_MSG_ERROR("Exiting...");
            exit(1);
          }
          selectionHasTriggerCut = true;
          ATH_MSG_INFO("Triggers for Selection \t"<<sel.m_name<<"\tare "<<cut);

          //split the trigger string at spaces
          std::stringstream ss(cut);
          std::string item;
          char delim = ' ';
          while (std::getline(ss, item, delim)) {
            if (item.size() > 0 && item.find("TRIGDEC") == std::string::npos) {
              tmpAllTriggers_Tight.insert(item);
              listAllTriggers_thisSelector_Tight.push_back(item);
              tmpAllTriggers_Loose.insert(item);
              listAllTriggers_thisSelector_Loose.push_back(item);
            }
          }
          listAllTriggers_thisSelector_Tight.sort();
          listAllTriggers_thisSelector_Tight.unique();
          listAllTriggers_thisSelector_Loose.sort();
          listAllTriggers_thisSelector_Loose.unique();

          // Turn list into vector
          for (auto trigger : listAllTriggers_thisSelector_Tight) {
            allTriggers_thisSelector_Tight.push_back( trigger );
           }
          for (auto trigger : listAllTriggers_thisSelector_Loose) {
            allTriggers_thisSelector_Loose.push_back( trigger );
          }
          // Split triggers into electron, muon and tau
          for (const auto& trigger : allTriggers_thisSelector_Tight) {
            if (isElectronTrigger(trigger)) {
              electronTriggers_thisSelector_Tight.push_back(trigger);
            }
            if (isMuonTrigger(trigger)) {
              muonTriggers_thisSelector_Tight.push_back(trigger);
            }
            if ( (trigger.find("_tau") != std::string::npos) ) {
              tauTriggers_thisSelector_Tight.push_back(trigger);
            }
          }

          for (const auto& trigger : allTriggers_thisSelector_Loose) {
            if (isElectronTrigger(trigger)) {
              electronTriggers_thisSelector_Loose.push_back(trigger);
            }
            if (isMuonTrigger(trigger)) {
              muonTriggers_thisSelector_Loose.push_back(trigger);
            }
            if ( (trigger.find("_tau") != std::string::npos) ) {
              tauTriggers_thisSelector_Loose.push_back(trigger);
            }
          }

          allTriggers_perSelector_Tight->insert( std::make_pair( sel.m_name , allTriggers_thisSelector_Tight ) );
          electronTriggers_perSelector_Tight->insert( std::make_pair( sel.m_name , electronTriggers_thisSelector_Tight ) );
          muonTriggers_perSelector_Tight->insert( std::make_pair( sel.m_name , muonTriggers_thisSelector_Tight ) );
          tauTriggers_perSelector_Tight->insert( std::make_pair( sel.m_name , tauTriggers_thisSelector_Tight ) );

          allTriggers_perSelector_Loose->insert( std::make_pair( sel.m_name , allTriggers_thisSelector_Loose ) );
          electronTriggers_perSelector_Loose->insert( std::make_pair( sel.m_name , electronTriggers_thisSelector_Loose ) );
          muonTriggers_perSelector_Loose->insert( std::make_pair( sel.m_name , muonTriggers_thisSelector_Loose ) );
          tauTriggers_perSelector_Loose->insert( std::make_pair( sel.m_name , tauTriggers_thisSelector_Loose ) );

        } // Cut requested is TRIGDEC

      } // Loop over all cuts

      if (!selectionHasGRLCut) {
        m_vetoEventsGRL = false;
      }

      if (!selectionHasGOODCALOCut) {
        m_vetoEventsGoodCalo = false;
      }

      if (!selectionHasPRIVTXCut) {
        m_vetoEventsPriVtx = false;
      }

      if (!selectionHasTriggerCut) {
        m_vetoEventsTrigger = false;
        ATH_MSG_INFO("Selection "<<sel.m_name<<" Does not request a trigger. No event veto will be applied");
      }
    } // Loop over all selections

    // Add triggers configured for the global trigger SF tool
    tmpAllTriggers_Tight.insert(globalTriggers_Tight.begin(), globalTriggers_Tight.end());
    tmpAllTriggers_Loose.insert(globalTriggers_Loose.begin(), globalTriggers_Loose.end());

    // Turn list into vector
    {
      std::vector<std::string> tmp;
      tmp.assign(tmpAllTriggers_Tight.begin(), tmpAllTriggers_Tight.end());
      std::sort(tmp.begin(), tmp.end());
      m_allTriggers_Tight.swap(tmp);
      tmp.clear();
      tmp.assign(tmpAllTriggers_Loose.begin(), tmpAllTriggers_Loose.end());
      std::sort(tmp.begin(), tmp.end());
      m_allTriggers_Loose.swap(tmp);
    }

    ATH_MSG_INFO("All requested Tight triggers are:");
    for (const auto& trigger : m_allTriggers_Tight) {
       ATH_MSG_INFO("  "<<trigger);
     }
    ATH_MSG_INFO("All requested Loose triggers are:");
    for (const auto& trigger : m_allTriggers_Loose) {
      ATH_MSG_INFO("  "<<trigger);
    }

    std::string outputInfoString("True");
    if (!m_vetoEventsTrigger) {
      outputInfoString = "False";
    }
    ATH_MSG_INFO("Apply event veto on trigger decision = "<<outputInfoString);

    // Split triggers into electron, muon and tau
    for (const auto& trigger : m_allTriggers_Tight) {
      if (isElectronTrigger(trigger)) {
        m_electronTriggers_Tight.push_back(trigger);
      }
      if (isMuonTrigger(trigger)) {
        m_muonTriggers_Tight.push_back(trigger);
      }
      if ( (trigger.find("_tau") != std::string::npos) ) {
        m_tauTriggers_Tight.push_back(trigger);
      }
    }
    // Split triggers into electron, muon and tau
    for (const auto& trigger : m_allTriggers_Loose) {
      if (isElectronTrigger(trigger)) {
        m_electronTriggers_Loose.push_back(trigger);
      }
      if (isMuonTrigger(trigger)) {
        m_muonTriggers_Loose.push_back(trigger);
      }
      if ( (trigger.find("_tau") != std::string::npos) ) {
        m_tauTriggers_Loose.push_back(trigger);
      }
    }

    // Tell TopConfig about the triggers
    m_config->allTriggers_Tight( allTriggers_perSelector_Tight );
    m_config->electronTriggers_Tight( electronTriggers_perSelector_Tight );
    m_config->muonTriggers_Tight( muonTriggers_perSelector_Tight );
    m_config->tauTriggers_Tight( tauTriggers_perSelector_Tight );
    m_config->allTriggers_Loose( allTriggers_perSelector_Loose );
    m_config->electronTriggers_Loose( electronTriggers_perSelector_Loose );
    m_config->muonTriggers_Loose( muonTriggers_perSelector_Loose );
    m_config->tauTriggers_Loose( tauTriggers_perSelector_Loose );

    // If the user has requested that all events are saved, then we'd better turn off the vetos
    if (!m_config->saveOnlySelectedEvents()) {
      m_vetoEventsGRL = false;
      m_vetoEventsGoodCalo = false;
      m_vetoEventsTrigger = false;
    }

  }

  bool EventCleaningSelection::applyGRL() const
  {
    const xAOD::EventInfo* eventInfo(nullptr);
    top::check(evtStore()->retrieve(eventInfo,m_config->sgKeyEventInfo()),"Failed to retrieve EventInfo");

    ///-- Apply GRL to data --///
    char passGRL(1);
    if (!m_config->isMC() && asg::ToolStore::contains<IGoodRunsListSelectionTool> ("GoodRunsListSelectionTool")) {
      if (!m_grlTool->passRunLB( *eventInfo )) {
        passGRL = 0;
      }
    }
    eventInfo->auxdecor<char>("AnalysisTop_GRL") = passGRL;  

    // Do we veto events? Only if ALL selectors request GRL and GRL fails
    if (m_vetoEventsGRL) {
      if (passGRL == 1) {
        return true;
      }
      if (passGRL == 0) {
        return false;
      }
    }

    // otherwise, we don't veto the event
    return true;
  }

  bool EventCleaningSelection::applyGoodCalo() const
  {
    const xAOD::EventInfo* eventInfo(nullptr);
    top::check(evtStore()->retrieve(eventInfo,m_config->sgKeyEventInfo()),"Failed to retrieve EventInfo");

    ///-- Apply GoodCalo tests to data --///
    ///-- https://twiki.cern.ch/twiki/bin/view/AtlasProtected/PhysicsAnalysisWorkBookRel20CPRec --///
    char passGoodCalo(1);
    if (!m_config->isMC()) {
      bool badTile(false), badLAr(false), badSCT(false), incompleteEvent(false);

      ///-- Remove bad events due to problems in TileCal --///
      if (eventInfo->errorState(xAOD::EventInfo::Tile)==xAOD::EventInfo::Error) {
        badTile = true;
      }

      ///-- Remove bad events due to problems in LAr --///
      if (eventInfo->errorState(xAOD::EventInfo::LAr)==xAOD::EventInfo::Error) {
        badLAr = true;
      }

      // <tom.neep@cern.ch> Technically not the Calo, but this is easier :)
      if (eventInfo->errorState(xAOD::EventInfo::SCT) == xAOD::EventInfo::Error) {
        badSCT = true;
      }

      // <tom.neep@cern.ch> Technically not the Calo (volume 2), but this is easier :)
      if (eventInfo->isEventFlagBitSet(xAOD::EventInfo::Core, 18)) {
        incompleteEvent = true;
      }

      if (badTile || badLAr || badSCT || incompleteEvent) {
        passGoodCalo = 0;
      }
    }
    eventInfo->auxdecor<char>("AnalysisTop_GOODCALO") = passGoodCalo;  

    // Do we veto events? Only if ALL selectors request GOODCALO and GOODCALO fails
    if (m_vetoEventsGoodCalo) {
      if (passGoodCalo == 1) {
        return true;
      }  
      if (passGoodCalo == 0) {
        return false;
      }
    }

    // otherwise, we don't veto the event
    return true;
  }

  bool EventCleaningSelection::applyPrimaryVertex() const
  {
    ///-- https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/InDetTrackingPerformanceGuidelines#Vertexing --///
    const xAOD::EventInfo* eventInfo(nullptr);
    top::check(evtStore()->retrieve(eventInfo,m_config->sgKeyEventInfo()),"Failed to retrieve EventInfo");

    const xAOD::VertexContainer* vertices(nullptr);
    top::check(evtStore()->retrieve(vertices,m_config->sgKeyPrimaryVertices()), "Failed to retrieve Primary Vertices");

    char passPriVtx(0);
    float pri_vtx_z(0);

    for (auto vtx : *vertices) {
      if (vtx->vertexType() == xAOD::VxType::PriVtx) {
        passPriVtx = 1;
	pri_vtx_z = vtx->z();
        break; // No need to loop over all vertices, we only need one
      }
    }
    eventInfo->auxdecor<char>("AnalysisTop_PRIVTX") = passPriVtx;
    if( passPriVtx == 1 )
      eventInfo->auxdecor<float>("AnalysisTop_PRIVTX_z_position") = pri_vtx_z;

    // Do we veto events? Only if ALL selectors request PRIVTX and PRIVTX fails
    if (m_vetoEventsPriVtx) {
      if (passPriVtx == 1) {
        return true;
      }
      if (passPriVtx == 0) {
        return false;
      }
    }

    // otherwise, we don't veto the event
    return true;
  }

  bool EventCleaningSelection::applyTrigger()
  {
    const xAOD::EventInfo* eventInfo(nullptr);
    top::check(evtStore()->retrieve(eventInfo,m_config->sgKeyEventInfo()),"Failed to retrieve EventInfo");

    bool orOfAllTriggers(false);
    std::unordered_set<std::string> triggers;
    triggers.insert(m_allTriggers_Tight.begin(), m_allTriggers_Tight.end());
    triggers.insert(m_allTriggers_Loose.begin(), m_allTriggers_Loose.end());
    for (const auto& trigger : triggers) {
      //decorating event with trigger decision
      bool passThisTrigger =  m_trigDecisionTool->isPassed(trigger);
      char decoration = passThisTrigger ? 1 : 0;
      eventInfo->auxdecor<char>( "TRIGDEC_" + trigger ) = decoration; 
      orOfAllTriggers |= passThisTrigger;

      //decorating event with trigger prescale (on Data)
      if (!m_config->isMC()) {
        auto cg = m_trigDecisionTool->getChainGroup(trigger);
        float prescale = cg->getPrescale();
        eventInfo->auxdecor<float>( "TRIGPS_" + trigger ) = prescale;
      }
    }

    // match offline objects to trigger
    if (m_config->useElectrons()) matchElectrons();
    if (m_config->useMuons()) matchMuons();
    if (m_config->useTaus()) matchTaus();

    // Do we veto events? Only if ALL selectors request TRIGDEC and no trigger passes
    if (m_vetoEventsTrigger) {
     return orOfAllTriggers;
    }

    // otherwise, we don't veto the event
    return true;
  }

  void EventCleaningSelection::matchElectrons()
  {
    const xAOD::EventInfo* eventInfo(nullptr);
    top::check(evtStore()->retrieve(eventInfo,m_config->sgKeyEventInfo()),"Failed to retrieve EventInfo");

    // Take electrons from input file. Decorate these before doing any calibration/shallow copies
    const xAOD::ElectronContainer* electrons(nullptr);
    top::check(evtStore()->retrieve(electrons,m_config->sgKeyElectrons()),"Failed to retrieve electrons");

    // Loop over electrons
    std::unordered_set<std::string> triggers;
    triggers.insert(m_electronTriggers_Tight.begin(), m_electronTriggers_Tight.end());
    triggers.insert(m_electronTriggers_Loose.begin(), m_electronTriggers_Loose.end());
    for (const auto* el : *electrons) {
      // Loop over electron triggers
      for (const auto& trigger : triggers) {
        bool match(false);
        // Match even if event fails trigger decistion - it's important in case of pre-scaled menus
        if (!m_config->useGlobalTrigger() && el->isAvailable<char>(m_config->getDerivationStream() + "_" + trigger)) {
          match = el->auxdataConst<char>( m_config->getDerivationStream() + "_" + trigger);
        } else {
          match = m_trigMatchTool->match(*el, trigger);
        }
        char decoration = match ? 1 : 0;
        el->auxdecor<char>("TRIGMATCH_" + trigger ) = decoration;
      }
    }
  }

  void EventCleaningSelection::matchMuons() 
  {
    const xAOD::EventInfo* eventInfo(nullptr);
    top::check(evtStore()->retrieve(eventInfo,m_config->sgKeyEventInfo()),"Failed to retrieve EventInfo");

    // Take muons from input file. Decorate these before doing any calibration/shallow copies
    const xAOD::MuonContainer* muons(nullptr);
    top::check(evtStore()->retrieve(muons,m_config->sgKeyMuons()),"Failed to retrieve muons");

    // Loop over muons
    std::unordered_set<std::string> triggers;
    triggers.insert(m_muonTriggers_Tight.begin(), m_muonTriggers_Tight.end());
    triggers.insert(m_muonTriggers_Loose.begin(), m_muonTriggers_Loose.end());
    for (const auto* mu : *muons) {
      // Loop over muon triggers
      for (const auto& trigger : triggers) {
        bool match(false);
        // Match even if event fails trigger decistion - it's important in case of pre-scaled menus
        if (!m_config->useGlobalTrigger() && mu->isAvailable<char>(m_config->getDerivationStream() + "_" + trigger)) {
          match = mu->auxdataConst<char>(m_config->getDerivationStream() + "_" + trigger);
        } else {
          match = m_trigMatchTool->match(*mu, trigger);
        }
        char decoration = match ? 1 : 0;
        mu->auxdecor<char>("TRIGMATCH_" + trigger ) = decoration;
      }
    }
  }

  void EventCleaningSelection::matchTaus()
  {

    const xAOD::EventInfo* eventInfo(nullptr);
    top::check(evtStore()->retrieve(eventInfo,m_config->sgKeyEventInfo()),
	       "Failed to retrieve EventInfo");

    // Take taus from input file.
    // Decorate these before doing any calibration/shallow copies
    const xAOD::TauJetContainer* taus(nullptr);
    top::check(evtStore()->retrieve(taus,m_config->sgKeyTaus()),
	       "Failed to retrieve taus");

    // Loop over taus
    for (const auto* tau : *taus) {
      // Loop over tau triggers
      for (const auto& trigger : m_tauTriggers_Tight) {
        bool match(false);
        // Match even if event fails trigger decistion - it's important in case of pre-scaled menus
        match = m_trigMatchTauTool->match(tau, trigger);
        if (tau->isAvailable<char>(m_config->getDerivationStream() + "_" + trigger)) {
          match = tau->auxdataConst<char>(m_config->getDerivationStream() + "_" + trigger);
        } else {
          match = m_trigMatchTool->match(*tau, trigger);
        }
        char decoration = match ? 1 : 0;
        tau->auxdecor<char>("TRIGMATCH_" + trigger ) = decoration;
      }
      // Loop over tau triggers
      for (const auto& trigger : m_tauTriggers_Loose) {
        // let's make sure this isn't done twice
        if ( tau->isAvailable<char>("TRIGMATCH_" + trigger ) ) continue;
        bool match(false);
        // Match even if event fails trigger decistion - it's important in case of pre-scaled menus
        match = m_trigMatchTauTool->match(tau, trigger);
        if (tau->isAvailable<char>(m_config->getDerivationStream() + "_" + trigger)) {
          match = tau->auxdataConst<char>(m_config->getDerivationStream() + "_" + trigger);
        } else {
          match = m_trigMatchTool->match(*tau, trigger);
        }
        char decoration = match ? 1 : 0;
        tau->auxdecor<char>("TRIGMATCH_" + trigger ) = decoration;
      }
    }
  }

  void EventCleaningSelection::addExtraBranches( std::vector<std::string>& extraBranchList) {

    for (const auto& trigger : m_allTriggers_Tight)
      extraBranchList.push_back( "TRIGDEC_" + trigger );
    for (const auto& trigger : m_allTriggers_Loose) {
      if (std::find(extraBranchList.begin(),extraBranchList.end(),std::string{"TRIGDEC_"} + trigger) == extraBranchList.end()) { // to not add them twice
        extraBranchList.push_back( "TRIGDEC_" + trigger );
      }
    }

  }

  void EventCleaningSelection::printTriggers() const 
  {
    // printout all HLT chains
    // Debug only - this is very verbose
    m_trigConfTool->chainList()->print("",10);
  }

  std::vector<std::string> EventCleaningSelection::getIndividualFromGlobalTriggers(std::vector<std::string> const & triggers) {
    std::vector<std::string> result;
    std::string const delim("_OR_");
    for (std::string const & trigger : triggers) {
      size_t cur = 0;
      while (true) {
        size_t next = trigger.find(delim, cur);
        bool last = (next == std::string::npos);
        result.emplace_back(std::string("HLT_") + trigger.substr(cur, (last ? std::string::npos : next - cur)));
        if (last)
          break;
        cur = next + delim.size();
      }
    }
    return result;
  }

  bool EventCleaningSelection::isElectronTrigger(std::string const & trigger) const {
    top::check(trigger.find("HLT_") == 0, "Expected trigger name to start with `HLT_'");
    bool success;
    return (TrigGlobEffCorr::ImportData::associatedLeptonFlavour(trigger.substr(4), success) == xAOD::Type::Electron);
  }

  bool EventCleaningSelection::isMuonTrigger(std::string const & trigger) const {
    top::check(trigger.find("HLT_") == 0, "Expected trigger name to start with `HLT_'");
    bool success;
    return (TrigGlobEffCorr::ImportData::associatedLeptonFlavour(trigger.substr(4), success) == xAOD::Type::Muon);
  }

}
