/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#include "TrigTauEmulation/JetRoISelectionTool.h"
#include "TrigTauEmulation/MsgStream.h"

JetRoISelectionTool::JetRoISelectionTool(const std::string& name) : Level1SelectionTool(name) {
  declareProperty("ClusterPt", m_roi_pt=25000., "cut on the JetRoI transverse energy");
  declareProperty("ClusterEta", m_roi_eta=3.2, "cut on the JetRoi |eta|");
}

JetRoISelectionTool::JetRoISelectionTool(const JetRoISelectionTool& other) : Level1SelectionTool(other.name() + "_copy") {
  m_roi_pt = other.m_roi_pt;
  m_roi_eta = other.m_roi_eta;

}

// Tool initialize
StatusCode JetRoISelectionTool::initialize()
{
  m_accept.addCut("JetRoI", "JetRoI");

  return StatusCode::SUCCESS;
}

// Accept method
const Root::TAccept& JetRoISelectionTool::accept(const xAOD::JetRoI& l1jet) const
{
  m_accept.clear();
  m_accept.setCutResult("JetRoI", false);

  if (fabs(l1jet.eta()) > m_roi_eta) {
    MY_MSG_DEBUG("reject L1: did not pass RoI eta cut");
    return m_accept;
  }

  // if (l1jet.et8x8() <= m_roi_pt)
  //   return m_accept;

  if (l1jet.etLarge() <= m_roi_pt) {
    MY_MSG_DEBUG("reject L1: did not pass RoI pT cut");
    return m_accept;
  }

  m_accept.setCutResult("JetRoI", true);
  return m_accept;
}

