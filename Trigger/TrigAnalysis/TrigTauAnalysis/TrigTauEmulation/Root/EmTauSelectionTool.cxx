/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#include "TrigTauEmulation/EmTauSelectionTool.h"

// Default constructor
EmTauSelectionTool::EmTauSelectionTool(const std::string& name) : Level1SelectionTool(name)
{
  declareProperty("ClusterPt", m_roi_pt_cut=12000., "cut on the EmTauRoI transverse energy");
  declareProperty("ClusterEta", m_roi_eta_cut=2.5, "cut on the EmTauRoi |eta|");
  declareProperty("IsolationSlope", m_iso_slope=0.1, "slope of the pT-dep. isolation cut");
  declareProperty("IsolationOffset", m_iso_offset=2000., "Offset of the pT-dep. isolation cut");
  declareProperty("IsolationThresh", m_iso_thresh=60000., "Threshold of the pT-dep. isolation cut");
  declareProperty("IsolationMinimum", m_iso_min=0., "Minimum Value on pT-dep. isolation cut");
  declareProperty("HadronicLeakageSlope", m_had_leak_slope=0.1, "slope of the pT-dep. hadronic leakage cut");
  declareProperty("HadronicLeakageOffset", m_had_leak_offset=2000., "Offset of the pT-dep. hadronic leakage cut");
  declareProperty("HadronicLeakageThresh", m_had_leak_thresh=60000., "Threshold of the pT-dep. hadronic leakage cut");
  declareProperty("HadronicLeakageMinimum", m_had_leak_min=1000., "Minimum Value on pT-dep. hadronic leakage cut");
  declareProperty("UseEmClus", m_use_emclus=false, "Switch TAU/EM cluster");
  declareProperty("UseHadCore", m_use_had_core=false, "Hadronic core req. switch");

}

// Copy constructor
EmTauSelectionTool::EmTauSelectionTool(const EmTauSelectionTool& other) : Level1SelectionTool(other.name() + "_copy") {
  m_roi_pt_cut = other.m_roi_pt_cut;
  m_roi_eta_cut = other.m_roi_eta_cut;
  m_iso_slope = other.m_iso_slope;
  m_iso_offset = other.m_iso_offset;
  m_iso_thresh = other.m_iso_thresh;
  m_iso_min = other.m_iso_min;
  m_had_leak_slope = other.m_had_leak_slope;
  m_had_leak_offset = other.m_had_leak_offset;
  m_had_leak_thresh = other.m_had_leak_thresh;
  m_had_leak_min = other.m_had_leak_min;
  m_use_emclus = other.m_use_emclus;
  m_use_had_core = other.m_use_had_core;
  
}

// Tool initialize
StatusCode EmTauSelectionTool::initialize()
{
  m_accept.addCut("EmTauRoI", "EmTauRoI");

  return StatusCode::SUCCESS;
}

const asg::AcceptInfo& EmTauSelectionTool::getAcceptInfo() const
{
  return m_accept;
}


// Accept method
asg::AcceptData EmTauSelectionTool::accept(const xAOD::EmTauRoI& l1tau) const

{
  asg::AcceptData acceptData (&m_accept);
  acceptData.setCutResult("EmTauRoI", false);

  // if (fabs(l1tau.eta()) >= m_roi_eta_cut)
  //   return acceptData;

  double roi_pt = m_use_emclus ? l1tau.emClus() : l1tau.tauClus();
  //std::cout << "=> testing L1 cut" << std::endl;
  if (roi_pt <= m_roi_pt_cut) {
    //std::cout << "reject L1: did not pass pT cut (" << roi_pt << " <= " << m_roi_pt_cut <<  ")" << std::endl;
    return acceptData;
  }

  if (!pass_isolation(l1tau)) {
    //std::cout << "reject L1: did not pass isolation cut" << std::endl;
    return acceptData;
  }

  if (!pass_hadronic_leakage(l1tau)) {
    //std::cout << "reject L1: did not pass hadronic leakage" << std::endl;
    return acceptData;
  }

  int roi_type = m_use_emclus ? xAOD::EmTauRoI::EMRoIWord : xAOD::EmTauRoI::TauRoIWord;
  if (l1tau.roiType() != roi_type) {
    //std::cout << "reject L1: did not pass RoI type cut" << std::endl;
    return acceptData;
  }

  //std::cout << "PASSED L1 CUTS" << std::endl;
  acceptData.setCutResult("EmTauRoI", true);
  return acceptData;
}

bool EmTauSelectionTool::pass_isolation(const xAOD::EmTauRoI& l1tau) const
{
  // No isolation applied at high-pT
  double roi_pt = m_use_emclus ? l1tau.emClus() : l1tau.tauClus();
  if (roi_pt >= m_iso_thresh)
    return true;

  float isolation_cut = m_iso_offset + m_iso_slope * roi_pt;
  float roi_isol = l1tau.emIsol();
  if (isolation_cut < m_iso_min) 
    isolation_cut = m_iso_min;
  if  (roi_isol <= isolation_cut)
    return true;
  else
    return false;
  
}

bool EmTauSelectionTool::pass_hadronic_leakage(const xAOD::EmTauRoI& l1tau) const
{
  // when cut is not used
  if (!m_use_had_core)
    return true;

  // No isolation applied at high-pT
  double roi_pt = m_use_emclus ? l1tau.emClus() : l1tau.tauClus();
  if (roi_pt >= m_iso_thresh)
    return true;

  float had_leak_cut = m_had_leak_offset + m_had_leak_slope * roi_pt;
  if (had_leak_cut < m_had_leak_min) 
    had_leak_cut = m_had_leak_min;
  if  (l1tau.hadCore() <= had_leak_cut)
    return true;
  else
    return false;



}
