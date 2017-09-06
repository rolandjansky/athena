/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventCleaningSelection.cxx 802748 2017-04-11 20:29:03Z iconnell $
#include "TopObjectSelectionTools/EventCleaningSelection.h"
#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTracking/VertexContainer.h"


#include <sstream>
#include <list>

namespace top {

  EventCleaningSelection::EventCleaningSelection( const std::string& name ) :
    asg::AsgTool( name ),
    m_config(nullptr),

    m_grlTool("GoodRunsListSelectionTool"),

    m_trigConfTool("TrigConf::xAODConfigTool"),
    m_trigDecisionTool("Trig::TrigDecisionTool"),
    m_trigMatchTool("Trig::MatchingTool"),
    m_trigMatchTauTool("Trig::TrigTauMatchingTool"),

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

  StatusCode EventCleaningSelection::initilize()
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
    std::list<std::string> tmpAllTriggers;

    m_allTriggers.clear();
    m_electronTriggers.clear();
    m_muonTriggers.clear();
    m_tauTriggers.clear();

    // Trigger maps for TopConfig - to be used by individual selectors
    std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> allTriggers_perSelector
      ( new std::unordered_map<std::string,std::vector<std::string>> );

    std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> electronTriggers_perSelector
      ( new std::unordered_map<std::string,std::vector<std::string>> );

    std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> muonTriggers_perSelector
      ( new std::unordered_map<std::string,std::vector<std::string>> );

    std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> tauTriggers_perSelector
      ( new std::unordered_map<std::string,std::vector<std::string>> );


    // Loop over all selections
    m_vetoEventsTrigger = true;
    m_vetoEventsGRL = true;
    m_vetoEventsGoodCalo = true;
    m_vetoEventsPriVtx = true;

    for (auto sel : selections) {

      std::list<std::string> listAllTriggers_thisSelector;
      std::vector<std::string> allTriggers_thisSelector;
      std::vector<std::string> electronTriggers_thisSelector;
      std::vector<std::string> muonTriggers_thisSelector;
      std::vector<std::string> tauTriggers_thisSelector;

      // Loop over cut names and look for TRIGDEC, GRL, GOODCALO, PRIVTX
      bool selectionHasTriggerCut(false),selectionHasGRLCut(false);
      bool selectionHasGOODCALOCut(false),selectionHasPRIVTXCut(false);
      for (auto cut : sel.m_cutnames) {

        if (cut.find("GRL") != std::string::npos) {
          selectionHasGRLCut = true;
        }

        if (cut.find("GOODCALO") != std::string::npos) {
          selectionHasGOODCALOCut = true;
        }

        if (cut.find("PRIVTX") != std::string::npos) {
          selectionHasPRIVTXCut = true;
        }

        if (cut.find("TRIGDEC") != std::string::npos) {
          selectionHasTriggerCut = true;
          ATH_MSG_INFO("Triggers for Selection \t"<<sel.m_name<<"\tare "<<cut);

          //split the trigger string at spaces
          std::stringstream ss(cut);
          std::string item;
          char delim = ' ';
          while (std::getline(ss, item, delim)) {
            if (item.size() > 0 && item.find("TRIGDEC") == std::string::npos) {
              tmpAllTriggers.push_back(item);
              listAllTriggers_thisSelector.push_back(item);
            }
          }
          tmpAllTriggers.sort();
          tmpAllTriggers.unique();
          listAllTriggers_thisSelector.sort();
          listAllTriggers_thisSelector.unique();

          // Turn list into vector
          for (auto trigger : listAllTriggers_thisSelector) {
            allTriggers_thisSelector.push_back( trigger );
          }
          // Split triggers into electron, muon and tau
          for (const auto& trigger : allTriggers_thisSelector) {
            if ( (trigger.find("HLT_e") != std::string::npos) || (trigger.find("HLT_2e") != std::string::npos) ) {
              electronTriggers_thisSelector.push_back(trigger);
            }
            if ( (trigger.find("HLT_mu") != std::string::npos) || (trigger.find("HLT_2mu") != std::string::npos) || (trigger.find("_mu") != std::string::npos) ) {
              muonTriggers_thisSelector.push_back(trigger);
            }
	    if ( (trigger.find("_tau") != std::string::npos) ) {
              tauTriggers_thisSelector.push_back(trigger);
            }
          }

          allTriggers_perSelector->insert( std::make_pair( sel.m_name , allTriggers_thisSelector ) );
          electronTriggers_perSelector->insert( std::make_pair( sel.m_name , electronTriggers_thisSelector ) );
          muonTriggers_perSelector->insert( std::make_pair( sel.m_name , muonTriggers_thisSelector ) );
	  tauTriggers_perSelector->insert( std::make_pair( sel.m_name , tauTriggers_thisSelector ) );

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

    // Turn list into vector
    for (auto trigger : tmpAllTriggers) {
      m_allTriggers.push_back( trigger );
    }

    ATH_MSG_INFO("All requested triggers are:");
    for (const auto& trigger : m_allTriggers) {
      ATH_MSG_INFO("  "<<trigger);
    }

    std::string outputInfoString("True");
    if (!m_vetoEventsTrigger) {
      outputInfoString = "False";
    }
    ATH_MSG_INFO("Apply event veto on trigger decision = "<<outputInfoString);

    // Split triggers into electron, muon and tau
    for (const auto& trigger : m_allTriggers) {
      if ( (trigger.find("HLT_e") != std::string::npos) || (trigger.find("HLT_2e") != std::string::npos) ) {
        m_electronTriggers.push_back(trigger);
      }
      if ( (trigger.find("HLT_mu") != std::string::npos) || (trigger.find("HLT_2mu") != std::string::npos) || (trigger.find("_mu") != std::string::npos) ) {
        m_muonTriggers.push_back(trigger);
      }
      if ( (trigger.find("_tau") != std::string::npos) ) {
        m_tauTriggers.push_back(trigger);
      }
    }

    // Tell TopConfig about the triggers
    m_config->allTriggers( allTriggers_perSelector );
    m_config->electronTriggers( electronTriggers_perSelector );
    m_config->muonTriggers( muonTriggers_perSelector );
    m_config->tauTriggers( tauTriggers_perSelector );

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
    for (const auto& trigger : m_allTriggers) {
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

    // Take muons from input file. Decorate these before doing any calibration/shallow copies
    const xAOD::ElectronContainer* electrons(nullptr);
    top::check(evtStore()->retrieve(electrons,m_config->sgKeyElectrons()),"Failed to retrieve electrons");

    // Loop over muons
    for (const auto* el : *electrons) {
      // Loop over muon triggers
      for (const auto& trigger : m_electronTriggers) {
        bool match(false);
        // Match even if event fails trigger decistion - it's important in case of pre-scaled menus
        if (el->isAvailable<char>(m_config->getDerivationStream() + "_" + trigger)) {
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
    for (const auto* mu : *muons) {
      // Loop over muon triggers
      for (const auto& trigger : m_muonTriggers) {
        bool match(false);
        // Match even if event fails trigger decistion - it's important in case of pre-scaled menus
        if (mu->isAvailable<char>(m_config->getDerivationStream() + "_" + trigger)) {
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
      for (const auto& trigger : m_tauTriggers) {
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

    for (const auto& trigger : m_allTriggers)
      extraBranchList.push_back( "TRIGDEC_" + trigger );

  }

  void EventCleaningSelection::printTriggers() const 
  {
    // printout all HLT chains
    // Debug only - this is very verbose
    m_trigConfTool->chainList()->print("",10);
  }

}
