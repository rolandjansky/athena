/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_ClusterMakerTools/src/HGTD_ClusterMakerTool.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 *
 * @brief
 */

#include "HGTD_ClusterMakerTool.h"

#include "HGTD_PrepRawData/HGTD_Cluster.h"

// usable in the current file only
namespace {
inline double square(const double x) { return x * x; }

constexpr double g_one_over_twelve = 1. / 12.;
} // namespace

HGTD_ClusterMakerTool::HGTD_ClusterMakerTool(const std::string& t,
                                             const std::string& n,
                                             const IInterface* p)
    : AthAlgTool(t, n, p) {}

std::unique_ptr<HGTD_Cluster> HGTD_ClusterMakerTool::createCluster(
    const Identifier& rdo_id, const Amg::Vector2D& loc_pos,
    const std::vector<Identifier>& rdo_list, const InDet::SiWidth& width,
    const InDetDD::SolidStateDetectorElementBase* det_el,
    const float time_of_arrival,
    const std::vector<int>& time_over_threshold) const {

  Amg::MatrixX loc_err_matx = Amg::MatrixX(2, 2);
  loc_err_matx.setIdentity();
  // HGTD pixels are 1.3x1.3 mm2, using phiR and z methods for
  // consistency with other code
  loc_err_matx.fillSymmetric(0, 0, square(width.phiR()) * g_one_over_twelve);
  loc_err_matx.fillSymmetric(1, 1, square(width.z()) * g_one_over_twelve);

  // time_of_arrival_err
  // FIXME: to define the expected time resolution, I should have a service that
  // tells me  which  resolution I expect, depending on the radius and after
  // the current integrated luminosity
  float time_of_arrival_err =
      0.035; // 35 pico second resolution at start of operation, given in nano
             // seconds in ATHENA

  return std::make_unique<HGTD_Cluster>(
      rdo_id, loc_pos, rdo_list, width, det_el, std::move(loc_err_matx), time_of_arrival,
      time_of_arrival_err, time_over_threshold);
}
