/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_RecAlgs/src/PadClusterizationAlg.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @brief
 */

#include "HGTD_RecAlgs/PadClusterizationAlg.h"

#include "HGTD_Identifier/HGTD_ID.h"
#include "StoreGate/WriteHandle.h"

namespace HGTD {

PadClusterizationAlg::PadClusterizationAlg(const std::string& name,
                                           ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode PadClusterizationAlg::initialize() {

  ATH_CHECK(m_clusterization_tool.retrieve());

  ATH_CHECK(m_rdo_rh_key.initialize());
  ATH_CHECK(m_prd_wh_key.initialize());

  ATH_CHECK(detStore()->retrieve(m_hgtd_idhelper, "HGTD_ID"));

  return StatusCode::SUCCESS;
}

StatusCode PadClusterizationAlg::execute(const EventContext& ctx) const {
  // retrieve the RDOs
  SG::ReadHandle<HGTD_RDO_Container> rdo_container_handle(m_rdo_rh_key, ctx);

  const HGTD_RDO_Container* rdo_container = rdo_container_handle.cptr();

  if (not rdo_container) {
    //assume this is fast digi input and ignore
    return StatusCode::SUCCESS;
  }

  // register the PRD container in storegate
  SG::WriteHandle<HGTD_ClusterContainer> prd_container_handle(
      m_prd_wh_key, ctx);

  ATH_CHECK(
      prd_container_handle.record(std::make_unique<HGTD_ClusterContainer>(
          m_hgtd_idhelper->wafer_hash_max())));

  HGTD_ClusterContainer* locp_prd_container = prd_container_handle.ptr();

  // Transform the RDOs collection by collection
  for (const auto& rdo_collection : *rdo_container) {
    if (rdo_collection->empty()) {
      continue;
    }

    std::unique_ptr<HGTD_ClusterCollection> prd_collection =
        m_clusterization_tool->clusterize(*rdo_collection);

    // can happen if some channels are masked, not really relevant currently
    if (prd_collection->empty()) {
      continue;
    }
    const IdentifierHash identifyHash{prd_collection->identifyHash()};
    if (locp_prd_container
            ->addCollection(prd_collection.release(),identifyHash)
            .isFailure()) {
      ATH_MSG_WARNING("Could not add ClusterCollection to container!");
    }
  } // END LOOP over container

  return StatusCode::SUCCESS;
}

} // namespace HGTD
