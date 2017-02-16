/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuonTPEfficiencyTool
//  ASG Tool performing probe matching
//
//  (c) ATLAS Muon Combined Performance Software
//////////////////////////////////////////////////////////////////////////////
#ifndef MuonTPEfficiencyTool_H
#define MuonTPEfficiencyTool_H

#include "MuonTPTools/IMuonTPEfficiencyTool.h"
#include "MuonSelectorTools/IMuonSelectionTool.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "MuonEfficiencyCorrections/IMuonEfficiencyScaleFactors.h"
#include "MuonEfficiencyCorrections/IMuonTriggerScaleFactors.h"
#include "MuonTPTools/IIDTrackIsolationDecoratorTool.h"
#include "MuonTPTools/MuonTPTrigUtils.h"
#include "IsolationSelection/IsolationSelectionTool.h"

class MuonTPEfficiencyTool
: public asg::AsgTool,
  virtual public IMuonTPEfficiencyTool {
//   ASG_TOOL_CLASS(MuonTPEfficiencyTool, IMuonTPEfficiencyTool)
    
public:
  
  MuonTPEfficiencyTool(std::string myname);

  ~MuonTPEfficiencyTool();

  virtual StatusCode initialize();

  /// Match probes for efficiency calculation
  virtual void matchProbes(ProbeContainer*) const =0;
  
  /// dR-based matching
  virtual void dRMatching(ProbeContainer* probes, const xAOD::IParticleContainer* matches) const;

  /// Calculate dR
  double deltaR(Probe* probe, const xAOD::IParticle* match) const;

  /// Get Efficiency Flag
  std::string efficiencyFlag() {return m_efficiencyFlag;}
  
  //  check for a trigger match (probe side)
  bool MatchTrigger (const xAOD::IParticle* match,  std::string trigger) const;
  
  // check if the tool represents a nominal matching
  bool isNominal() const {return m_isNominal;}
  
  // check if the tool applies SF
  bool doesApplySF() const {return m_do_sf;}
  
  // return the triger item (if any configured)
  std::string triggerItem() {return m_trigger_item;}

  // return if the tool is using a rerun mode trigger
  bool rerunTriggerMode() {return m_rerunMode;}  

protected:

  double m_matchPtCut; 
  double m_matchEtaCut;
  double m_maximumDrCut;
  unsigned int m_muonAuthor;  
  std::string m_efficiencyFlag;
  bool m_isNominal;
  std::string m_trigger_item;
  ToolHandle<CP::IMuonSelectionTool> m_selection_tool;
  ToolHandle<CP::IMuonEfficiencyScaleFactors> m_reco_sf_tool;
  ToolHandle<CP::IMuonEfficiencyScaleFactors> m_isol_sf_tool;
  ToolHandle<CP::IMuonTriggerScaleFactors> m_trig_sf_tool;
  bool m_do_sf;

  ToolHandle<IMuonTPTrigUtils> m_trigUtils;
  ToolHandle<CP::IIsolationSelectionTool> m_isolTool;
  ToolHandle<IIDTrackIsolationDecoratorTool> m_IDtrack_isol_tool;
  bool m_rerunMode;

};

#endif
