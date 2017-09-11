/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventCleaningSelection.h 809763 2017-08-25 11:04:10Z grancagn $
#ifndef ANALYSISTOP_TOPOBJECTSELECTIONTOOLS_EVENTCLEANINGSELECTION_H
#define ANALYSISTOP_TOPOBJECTSELECTIONTOOLS_EVENTCLEANINGSELECTION_H

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

// GRL include(s):
#include "AsgAnalysisInterfaces/IGoodRunsListSelectionTool.h"

// Trigger include(s):
#include "TrigConfInterfaces/ITrigConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TriggerMatchingTool/IMatchingTool.h"
#include "TrigTauMatching/ITrigTauMatching.h"

// Top include(s):
#include "TopConfiguration/SelectionConfigurationData.h"

namespace top{
  class TopConfig;
  
  class EventCleaningSelection final : public asg::AsgTool {
    public:
      explicit EventCleaningSelection( const std::string& name );
      virtual ~EventCleaningSelection(){}
      
      StatusCode initialize();
      
      void setEventSelections( const std::vector<top::SelectionConfigurationData>& selections );
      bool applyGRL() const;
      bool applyTrigger();
      bool applyGoodCalo() const;
      bool applyPrimaryVertex() const;
      
      // Print trigger configuration - very verbose, but tells you which triggers are included for an event
      void printTriggers() const;

      // helper to allow us to added trigger decisions to our output files
      void addExtraBranches( std::vector<std::string>& extraBranchList );
      
      // Delete Standard constructors
      EventCleaningSelection(const EventCleaningSelection& rhs) = delete;
      EventCleaningSelection(EventCleaningSelection&& rhs) = delete;
      EventCleaningSelection& operator=(const EventCleaningSelection& rhs) = delete;
      
    private:
      
      ///Match offline electrons to the trigger
      void matchElectrons();
      
      ///Match offline muons to the trigger
      void matchMuons();

      ///Match offline taus to the trigger
      void matchTaus();
      
      /// Top config
      std::shared_ptr<top::TopConfig> m_config;
      
      /// GoodRunsLists
      ToolHandle<IGoodRunsListSelectionTool> m_grlTool;
      
      /// Trigger configuration tool
      ToolHandle<TrigConf::ITrigConfigTool> m_trigConfTool;
      
      ///Trigger decision tool
      ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;

      // Trigger matching tool
      ToolHandle<Trig::IMatchingTool> m_trigMatchTool;

      //Tau trigger matching
      ToolHandle<Trig::ITrigTauMatchingTool> m_trigMatchTauTool;
      
      ///List of triggers to 'or' together for each event. If any one passes, the event passes
      std::vector<std::string> m_allTriggers_Tight;
      std::vector<std::string> m_electronTriggers_Tight;
      std::vector<std::string> m_muonTriggers_Tight;
      std::vector<std::string> m_tauTriggers_Tight;
      std::vector<std::string> m_allTriggers_Loose;
      std::vector<std::string> m_electronTriggers_Loose;
      std::vector<std::string> m_muonTriggers_Loose;
      std::vector<std::string> m_tauTriggers_Loose;
      
      /// Apply a logical OR cut to all supplied triggers
      /// If ANY selection does not request the trigger, this will not be set 
      /// In this case, the apply method will always return true
      ///
      /// If ALL selections request a trigger, then we can veto events 
      /// and not run the systematics, pre-OR cuts and the overlap removal
      bool m_vetoEventsTrigger;
      
      /// If ALL selections request GRL, then we can veto events
      /// and not run the systematics, pre-OR cuts and the overlap removal
      bool m_vetoEventsGRL;
      
      /// If ALL selections request GOODCALO, then we can veto events
      /// Data only
      bool m_vetoEventsGoodCalo;
      
      /// If ALL selections request PRIVTX, then we can veto events
      bool m_vetoEventsPriVtx;
  };
}
#endif
