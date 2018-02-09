/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuonResonanceSelectionTool
//  ASG Tool performing tag and probe selection
//
//  (c) ATLAS Muon Combined Performance Software
//////////////////////////////////////////////////////////////////////////////
#ifndef MuonResonanceSelectionTool_H
#define MuonResonanceSelectionTool_H
 
#include "AsgTools/AsgTool.h"
#include "AsgTools/AsgToolsConf.h"
#include "AsgTools/AsgMetadataTool.h"
#include "MuonResonanceTools/IMuonResonanceSelectionTool.h"
#ifndef ROOTCORE
#include "GaudiKernel/ToolHandle.h"
#else
#include "AsgTools/ToolHandle.h"
#endif

class MuonResonanceSelectionTool
: virtual public asg::AsgTool,
  virtual public IMuonResonanceSelectionTool {
  ASG_TOOL_CLASS(MuonResonanceSelectionTool, IMuonResonanceSelectionTool)
    
    public:

  MuonResonanceSelectionTool(std::string myname);

  virtual StatusCode initialize();

  /// Select Probes
  std::pair<std::vector<const xAOD::Muon*>,std::vector<const xAOD::Muon*> > selectMuons(const xAOD::MuonContainer*, bool isMC, CP::SystematicSet sys) const;
  // cut on Impact parameters
  bool IPCut(const xAOD::Muon& mu, float z0cut, float d0cut) const;
  bool IPCutAbs(const xAOD::Muon& mu, float Abs_z0, float Abs_d0) const;
  // cut on ID track quality
  bool IDTrk(const xAOD::Muon& mu) const;
  void applySF(const xAOD::Muon& mu, bool isMC) const;
  xAOD::Muon* copy(const xAOD::Muon& mu) const;
  bool hasPassedGRL (void) const;
  bool isTriggered (void) const;
  void applyTriggerMatch(xAOD::Muon& mu) const; 


 private:

  float m_ptCut;
  float m_etaCut;
  float m_isoCaloCut;
  float m_isoTrkCut;
  float m_highMassWindow;
  float m_lowMassWindow;
  float m_d0Cut;
  float m_z0Cut;
  float m_Abs_d0Cut;
  float m_Abs_z0Cut;

  bool m_IDCuts;
  bool m_doCalib;
  bool m_doEff;

  std::vector<std::string> m_triggerList;
 
  ToolHandle< CP::IMuonSelectionTool >              m_seliTool;
  ToolHandle< CP::IMuonEfficiencyScaleFactors >     m_sfTool;
  ToolHandle< CP::IMuonCalibrationAndSmearingTool > m_calibTool;
  ToolHandle< Trig::ITrigMuonMatching >             m_matchTool;
  ToolHandle< Trig::TrigDecisionTool >              m_trigTool;
   
  };

#endif
