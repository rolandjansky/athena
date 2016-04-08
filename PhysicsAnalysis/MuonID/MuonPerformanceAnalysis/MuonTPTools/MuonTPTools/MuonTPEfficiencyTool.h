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
#include "MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h"
#include "TrigMuonMatching/ITrigMuonMatching.h"

class MuonTPEfficiencyTool
: public asg::AsgTool,
  virtual public IMuonTPEfficiencyTool {
//   ASG_TOOL_CLASS(MuonTPEfficiencyTool, IMuonTPEfficiencyTool)
    
public:
  
  MuonTPEfficiencyTool(std::string myname);

  ~MuonTPEfficiencyTool();

  virtual StatusCode initialize();

  /// Match probes for efficiency calculation
  virtual void matchProbes(ProbeContainer*, const xAOD::IParticleContainer*) const =0;
  
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
  
  // return the triger item (if any configured)
  std::string triggerItem() {return m_trigger_item;}

protected:

  double m_matchPtCut; 
  double m_matchEtaCut;
  double m_maximumDrCut;
  unsigned int m_muonAuthor;  
  std::string m_efficiencyFlag;
  bool m_isNominal;
  std::string m_trigger_item;
  ToolHandle<CP::IMuonSelectionTool> m_selection_tool;
  ToolHandle<CP::IMuonEfficiencyScaleFactors> m_sf_tool;
  bool m_do_sf;
//   ToolHandle<Trig::TrigDecisionTool> m_trigTool;
  ToolHandle<Trig::ITrigMuonMatching> m_matchTool;

};

#endif
