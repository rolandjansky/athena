/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @file PFSumFex.cxx
 *
 * Implementation of the PFlow fex class
 * @author Jon Burr
 *****************************************************************************/

#include "PFSumFex.h"
#include "TrigEFMissingET/METComponent.h"
#include <array>

namespace HLT { namespace MET {

  PFSumFex::PFSumFex(const std::string& name, ISvcLocator* pSvcLocator) :
    FexBase(name, pSvcLocator)
  {}

  StatusCode PFSumFex::initialize()
  {
    CHECK( m_neutralPFOKey.initialize() );
    CHECK( m_chargedPFOKey.initialize() );
    return initializeBase({"NeutralPFOs", "ChargedHSPFOs", "ChargedPUPFOs"});
  }

  StatusCode PFSumFex::fillMET(
      xAOD::TrigMissingET& met,
      const EventContext& context,
      MonGroupBuilder&) const
  {
    auto charged = SG::makeHandle(m_chargedPFOKey, context);
      if (!charged.isValid())
      {
        ATH_MSG_ERROR("Failed to retrieve " << m_chargedPFOKey);
        return StatusCode::FAILURE;
      }
    auto neutral = SG::makeHandle(m_neutralPFOKey, context);
      if (!neutral.isValid())
      {
        ATH_MSG_ERROR("Failed to retrieve " << m_neutralPFOKey);
        return StatusCode::FAILURE;
      }

    std::array<METComponent, 3> pfoSums;
    for (const xAOD::PFO* ipfo : *neutral)
      // 0 is the 'NeutralPFOs' component
      pfoSums.at(0) += *ipfo;
    const static SG::AuxElement::ConstAccessor<char> PVMatchedAcc("matchedToPV");
    for (const xAOD::PFO* ipfo : *charged)
      // 1 is the 'ChargedHSPFOs' component, 2 is the 'ChargedPUPFOs' component
      (PVMatchedAcc(*ipfo) ? pfoSums.at(1) : pfoSums.at(2) ) += *ipfo;

    // Sum together the components
    METComponent total = pfoSums.at(0) + pfoSums.at(1);
    if (!m_chargedOnlyFromPV)
      total += pfoSums.at(2);
    total.fillMET(met);
    for (std::size_t ii = 0; ii < 3; ++ii)
      pfoSums.at(ii).fillMETComponent(ii, met);
    return StatusCode::SUCCESS;
  }
} } //> end namespace HLT::MET
