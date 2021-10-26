/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaRecBuilder.h"

#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "CaloUtils/CaloClusterStoreHelper.h"
#include "egammaInterfaces/IEMConversionBuilder.h"
#include "egammaInterfaces/IEMTrackMatchBuilder.h"
#include "egammaInterfaces/IegammaCheckEnergyDepositTool.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include <vector>
#include <algorithm>
#include <cmath>

egammaRecBuilder::egammaRecBuilder(const std::string& name,
                                   ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{}

StatusCode
egammaRecBuilder::initialize()
{
  // First the data handle keys
  ATH_CHECK(m_inputClusterContainerKey.initialize());
  ATH_CHECK(m_egammaRecContainerKey.initialize());
  ATH_CHECK(m_trackMatchedEgammaRecs.initialize(m_doTrackMatchedView));
  // retrieve track match builder
  CHECK(RetrieveEMTrackMatchBuilder());
  // retrieve conversion builder
  CHECK(RetrieveEMConversionBuilder());
  return StatusCode::SUCCESS;
}

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

  return StatusCode::SUCCESS;
}

StatusCode
egammaRecBuilder::RetrieveEMConversionBuilder()
{
  // retrieve EMConversionBuilder tool
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
  return StatusCode::SUCCESS;
}

StatusCode
egammaRecBuilder::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("Executing egammaRecBuilder");

  SG::ReadHandle<xAOD::CaloClusterContainer> clusters(
    m_inputClusterContainerKey, ctx);

  // validity check is only really needed for serial running. Remove when MT is
  // only way.
  if (!clusters.isValid()) {
    ATH_MSG_ERROR("Could not retrieve cluster container:"
                  << m_inputClusterContainerKey.key());
    return StatusCode::FAILURE;
  }

  SG::WriteHandle<EgammaRecContainer> egammaRecs(m_egammaRecContainerKey, ctx);
  ATH_CHECK(egammaRecs.record(std::make_unique<EgammaRecContainer>()));

  // one egamma Rec objects per cluster
  const size_t nClusters = clusters->size();
  egammaRecs->reserve(nClusters);
  for (size_t i = 0; i < nClusters; ++i) {
    const ElementLink<xAOD::CaloClusterContainer> clusterLink(
      *clusters, i, ctx);
    const std::vector<ElementLink<xAOD::CaloClusterContainer>>
      clusterLinkVector{ clusterLink };
    auto egRec = std::make_unique<egammaRec>();
    egRec->setCaloClusters(clusterLinkVector);
    egammaRecs->push_back(std::move(egRec));
  }

  // Append track Matching information if requested
  if (m_doTrackMatching) {
    ATH_CHECK(m_trackMatchBuilder->executeRec(ctx, egammaRecs.ptr()));
  }

  // Append conversion matching information if requested
  if (m_doConversions) {
    for (auto egRec : *egammaRecs) {
      ATH_CHECK(m_conversionBuilder->executeRec(ctx, egRec));
    }
  }

  // create a view of the track matched egRecs if requested
  if (m_doTrackMatchedView) {
    SG::WriteHandle<ConstDataVector<EgammaRecContainer>> trackMatchedEgammaRecs(
      m_trackMatchedEgammaRecs, ctx);

    auto viewCopy =
      std::make_unique<ConstDataVector<EgammaRecContainer>>(SG::VIEW_ELEMENTS);

    if (m_doTrackMatching) {
      for (const egammaRec* eg : *egammaRecs) {
        if (eg->getNumberOfTrackParticles() > 0) {
          viewCopy->push_back(eg);
        }
      }
    }
    ATH_CHECK(trackMatchedEgammaRecs.record(std::move(viewCopy)));
  }

  return StatusCode::SUCCESS;
}
