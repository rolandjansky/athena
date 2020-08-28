/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @file TCFex.cxx
 *
 * Implementation of the topocluster fex class
 * @author Jon Burr
 *****************************************************************************/

#include "TCFex.h"
#include "TrigEFMissingET/METComponent.h"
#include <array>

namespace {
  bool isBarrel(const xAOD::CaloCluster* iclus,
                xAOD::CaloCluster::State state) {
    return fabs(iclus->eta(state) ) < 1.5;
  }
}

namespace HLT { namespace MET {
  TCFex::TCFex(const std::string& name, ISvcLocator* pSvcLocator) :
    FexBase(name, pSvcLocator)
  {}

  StatusCode TCFex::initialize()
  {
    CHECK( m_clusterKey.initialize() );
    return initializeBase({"TCB1", "TCB2", "TCE1", "TCE2"});
  }

  StatusCode TCFex::fillMET(
      xAOD::TrigMissingET& met,
      const EventContext& context,
      MonGroupBuilder&) const
  {
    // Retrieve the inputs
    auto clusters = SG::makeHandle(m_clusterKey, context);
    if (!clusters.isValid())
    {
      ATH_MSG_ERROR("Failed to retrieve " << m_clusterKey);
      return StatusCode::FAILURE;
    }

    auto state = m_useUncalibrated ? 
      xAOD::CaloCluster::UNCALIBRATED : 
      xAOD::CaloCluster::CALIBRATED;

    // Prepare the output values
    std::array<METComponent, 4> tcSums;
    for (const xAOD::CaloCluster* iclus : *clusters) {
      // Pick the right component
      std::size_t componentIdx = 0;
      if (iclus->eta() < 0)
        ++componentIdx;
      if (!isBarrel(iclus, state) )
        componentIdx += 2;
      tcSums.at(componentIdx) += SignedKinematics::fromEnergyEtaPhiM(
          iclus->e(state), iclus->eta(state), iclus->phi(state),
          m_useUncalibrated ? iclus->rawM() : iclus->calM() );
    }
    // Save the total
    std::accumulate(tcSums.begin(), tcSums.end(), METComponent{}).fillMET(met);

    // Save each component
    for (std::size_t ii = 0; ii < 4; ++ii)
      tcSums.at(ii).fillMETComponent(ii, met);

    return StatusCode::SUCCESS;
  }
} } //> end namespace HLT::MET
