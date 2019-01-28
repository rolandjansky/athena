/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2

#include "TrigTauEmulation/EnergySumSelectionTool.h"
#include <TMath.h>
// Default constructor
EnergySumSelectionTool::EnergySumSelectionTool(const std::string& name) : Level1SelectionTool(name)
{
  declareProperty("XE_cut", m_MET_cut=0., "cut on the EnergySumRoI transverse energy");
}

// Copy constructor
EnergySumSelectionTool::EnergySumSelectionTool(const EnergySumSelectionTool& other) : Level1SelectionTool(other.name() + "_copy") {
  m_MET_cut = other.m_MET_cut;
}

// Tool initialize
StatusCode EnergySumSelectionTool::initialize()
{
  m_accept.addCut("EnergySumRoI", "EnergySumRoI");

  return StatusCode::SUCCESS;
}

const asg::AcceptInfo& EnergySumSelectionTool::getAcceptInfo() const
{
  return m_accept;
}


// Accept method
asg::AcceptData EnergySumSelectionTool::accept(const xAOD::EnergySumRoI& l1xe) const

{
  asg::AcceptData acceptData (&m_accept);
  acceptData.setCutResult("EnergySumRoI", false);

  if (calculate_MET(l1xe) < m_MET_cut)
    return acceptData;

  acceptData.setCutResult("EnergySumRoI", true);
  return acceptData;
}

double EnergySumSelectionTool::calculate_MET(const xAOD::EnergySumRoI& l1xe) const{
  double ENERGYSUM_ex = l1xe.energyX();
  double ENERGYSUM_ey = l1xe.energyY();
  double L1_XE_met = TMath::Sqrt(TMath::Power(ENERGYSUM_ex, 2) + TMath::Power(ENERGYSUM_ey, 2));
  return L1_XE_met;
}
