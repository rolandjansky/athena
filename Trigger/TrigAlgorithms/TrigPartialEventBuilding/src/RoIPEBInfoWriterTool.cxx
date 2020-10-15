/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "RoIPEBInfoWriterTool.h"
#include "CxxUtils/phihelper.h"
#include <algorithm>
#include <unordered_map>
#include <string_view>

// =============================================================================

RoIPEBInfoWriterTool::RoIPEBInfoWriterTool(const std::string& type, const std::string& name, const IInterface* parent)
: PEBInfoWriterToolBase(type,name,parent) {}

// =============================================================================

StatusCode RoIPEBInfoWriterTool::initialize() {
  ATH_MSG_DEBUG("Initialising RoIPEBInfoWriterTool/" << name());

  ATH_CHECK(m_regionSelectorTools.retrieve());

  m_extraPebInfo.robs.insert(m_extraROBs.begin(), m_extraROBs.end());
  m_extraPebInfo.subdets.insert(m_extraSubDets.begin(), m_extraSubDets.end());
  ATH_MSG_DEBUG("Extra PEBInfo attached to every passed event: " << m_extraPebInfo);

  return StatusCode::SUCCESS;
}

// =============================================================================

PEBInfoWriterToolBase::PEBInfo RoIPEBInfoWriterTool::createPEBInfo(const PEBInfoWriterToolBase::Input& input) const {
  // Create output PEBInfo starting from the static extra PEBInfo
  PEBInfo pebi = m_extraPebInfo;

  ATH_MSG_DEBUG("Processing RoI " << **(input.roiEL));
  float eta = (*input.roiEL)->eta();
  float etaMin = eta - m_etaWidth;
  float etaMax = eta + m_etaWidth;
  // Stop further execution if RoI is entirely outside the max |eta| range
  if (etaMin > m_etaEdge || etaMax < -m_etaEdge) {
    ATH_MSG_DEBUG("The eta range (" << etaMin << ", " << etaMax << ") is outside |eta|<" << m_etaEdge
                  << " - skipping this RoI");
    return pebi;
  }
  // Restrict the eta range
  etaMin = std::max(-m_etaEdge.value(), etaMin);
  etaMax = std::min( m_etaEdge.value(), etaMin);

  float phi = (*input.roiEL)->phi();
  float phiMin = CxxUtils::wrapToPi(phi - m_phiWidth); // range (-pi, pi)
  float phiMax = CxxUtils::wrapToPi(phi + m_phiWidth); // range (-pi, pi)

  TrigRoiDescriptor roiForPEB(eta, etaMin, etaMax, phi, phiMin, phiMax);

  for (const auto& tool : m_regionSelectorTools) {
    std::vector<uint32_t> detROBs;
    tool->ROBIDList(roiForPEB, detROBs);
    pebi.robs.insert(detROBs.begin(),detROBs.end());
  }

  ATH_MSG_DEBUG("Created PEBInfo = " << pebi);
  return pebi;
}
