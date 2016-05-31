/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
// author Quentin Buat <quentin.buat@no.spam.cern.ch>
#ifndef HLTEMULATIONTOOL_HLTEMULATIONTOOL_H
#define HLTEMULATIONTOOL_HLTEMULATIONTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolStore.h"
#include "AsgTools/ToolHandleArray.h"
#include "AsgTools/ToolHandle.h"

// Trigger Decision Tool
#include "TrigDecisionTool/TrigDecisionTool.h"

//Local includes
#include "TrigTauEmulation/IHltEmulationTool.h"
#include "TrigTauEmulation/ILevel1EmulationTool.h"
#include "TrigTauEmulation/IHltTauSelectionTool.h"

#include "TrigTauEmulation/HltL1MatchingTool.h"
#include "TrigTauEmulation/HltChain.h"

#include "TrigTauEmulation/DecoratedHltTau.h"
#include "TrigTauEmulation/MsgStream.h"

namespace TrigTauEmul {
  class HltEmulationTool : virtual public IHltEmulationTool, public asg::AsgTool
  {

    /// Proper constructor for Athena
    ASG_TOOL_CLASS(HltEmulationTool, IHltEmulationTool)

    public:

      /// Standard constructor for standalone usage
      HltEmulationTool(const std::string& name);

      /// Copy constructor for reflex in Athena
      HltEmulationTool(const HltEmulationTool& other);
      HltEmulationTool& operator=(const HltEmulationTool& other) = delete;

      /// virtual destructor
      virtual ~HltEmulationTool();

      /// Initialize the tool
      virtual StatusCode initialize();

      /// Initialize the tool
      virtual StatusCode finalize();

      virtual StatusCode execute(const xAOD::EmTauRoIContainer* l1taus, 
          const xAOD::JetRoIContainer* l1jets,
          const xAOD::MuonRoIContainer* l1muons,
          const xAOD::EnergySumRoI* l1xe,
          const std::vector<DecoratedHltTau>& hlt_taus);

      virtual StatusCode calculate(const std::vector<DecoratedHltTau>& hlt_taus,
          const xAOD::EmTauRoIContainer* l1taus);

      virtual bool decision(const std::string & chain_name);

      void clearL1Decision();

      const std::map<std::string, HltChain> getHltChains() const { return m_chains; }

    private:
      bool m_perform_l1_emulation;  
      //bool m_recalculateBDTscore;

      unsigned int m_HLTTriggerCondition;
      unsigned int m_L1TriggerCondition;

      HltL1MatchingTool* m_matching_tool;

      std::vector<std::string> m_hlt_chains_vec; //chains that we get passed
      std::map<std::string, HltChain> m_chains; //chains that we actually use

      std::map<std::string, bool> m_HLT_tau_decision;
      std::map<std::string, bool> m_L1_tau_decision;
      std::map<std::string, bool> m_L1_tau_decision_calculated;

      void reset_tau_decision();
      StatusCode calculate_tau_decision(const std::vector<DecoratedHltTau>& hlt_taus,
          const xAOD::EmTauRoIContainer* l1taus);

      ToolHandle<TrigTauEmul::ILevel1EmulationTool> m_l1_emulation_tool;
      //ToolHandleArray<IHltTauSelectionTool> m_hlt_tau_tools;
      ToolHandle<Trig::TrigDecisionTool> *m_trigdec_tool;
  };

}  // End of the namespace

#endif
