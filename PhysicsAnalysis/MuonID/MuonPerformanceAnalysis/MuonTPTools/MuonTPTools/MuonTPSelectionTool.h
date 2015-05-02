/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuonTPSelectionTool
//  ASG Tool performing tag and probe selection
//
//  (c) ATLAS Muon Combined Performance Software
//////////////////////////////////////////////////////////////////////////////
#ifndef MuonTPSelectionTool_H
#define MuonTPSelectionTool_H

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "MuonTPTools/IMuonTPSelectionTool.h"
#include "MuonSelectorTools/IMuonSelectionTool.h"
#include "xAODTruth/TruthParticle.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigMuonMatching/ITrigMuonMatching.h"
#include "AsgTools/AsgToolsConf.h"
#include "AsgTools/AsgMetadataTool.h"

//  for full athena, we also use the isolation tools
# if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
#include "RecoToolInterfaces/ITrackIsolationTool.h"
#include "RecoToolInterfaces/ICaloTopoClusterIsolationTool.h"
# endif


class MuonTPSelectionTool
: virtual public asg::AsgTool,
  virtual public IMuonTPSelectionTool {
  ASG_TOOL_CLASS(MuonTPSelectionTool, IMuonTPSelectionTool)

public:

  MuonTPSelectionTool(std::string myname);

  virtual StatusCode initialize();

  /// Select Probes
  ProbeContainer* selectProbes(const xAOD::MuonContainer*, const xAOD::IParticleContainer*) const;

  /// Get Efficiency Flag. Used to name the tools etc
  std::string efficiencyFlag() {return m_efficiencyFlag;}

  /// Check if Probe and Tag are the same object
  bool isTag(const xAOD::Muon* tag, const xAOD::IParticle* probe) const;

  /// return the product q_1 * q_2
  int ChargeProd(const xAOD::Muon* tag, const xAOD::IParticle* probe) const;

  // helper Method to get the absolute value of delta phi between tag and probe
  double DeltaPhiTP (const xAOD::Muon* tag, const xAOD::IParticle* probe) const;

  // helper method to get the ptcone40 isolation of an ID track
  double ptcone_trk (const xAOD::IParticle* part, double conesize = 0.40) const;
  // helper method to get the etcone40 isolation of an ID track
  double etcone_trk (const xAOD::IParticle* part, double conesize = 0.40) const;

  // helper method to select Truth probes
  bool isFinalStateTruthMuon(const xAOD::IParticle* part) const;

  // select muons that 'probably' fired the trigger - workaround for 19.1 xAODs without trigger info!
  bool passDummyTrigger(const xAOD::Muon* tag) const;

  //  checks if the event fired one of the triggers specified in the tool properties
  bool Event_Trigger (void) const;
  
  //  performs a tag-side trigger match
  bool TagTriggerMatch (const xAOD::Muon* tag) const;

  // apply impact parameter cuts
  bool PassIPCuts(const xAOD::TrackParticle* probe, double d0cut, double d0signcut, double z0cut) const;

  virtual void AddCutFlowHist(MuonTPCutFlowBase* hist);
  virtual void FillCutFlows(std::string step, double weight =1.) const ;

protected:


  double m_tagPtCut;
  double m_tagEtaCut;
  double m_probePtCut;
  double m_probeEtaCut;
  double m_highMassWindow;
  double m_lowMassWindow;
  std::string m_efficiencyFlag;
  std::vector<std::string> m_tag_Triggers;
  ToolHandle<CP::IMuonSelectionTool> m_selection_tool;
  std::vector<MuonTPCutFlowBase*> m_cutFlows;
  ToolHandle<Trig::TrigDecisionTool> m_trigTool;
  ToolHandle<Trig::ITrigMuonMatching> m_matchTool;
  
# if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
  ToolHandle<xAOD::ITrackIsolationTool> m_track_iso_tool;
  ToolHandle<xAOD::ICaloTopoClusterIsolationTool> m_calo_iso_tool;
  std::vector<xAOD::Iso::IsolationType> m_track_iso_to_run;
  std::vector<xAOD::Iso::IsolationType> m_calo_iso_to_run;
#   endif
  

};

#endif
