/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauEmulation/JetRoISelectionTool.h"

// Default constructor
JetRoISelectionTool::JetRoISelectionTool(const std::string& name) : asg::AsgTool(name)
{
  declareProperty("ClusterPt", m_roi_pt=25000., "cut on the JetRoI transverse energy");
  declareProperty("ClusterEta", m_roi_eta=3.2, "cut on the JetRoi |eta|");

}

// Copy constructor
JetRoISelectionTool::JetRoISelectionTool(const JetRoISelectionTool& other) : asg::AsgTool(other.name() + "_copy")
{}

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

  if (fabs(l1jet.eta()) > m_roi_eta)
    return m_accept;

  if (l1jet.et8x8() <= m_roi_pt)
    return m_accept;

  m_accept.setCutResult("JetRoI", true);
  return m_accept;
}

