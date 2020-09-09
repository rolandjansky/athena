/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @file MHTFex.cxx
 *
 * Implementation of the mht fex class
 * @author Jon Burr
 *****************************************************************************/

#include "MHTFex.h"
#include "TrigEFMissingET/METComponent.h"
#include <array>

namespace HLT { namespace MET {
  MHTFex::MHTFex(const std::string& name, ISvcLocator* pSvcLocator) :
    FexBase(name, pSvcLocator)
  {}

  StatusCode MHTFex::initialize()
  {
    CHECK( m_jetKey.initialize() );
    return initializeBase({"JETB1", "JETB2", "JETE1", "JETE2"});
  }

  StatusCode MHTFex::fillMET(
      xAOD::TrigMissingET& met,
      const EventContext& context,
      MonGroupBuilder&) const
  {
    // Retrieve the inputs
    auto jets = SG::makeHandle(m_jetKey, context);
    if (!jets.isValid())
    {
      ATH_MSG_ERROR("Failed to retrieve " << m_jetKey);
      return StatusCode::FAILURE;
    }

    // Prepare the output values
    std::array<METComponent, 4> mhtSums;
    for (const xAOD::Jet* ijet : *jets) {
      // Apply the pT selection
      if (ijet->pt() < (isCentral(ijet) ? m_centralPtCut : m_forwardPtCut) )
        continue;
      // Get the right MET component
      std::size_t componentIdx = 0;
      if (ijet->eta() < 0)
        ++componentIdx;
      if (!isCentral(ijet) )
        componentIdx += 2;
      mhtSums.at(componentIdx) += ijet->p4();
    }

    // Save the full sum
    std::accumulate(mhtSums.begin(), mhtSums.end(), METComponent{}).fillMET(met);
    // Save each component
    for (std::size_t ii = 0; ii < 4; ++ii)
      mhtSums.at(ii).fillMETComponent(ii, met);

    return StatusCode::SUCCESS;
  }
} } //> end class HLT::MET
