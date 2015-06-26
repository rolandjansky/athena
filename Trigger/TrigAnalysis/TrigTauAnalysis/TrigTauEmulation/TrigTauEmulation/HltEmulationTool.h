/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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

  /// virtual destructor
  virtual ~HltEmulationTool() { };

  /// Initialize the tool
  virtual StatusCode initialize();

  /// Initialize the tool
  virtual StatusCode finalize();


  virtual StatusCode execute(const xAOD::EmTauRoIContainer* l1taus, 
			     const xAOD::JetRoIContainer* l1jets,
			     const xAOD::MuonRoIContainer* l1muons,
			     const xAOD::EnergySumRoI* l1xe,
			     const xAOD::TauJetContainer* hlt_taus,
			     const DataVector<xAOD::TrackParticle>* fast_tracks);

  /// 
  virtual StatusCode calculate(const xAOD::TauJetContainer* hlt_taus,
			       const xAOD::EmTauRoIContainer* l1taus,
			       const DataVector<xAOD::TrackParticle>* fast_tracks);

  virtual StatusCode execute(const xAOD::EmTauRoIContainer* l1taus, 
			     const xAOD::JetRoIContainer* l1jets,
			     const xAOD::MuonRoIContainer* l1muons,
			     const xAOD::EnergySumRoI* l1xe,
			     const xAOD::TauJetContainer* hlt_taus,
			     const xAOD::TauJetContainer* presel_taus);

  /// 
  virtual StatusCode calculate(const xAOD::TauJetContainer* hlt_taus,
			       const xAOD::TauJetContainer* presel_taus,
			       const xAOD::EmTauRoIContainer* l1taus);

  virtual bool decision(const std::string & chain_name);


 private:
  bool m_perform_l1_emulation;  
  bool m_recalculateBDTscore;

  HltL1MatchingTool * m_matching_tool;

  std::vector<std::string> m_hlt_chains_vec;

  std::map<std::string, bool> m_tau_decision;

  void reset_tau_decision();
  StatusCode calculate_tau_decision(const xAOD::TauJetContainer* hlt_taus,
				    const xAOD::EmTauRoIContainer* l1taus);

  ToolHandle<TrigTauEmul::ILevel1EmulationTool> m_l1_emulation_tool;
  ToolHandleArray<IHltTauSelectionTool> m_hlt_tau_tools;
  ToolHandle<Trig::TrigDecisionTool> *m_trigdec_tool;



  std::map<std::string, HltChain> m_chains;


};

}  // End of the namespace

#endif
