/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_ClusterMakerTools/SinglePadClusterTool.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 *
 * @brief Translates HGTD_RDO objects of a given detector element into
 * HGTD_PrepRawData clusters. Since currently no or only very small crosstalk
 * between neighbouring pixels of LGAD sensors is expected, a single RDO gets
 * translated directly into a HGTD_Cluster.
 *
 */

#ifndef HGTD_SINGLEPADCLUSTERTOOL_H
#define HGTD_SINGLEPADCLUSTERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "HGTD_RecToolInterfaces/IHGTD_PadClusterizationTool.h"

#include "HGTD_PadClusterizationTools/HGTD_ClusterMakerTool.h"

#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"

#include <memory>

namespace HGTD {

class SinglePadClusterTool : public extends<AthAlgTool, IHGTD_PadClusterizationTool> {

public:
  SinglePadClusterTool(const std::string& type, const std::string& name,
                       const IInterface* parent);

  virtual StatusCode initialize() override final;

  virtual std::unique_ptr<HGTD_ClusterCollection>
  clusterize(const HGTD_RDOCollection& rdo_coll) const override final;

private:
  ToolHandle<HGTD_ClusterMakerTool> m_cluster_maker{this, "ClusterMakerTool", "HGTD_ClusterMakerTool", "Tool for creating HGTD_Cluster objects"};
  const HGTD_DetectorManager* m_hgtd_det_mgr{nullptr};
};

} // namespace HGTD

#endif // HGTD_SINGLEPADCLUSTERTOOL_H
