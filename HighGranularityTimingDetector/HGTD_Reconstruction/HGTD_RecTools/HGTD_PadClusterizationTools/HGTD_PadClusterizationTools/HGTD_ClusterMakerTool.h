/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_ClusterMakerTools/HGTD_ClusterMakerTool.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 *
 * @brief Creates HGTD_Cluster objects from the pre-processed raw data
 * information. The clustering itself happens outside of this tool, it just
 * combines the information into a HGTD_Cluster object.
 *
 * TODO:
 * - The resolution of the TOA is currently fixed to 35ps. This is radius and
 * luminosity dependent and should be set via a dedicated tool!
 */

#ifndef HGTD_CLUSTERMAKERTOOL_H
#define HGTD_CLUSTERMAKERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "HGTD_PrepRawData/HGTD_Cluster.h"
#include <memory>

class HGTD_ClusterMakerTool : public AthAlgTool {

public:
  HGTD_ClusterMakerTool(const std::string&, const std::string&,
                        const IInterface*);

  /**
   * @brief Builds a HGTD_Cluster.
   *
   * @param [in] rdo_id Identifier of the Cluster, retrieved from centroid
   * position in case more than one pads are contained in the cluster.
   * @param [in] loc_pos local position of the cluster
   * @param [in] rdo_list RDO identifiers of the pads included in the cluster.
   * @param [in] width SiWidth information of the full cluster.
   * @param [in] det_el Detector element (module) the RDOs were found on.
   * @param [in] time_of_arrival Time of arrival assigned to the cluster.
   * @param [in] time_over_threshold TOT of each of the cluster components.
   *
   * @return Pointer to a HGTD_Cluster, owned by caller.
   */
  virtual std::unique_ptr<HGTD_Cluster>
  createCluster(const Identifier& rdo_id, const Amg::Vector2D& loc_pos,
                const std::vector<Identifier>& rdo_list,
                const InDet::SiWidth& width,
                const InDetDD::SolidStateDetectorElementBase* det_el,
                const float time_of_arrival,
                const std::vector<int>& time_over_threshold) const;
};

#endif // HGTD_CLUSTERMAKERTOOL_H
