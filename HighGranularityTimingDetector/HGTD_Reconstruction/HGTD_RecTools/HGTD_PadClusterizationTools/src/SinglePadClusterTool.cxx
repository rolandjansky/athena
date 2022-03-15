/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_ClusterMakerTools/src/SinglePadClusterTool.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 *
 * @brief
 *
 */

#include "SinglePadClusterTool.h"

#include "HGTD_ReadoutGeometry/HGTD_ModuleDesign.h"

namespace HGTD {

SinglePadClusterTool::SinglePadClusterTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent)
    : base_class(type, name, parent) {}

StatusCode SinglePadClusterTool::initialize() {

  ATH_CHECK(AlgTool::initialize());

  ATH_CHECK(m_cluster_maker.retrieve());

  ATH_CHECK(detStore()->retrieve(m_hgtd_det_mgr, "HGTD"));

  return StatusCode::SUCCESS;
}

std::unique_ptr<HGTD_ClusterCollection>
SinglePadClusterTool::clusterize(const HGTD_RDOCollection& rdo_coll) const {

  Identifier identifier = rdo_coll.identify();
  IdentifierHash id_hash = rdo_coll.identifierHash();

  std::unique_ptr<HGTD_ClusterCollection> cluster_collection =
      std::make_unique<HGTD_ClusterCollection>(id_hash);
  cluster_collection->setIdentifier(identifier);
  cluster_collection->reserve(rdo_coll.size());

  InDetDD::HGTD_DetectorElement* element =
      m_hgtd_det_mgr->getDetectorElement(identifier);

  for (const auto& rdo : rdo_coll) {

    Identifier rdo_id = rdo->identify();
    ATH_MSG_DEBUG("Using RDO: " << rdo_id);

    // get the local position from the detector design
    InDetDD::SiCellId si_cell_id = element->cellIdFromIdentifier(rdo_id);
    const InDetDD::HGTD_ModuleDesign& det_design = element->design();
    InDetDD::SiLocalPosition si_pos =
        det_design.localPositionOfCell(si_cell_id);
    Amg::Vector2D loc_pos(si_pos.xPhi(), si_pos.xEta());
    ATH_MSG_DEBUG("Local position: x=" << loc_pos.x() << " y=" << loc_pos.y());

    // a cluster consists only of the pad itself
    std::vector<Identifier> rdo_list = {rdo_id};

    // From SiWidth header:
    // Constructor with parameters: <col, row> in units of RDOs (so should be
    // int),
    //                              <phiR width in mm, Z width in mm>
    // NOTE: without a clustering of the pads, the width of each "cluster" is 1
    // in col and row direction
    int col_width = 1;
    int row_width = 1;
    //FIXME there is a bug in the detector design! should not be hardcoded here
    // double eta_width = det_design.etaPitch();
    // double phi_width = det_design.phiPitch();
    double eta_width = 1.3;
    double phi_width = 1.3;
    ATH_MSG_DEBUG("eta_width=" << eta_width << " phi_width=" << phi_width);

    InDet::SiWidth si_width(Amg::Vector2D(col_width, row_width),
                            Amg::Vector2D(phi_width, eta_width));

    // retrieve the time of arrival
    float time_of_arrival = rdo->getTOA();

    // this would hold the vector of all cluster components. Only size 1 if RDO
    // gets transformed to cluster directly
    std::vector<int> time_over_threshold = {static_cast<int>(rdo->getTOT())};

    std::unique_ptr<HGTD_Cluster> cluster =
        m_cluster_maker->createCluster(rdo_id, loc_pos, rdo_list, si_width,
                                       element, time_of_arrival,
                                       time_over_threshold);
    cluster->setHashAndIndex(cluster_collection->identifyHash(),
                             cluster_collection->size());

    cluster_collection->push_back(cluster.release());
  }
  return cluster_collection;
}

} // namespace HGTD
