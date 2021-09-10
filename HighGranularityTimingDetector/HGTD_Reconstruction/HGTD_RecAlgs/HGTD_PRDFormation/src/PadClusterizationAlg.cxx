/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_PRDFormation/src/PadClusterizationAlg.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date September, 2021
 * @brief
 */

#include "HGTD_PRDFormation/PadClusterizationAlg.h"

#include "HGTD_Identifier/HGTD_ID.h"
#include "StoreGate/WriteHandle.h"

namespace HGTD {

PadClusterizationAlg::PadClusterizationAlg(const std::string& name,
                                           ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator),
      m_clusterization_tool("HGTD::SinglePadClusterTool"),
      m_hgtd_idhelper(nullptr) {
  declareProperty("ClusterizationTool", m_clusterization_tool);
  ///////
  // FIXME remove these two lines in master
  declareProperty("RDOContainerName", m_rdo_rh_key = "HGTD_RDOs");
  declareProperty("PRDContainerName", m_prd_wh_key = "HGTD_Clusters");
  ///////
}

StatusCode PadClusterizationAlg::initialize() {

  ATH_CHECK(m_clusterization_tool.retrieve());

  ATH_CHECK(m_rdo_rh_key.initialize());
  ATH_CHECK(m_prd_wh_key.initialize());

  ATH_CHECK(detStore()->retrieve(m_hgtd_idhelper, "HGTD_ID"));

  return StatusCode::SUCCESS;
}

StatusCode PadClusterizationAlg::execute() {
  // retrieve the RDOs
  SG::ReadHandle<HGTD::HGTD_RDOContainer> rdo_container_handle(m_rdo_rh_key);

  const HGTD::HGTD_RDOContainer* rdo_container = rdo_container_handle.cptr();

  // register the PRD container in storegate
  SG::WriteHandle<HGTD::HGTD_ClusterContainer> prd_container_handle(
      m_prd_wh_key);

  ATH_CHECK(
      prd_container_handle.record(std::make_unique<HGTD::HGTD_ClusterContainer>(
          m_hgtd_idhelper->wafer_hash_max())));

  HGTD::HGTD_ClusterContainer* locp_prd_container = prd_container_handle.ptr();

  // Transform the RDOs collection by collection
  for (const auto& rdo_collection : *rdo_container) {
    if (rdo_collection->empty()) {
      continue;
    }

    std::unique_ptr<HGTD::HGTD_ClusterCollection> prd_collection =
        m_clusterization_tool->clusterize(*rdo_collection);

    // can happen if some channels are masked, not really relevant currently
    if (prd_collection->empty()) {
      continue;
    }

    if (locp_prd_container
            ->addCollection(prd_collection.release(),
                            prd_collection->identifyHash())
            .isFailure()) {
      ATH_MSG_WARNING("Could not add ClusterCollection to container!");
    }
  } // END LOOP over container

  return StatusCode::SUCCESS;
}

} // namespace HGTD
