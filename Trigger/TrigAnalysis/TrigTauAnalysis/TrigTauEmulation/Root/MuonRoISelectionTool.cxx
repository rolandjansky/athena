/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#include "TrigTauEmulation/MuonRoISelectionTool.h"

// Default constructor MuonRoISelectionTool
MuonRoISelectionTool::MuonRoISelectionTool(const std::string& name) : Level1SelectionTool(name)
{
  declareProperty("ClusterPt", m_roi_pt=10000., "cut on the MuonRoI transverse energy");
  declareProperty("ClusterEta", m_roi_eta=2.5, "cut on the MuonRoi |eta|");

}

// Copy constructor
MuonRoISelectionTool::MuonRoISelectionTool(const MuonRoISelectionTool& other) : Level1SelectionTool(other.name() + "_copy")
{
  m_roi_pt = other.m_roi_pt;
  m_roi_eta = other.m_roi_eta;
}

// Tool initialize
StatusCode MuonRoISelectionTool::initialize()
{
  m_accept.addCut("MuonRoI", "MuonRoI");

  return StatusCode::SUCCESS;
}

// Accept method
const Root::TAccept& MuonRoISelectionTool::accept(const xAOD::MuonRoI& l1muon) const

{
  m_accept.clear();
  m_accept.setCutResult("MuonRoI", false);

  if (fabs(l1muon.eta()) > m_roi_eta)
    return m_accept;

  if (l1muon.thrValue() < m_roi_pt)
    return m_accept;

  m_accept.setCutResult("MuonRoI", true);
  return m_accept;
}

