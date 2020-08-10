/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaRecBuilder.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "CaloUtils/CaloClusterStoreHelper.h"
#include "egammaInterfaces/IEMConversionBuilder.h"
#include "egammaInterfaces/IEMTrackMatchBuilder.h"
#include "egammaInterfaces/IegammaCheckEnergyDepositTool.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "egammaUtils/egammaDuplicateRemoval.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

// INCLUDE GAUDI HEADER FILES:
#include <algorithm>
#include <cmath>

egammaRecBuilder::egammaRecBuilder(const std::string& name,
                                   ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{}

// =================================================================
StatusCode
egammaRecBuilder::initialize()
{
  // initialize method

  ATH_MSG_DEBUG("Initializing egammaRecBuilder");

  // First the data handle keys
  ATH_CHECK(m_inputTopoClusterContainerKey.initialize());
  ATH_CHECK(m_egammaRecContainerKey.initialize());

  //////////////////////////////////////////////////
  // retrieve track match builder
  CHECK(RetrieveEMTrackMatchBuilder());
  // retrieve conversion builder
  CHECK(RetrieveEMConversionBuilder());

  ATH_MSG_DEBUG("Initialization completed successfully");
  return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode
egammaRecBuilder::RetrieveEMTrackMatchBuilder()
{
  // retrieve EMTrackMatchBuilder tool
  if (!m_doTrackMatching) {
    m_trackMatchBuilder.disable();
    return StatusCode::SUCCESS;
  }

  if (m_trackMatchBuilder.empty()) {
    ATH_MSG_ERROR(
      "EMTrackMatchBuilder is empty, but track matching is enabled");
    return StatusCode::FAILURE;
  }

  if (m_trackMatchBuilder.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve " << m_trackMatchBuilder);
    return StatusCode::FAILURE;
  }
    ATH_MSG_DEBUG("Retrieved Tool " << m_trackMatchBuilder);

  return StatusCode::SUCCESS;
}

StatusCode
egammaRecBuilder::RetrieveEMConversionBuilder()
{
  //
  // retrieve EMConversionBuilder tool
  //
  if (!m_doConversions) {
    m_conversionBuilder.disable();
    return StatusCode::SUCCESS;
  }
  if (m_conversionBuilder.empty()) {
    ATH_MSG_ERROR("EMConversionBuilder is empty");
    return StatusCode::FAILURE;
  }
  if (m_conversionBuilder.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve " << m_conversionBuilder);
    return StatusCode::FAILURE;
  }
    ATH_MSG_DEBUG("Retrieved Tool " << m_conversionBuilder);

  return StatusCode::SUCCESS;
}

StatusCode
egammaRecBuilder::finalize()
{
  // finalize method
  return StatusCode::SUCCESS;
}

StatusCode
egammaRecBuilder::execute_r(const EventContext& ctx) const
{
  // athena execute method

  ATH_MSG_DEBUG("Executing egammaRecBuilder");

  SG::ReadHandle<xAOD::CaloClusterContainer> topoclusters(
    m_inputTopoClusterContainerKey, ctx);

  // validity check is only really needed for serial running. Remove when MT is
  // only way.
  if (!topoclusters.isValid()) {
    ATH_MSG_ERROR("Could not retrieve cluster container:"
                  << m_inputTopoClusterContainerKey.key());
    return StatusCode::FAILURE;
  }

  // Build the initial egamma Rec objects for all copied Topo Clusters
  SG::WriteHandle<EgammaRecContainer> egammaRecs(m_egammaRecContainerKey, ctx);
  ATH_CHECK(egammaRecs.record(std::make_unique<EgammaRecContainer>()));
  const size_t nTopo = topoclusters->size();
  egammaRecs->reserve(nTopo);
  for (size_t i(0); i < nTopo; i++) {
    const ElementLink<xAOD::CaloClusterContainer> clusterLink(*topoclusters, i);
    const std::vector<ElementLink<xAOD::CaloClusterContainer>> ClusterLink{
      clusterLink
    };
    auto egRec = std::make_unique<egammaRec>();
    egRec->setCaloClusters(ClusterLink);
    egammaRecs->push_back(std::move(egRec));
  }
  /// Append track Matching information
  if (m_doTrackMatching) {
    ATH_CHECK(m_trackMatchBuilder->executeRec(ctx, egammaRecs.ptr()));
  }
  // Do the conversion matching
  if (m_doConversions) {
    for (auto egRec : *egammaRecs) {
      ATH_CHECK(m_conversionBuilder->executeRec(ctx, egRec));
    }
  }
  return StatusCode::SUCCESS;
}
