/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:      TrigCaloClusterCalibratorMT
 * PACKAGE:   Trigger/TrigAlgorithms/TrigCaloRec
 *
 * AUTHOR:    Jon Burr
 * CREATED:   2020/07/10
 *
 * Shallow copy an existing cluster container and apply cluster processors to
 * it. Largely copied from the TrigCaloClusterMakerMT.
 *********************************************************************/

#include "AthenaMonitoringKernel/Monitored.h"
#include "TrigCaloClusterCalibratorMT.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODCore/ShallowAuxContainer.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include <tuple>

TrigCaloClusterCalibratorMT::TrigCaloClusterCalibratorMT(
    const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
}

TrigCaloClusterCalibratorMT::~TrigCaloClusterCalibratorMT()
{
}

StatusCode TrigCaloClusterCalibratorMT::initialize()
{
  ATH_MSG_INFO("Initialise " << name() );

  if (!m_monTool.empty() )
    ATH_CHECK(m_monTool.retrieve() );
  else
    ATH_MSG_INFO("No monitoring tool configured");

  ATH_CHECK( m_clusterCorrections.retrieve() );
  ATH_CHECK( m_inputClustersKey.initialize() );
  ATH_CHECK( m_outputClustersKey.initialize() );
  ATH_CHECK( m_outputCellLinksKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode TrigCaloClusterCalibratorMT::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("Running " << name() );
  auto time_corr = Monitored::Timer("TIME_ClustCorr");

  // Do the shallow copying
  SG::WriteHandle<xAOD::CaloClusterContainer> outputClusters(m_outputClustersKey, ctx);
  {
    // Make a temporary scope - this means that the unique pointers die at the
    // end of the scope (so don't hang around after the move call)
    SG::ReadHandle<xAOD::CaloClusterContainer> inputClusters(m_inputClustersKey, ctx);
    std::pair<xAOD::CaloClusterContainer*, xAOD::ShallowAuxContainer*> copyPair =
      xAOD::shallowCopyContainer(*inputClusters);
    // Show that we're taking ownership explicitly
    std::unique_ptr<xAOD::CaloClusterContainer> clusters{copyPair.first};
    std::unique_ptr<xAOD::ShallowAuxContainer> clustersAux{copyPair.second};
    // And record
    ATH_CHECK( outputClusters.record(std::move(clusters), std::move(clustersAux)));
    // We also need to copy across the cell links information.
    // To explain: the non-const cell iteration methods use an internal pointer,
    // which is only finalized into an element link into the persistent
    // container later. Therefore while we have a modifiable cluster, it has to
    // have this local pointer present...
    for (auto itrPair = std::make_pair(inputClusters->begin(), outputClusters->begin());
         itrPair != std::make_pair(inputClusters->end(), outputClusters->end());
         ++itrPair.first, ++itrPair.second) {
      const CaloClusterCellLink* inputLinks = (**itrPair.first).getCellLinks();
      if (!inputLinks) {
        ATH_MSG_ERROR("Failed to read the cell links from the input clusters!");
        return StatusCode::FAILURE;
      }
      (**itrPair.second).addCellLink(std::make_unique<CaloClusterCellLink>(*inputLinks));
    }
  }

  time_corr.start();
  for (const ToolHandle<CaloClusterProcessor>& clcorr : m_clusterCorrections) {
    for (xAOD::CaloCluster* cl : *outputClusters) {
      if (!m_isSW.value() ||
          (std::abs(cl->eta0()) < 1.45  && clcorr->name().find("37") != std::string::npos) ||
          (std::abs(cl->eta0()) >= 1.45 && clcorr->name().find("55") != std::string::npos) ) {
        ATH_CHECK(clcorr->execute(ctx, cl));
        ATH_MSG_VERBOSE("Executed correction tool " << clcorr->name());
      }
    }
  }
  time_corr.stop();

  // Now we also have to make the cell links persistent
  SG::WriteHandle<CaloClusterCellLinkContainer> cellLinks(m_outputCellLinksKey, ctx);
  ATH_CHECK(CaloClusterStoreHelper::finalizeClusters(cellLinks, outputClusters.ptr()));
  // After this is done, the non-const cell iteration functions will segfault!
  // This means we really shouldn't let anyone retrieve this as a non-const
  // container...
  ATH_CHECK( outputClusters.setConst() );

  // fill monitored containers
  // Only monitor kinematic quantities which the calibrations may have changed
  Monitored::Group(m_monTool,
      time_corr,
      Monitored::Collection("Et", *outputClusters, &xAOD::CaloCluster::et),
      Monitored::Collection("Phi", *outputClusters, &xAOD::CaloCluster::calPhi),
      Monitored::Collection("Eta", *outputClusters, &xAOD::CaloCluster::calEta)).fill();

  // Add REGTEST entries
  if (!outputClusters->empty() ) {
    ATH_MSG_DEBUG(" REGTEST: Last Cluster Et  = " << outputClusters->back()->et());
    ATH_MSG_DEBUG(" REGTEST: Last Cluster eta = " << outputClusters->back()->eta());
    ATH_MSG_DEBUG(" REGTEST: Last Cluster phi = " << outputClusters->back()->phi());
  }

  return StatusCode::SUCCESS;
}
