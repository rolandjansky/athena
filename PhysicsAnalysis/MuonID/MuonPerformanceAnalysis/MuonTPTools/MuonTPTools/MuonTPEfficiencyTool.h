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

class MuonTPEfficiencyTool
: virtual public asg::AsgTool,
  virtual public IMuonTPEfficiencyTool {
  ASG_TOOL_CLASS(MuonTPEfficiencyTool, IMuonTPEfficiencyTool)
    
public:
  
  MuonTPEfficiencyTool(std::string myname);

  ~MuonTPEfficiencyTool();

  virtual StatusCode initialize();

  /// Match probes for efficiency calculation
  void matchProbes(ProbeContainer*, const xAOD::IParticleContainer*) const;

  /// dR-based matching
  virtual void dRMatching(ProbeContainer* probes, const xAOD::IParticleContainer* matches) const;

  /// Calculate dR
  double deltaR(Probe* probe, const xAOD::IParticle* match) const;

  /// Get Efficiency Flag
  std::string efficiencyFlag() {return m_efficiencyFlag;}

protected:

  double m_matchPtCut; 
  double m_matchEtaCut;
  bool m_dRMatching;
  double m_maximumDrCut;
  unsigned int m_muonAuthor;  
  std::string m_efficiencyFlag;
  ToolHandle<CP::IMuonSelectionTool> m_selection_tool;
  ToolHandle<CP::IMuonEfficiencyScaleFactors> m_sf_tool;
  bool m_do_sf;

};

#endif
