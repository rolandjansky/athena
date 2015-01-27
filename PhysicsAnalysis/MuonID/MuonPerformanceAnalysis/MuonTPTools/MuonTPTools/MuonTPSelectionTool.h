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

class MuonTPSelectionTool
: virtual public asg::AsgTool,
  virtual public IMuonTPSelectionTool {
  ASG_TOOL_CLASS(MuonTPSelectionTool, IMuonTPSelectionTool)
    
public:

  MuonTPSelectionTool(std::string myname);

  virtual StatusCode initialize();

  /// Select Probes
  ProbeContainer* selectProbes(const xAOD::MuonContainer*, const xAOD::IParticleContainer*) const;

  /// Get Efficiency Flag
  std::string efficiencyFlag() {return m_efficiencyFlag;}

  /// Check if Probe and Tag are the same object
  bool isTag(const xAOD::Muon* tag, const xAOD::IParticle* probe) const;

  /// return the product q_1 * q_2
  int ChargeProd(const xAOD::Muon* tag, const xAOD::IParticle* probe) const;

  // helper Method to get the absolute value of delta phi between tag and probe
  double DeltaPhiTP (const xAOD::Muon* tag, const xAOD::IParticle* probe) const;

  // helper method to get the ptcone40 isolation of an ID track
  double ptcone_trk (const xAOD::IParticle* part, const xAOD::IParticleContainer* cont, double conesize = 0.40) const;

  // helper method to select Truth probes
  bool isFinalStateTruthMuon(const xAOD::IParticle* part) const;

  // select muons that 'probably' fired the trigger - workaround for 19.1 xAODs without trigger info!
  bool passDummyTrigger(const xAOD::Muon* tag) const;


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
  ToolHandle<CP::IMuonSelectionTool> m_selection_tool;
  std::vector<MuonTPCutFlowBase*> m_cutFlows;


};

#endif
