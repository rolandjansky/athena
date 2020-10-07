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

  ATH_CHECK(m_regionSelector.retrieve());

  m_extraPebInfo.robs.insert(m_extraROBs.begin(), m_extraROBs.end());
  m_extraPebInfo.subdets.insert(m_extraSubDets.begin(), m_extraSubDets.end());
  ATH_MSG_DEBUG("Extra PEBInfo attached to every passed event: " << m_extraPebInfo);

  // Ugly solution - need to translate strings into enums. Wouldn't be needed if DETID enum was accessible from python.

  const std::unordered_map<std::string_view,DETID> detNameDict = {
    {"LAR",     DETID::LAR},
    {"TTEM",    DETID::TTEM},
    {"TTHEC",   DETID::TTHEC},
    {"TILE",    DETID::TILE},
    {"MDT",     DETID::MDT},
    {"RPC",     DETID::RPC},
    {"TGC",     DETID::TGC},
    {"CSC",     DETID::CSC},
    {"FCALEM",  DETID::FCALEM},
    {"FCALHAD", DETID::FCALHAD},
    {"FTK",     DETID::FTK},
    {"MM",      DETID::MM},
    {"STGC",    DETID::STGC},
  };

  const std::unordered_map< std::string_view, ToolHandle<IRegSelTool> > detTools = {
    { "PIXEL",   m_regionSelector_pix },
    { "SCT",     m_regionSelector_sct },
    { "TRT",     m_regionSelector_trt }
  };

  for (std::string_view name : m_detNames) {
    if (name=="All") {
      for (const auto& p : detNameDict) m_dets.insert(p.second);
      for (const auto& p : detTools)    m_tools.insert(p.second);
      break;
    }

    const auto itt = detTools.find(name);
    if ( itt!=detTools.cend() ) {
      ATH_MSG_DEBUG("The detector name " << name << " being inserted from the RegSelTools database");
      m_tools.insert(itt->second);
    }
    else {  
      ATH_MSG_DEBUG("The detector name " << name << " not in the RegSelTools database - trying RegSelEnum database");
      const auto it = detNameDict.find(name);
      if (it==detNameDict.cend()) {
	ATH_MSG_ERROR("The detector name " << name << " cannot be translated into RegSelEnum DETID");
	return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG("Adding " << name << "=" << it->second << " to detector list");
      m_dets.insert(it->second);
    }
  }

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

  for ( auto tool : m_tools ) { 
    std::vector<uint32_t> detROBs;
    tool->ROBIDList( roiForPEB, detROBs);
    pebi.robs.insert(detROBs.begin(),detROBs.end());
  }

  for (const DETID detid : m_dets) {
    std::vector<uint32_t> detROBs;
    m_regionSelector->DetROBIDListUint(detid, roiForPEB, detROBs);
    pebi.robs.insert(detROBs.begin(),detROBs.end());
  }

  ATH_MSG_DEBUG("Created PEBInfo = " << pebi);
  return pebi;
}
