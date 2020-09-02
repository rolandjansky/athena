/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaSwTool.h"

#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloUtils/CaloLayerCalculator.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "GaudiKernel/ListItem.h"

#include <algorithm>
#include <cmath>

//  END OF HEADER FILES INCLUDE

/////////////////////////////////////////////////////////////////

//  CONSTRUCTOR:

egammaSwTool::egammaSwTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent)
  : AthAlgTool(type, name, parent)
{

  // declare interface
  declareInterface<IegammaSwTool>(this);
}

StatusCode
egammaSwTool::initialize()
{
  ATH_MSG_DEBUG(" Initializing egammaSwTool");

  ATH_CHECK(m_clusterCorrectionPointersEle35.retrieve());
  ATH_CHECK(m_clusterCorrectionPointersEle37.retrieve());
  ATH_CHECK(m_clusterCorrectionPointersEle55.retrieve());
  //
  ATH_CHECK(m_clusterCorrectionPointersGam35.retrieve());
  ATH_CHECK(m_clusterCorrectionPointersGam37.retrieve());
  ATH_CHECK(m_clusterCorrectionPointersGam55.retrieve());
  //
  ATH_CHECK(m_clusterCorrectionPointersEconv35.retrieve());
  ATH_CHECK(m_clusterCorrectionPointersEconv37.retrieve());
  ATH_CHECK(m_clusterCorrectionPointersEconv55.retrieve());
  // SuperCluster
  ATH_CHECK(m_clusterCorrectionPointersSuperClusterEle35.retrieve());
  ATH_CHECK(m_clusterCorrectionPointersSuperClusterEle37.retrieve());
  ATH_CHECK(m_clusterCorrectionPointersSuperClusterEle55.retrieve());
  //
  ATH_CHECK(m_clusterCorrectionPointersSuperClusterGam35.retrieve());
  ATH_CHECK(m_clusterCorrectionPointersSuperClusterGam37.retrieve());
  ATH_CHECK(m_clusterCorrectionPointersSuperClusterGam55.retrieve());
  //
  ATH_CHECK(m_clusterCorrectionPointersSuperClusterEconv35.retrieve());
  ATH_CHECK(m_clusterCorrectionPointersSuperClusterEconv37.retrieve());
  ATH_CHECK(m_clusterCorrectionPointersSuperClusterEconv55.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode
egammaSwTool::execute(const EventContext& ctx, xAOD::CaloCluster* cluster) const
{
  ATH_MSG_DEBUG("Executing egammaSwTool");

  // protection against bad clusters
  if (cluster == nullptr)
    return StatusCode::SUCCESS;

  xAOD::CaloCluster::ClusterSize requestedSize = cluster->clusterSize();
  switch (requestedSize) {
    case xAOD::CaloCluster::SW_55ele:
      ATH_CHECK(processTools(ctx, m_clusterCorrectionPointersEle55, cluster));
      break;
    case xAOD::CaloCluster::SW_35ele:
      ATH_CHECK(processTools(ctx, m_clusterCorrectionPointersEle35, cluster));
      break;
    case xAOD::CaloCluster::SW_37ele:
      ATH_CHECK(processTools(ctx, m_clusterCorrectionPointersEle37, cluster));
      break;
    case xAOD::CaloCluster::SW_35gam:
      ATH_CHECK(processTools(ctx, m_clusterCorrectionPointersGam35, cluster));
      break;
    case xAOD::CaloCluster::SW_55gam:
      ATH_CHECK(processTools(ctx, m_clusterCorrectionPointersGam55, cluster));
      break;
    case xAOD::CaloCluster::SW_37gam:
      ATH_CHECK(processTools(ctx, m_clusterCorrectionPointersGam37, cluster));
      break;
    case xAOD::CaloCluster::SW_55Econv:
      ATH_CHECK(processTools(ctx, m_clusterCorrectionPointersEconv55, cluster));
      break;
    case xAOD::CaloCluster::SW_35Econv:
      ATH_CHECK(processTools(ctx, m_clusterCorrectionPointersEconv35, cluster));
      break;
    case xAOD::CaloCluster::SW_37Econv:
      ATH_CHECK(processTools(ctx, m_clusterCorrectionPointersEconv37, cluster));
      break;
    default:
      ATH_MSG_DEBUG(
        "Inexisting cluster type and calibration requested: " << requestedSize);
      return StatusCode::FAILURE;
      break;
  }
  return StatusCode::SUCCESS;
}
// ==============================================================
// ATHENA EXECUTE METHOD for superClusters

StatusCode
egammaSwTool::execute(const EventContext& ctx,
                      xAOD::CaloCluster* cluster,
                      xAOD::EgammaParameters::EgammaType egType,
                      bool isBarrel) const
{
  ATH_MSG_DEBUG("Executing egammaSwTool");

  // protection against bad clusters
  if (cluster == nullptr) {
    return StatusCode::SUCCESS;
  }

  if (isBarrel) {
    switch (egType) {
      case xAOD::EgammaParameters::electron: {
        ATH_MSG_DEBUG("correction for barrel electron");
        ATH_CHECK(processTools(
          ctx, m_clusterCorrectionPointersSuperClusterEle37, cluster));
        break;
      }
      case xAOD::EgammaParameters::unconvertedPhoton: {
        ATH_MSG_DEBUG("correction for barrel unconverted");
        ATH_CHECK(processTools(
          ctx, m_clusterCorrectionPointersSuperClusterGam37, cluster));
        break;
      }
      case xAOD::EgammaParameters::convertedPhoton: {
        ATH_MSG_DEBUG("correction for barrel converted");
        ATH_CHECK(processTools(
          ctx, m_clusterCorrectionPointersSuperClusterEconv37, cluster));
        break;
      }
      default: {
        ATH_MSG_DEBUG("Inexisting correction requested for egType: "
                      << egType << " isBarrel: " << isBarrel);
        return StatusCode::FAILURE;
        break;
      }
    }
  } else {
    switch (egType) {
      case xAOD::EgammaParameters::electron: {
        ATH_MSG_DEBUG("correction for endcap electron");
        ATH_CHECK(processTools(
          ctx, m_clusterCorrectionPointersSuperClusterEle55, cluster));
        break;
      }
      case xAOD::EgammaParameters::unconvertedPhoton: {
        ATH_MSG_DEBUG("correction for endcap unconverted");
        ATH_CHECK(processTools(
          ctx, m_clusterCorrectionPointersSuperClusterGam55, cluster));
        break;
      }
      case xAOD::EgammaParameters::convertedPhoton: {
        ATH_MSG_DEBUG("correction for endcap converted");
        ATH_CHECK(processTools(
          ctx, m_clusterCorrectionPointersSuperClusterEconv55, cluster));
        break;
      }
      default: {
        ATH_MSG_DEBUG("Inexisting correction requested for egType: "
                      << egType << " isBarrel: " << isBarrel);
        return StatusCode::FAILURE;
        break;
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode
egammaSwTool::processTools(const EventContext& ctx,
                           const ToolHandleArray<CaloClusterProcessor>& tools,
                           xAOD::CaloCluster* cluster) const
{
  auto firstTool = tools.begin();
  auto lastTool = tools.end();
  // loop tools
  for (; firstTool != lastTool; ++firstTool) {
    StatusCode processCheck = (*firstTool)->execute(ctx, cluster);
    ATH_MSG_DEBUG("Tool " << (*firstTool)->name() << " executing  ");
    if (processCheck.isFailure()) {
      ATH_MSG_ERROR("Cluster corrections failed!");
    }
  }
  return StatusCode::SUCCESS;
}
