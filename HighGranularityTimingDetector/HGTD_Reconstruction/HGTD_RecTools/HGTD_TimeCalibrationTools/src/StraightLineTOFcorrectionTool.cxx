/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_TimeCalibrationTools/src/StraightLineTOFcorrectionTool.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 *
 * @brief Implementation file of StraightLineTOFcorrectionTool.h
 */

#include "HGTD_TimeCalibrationTools/StraightLineTOFcorrectionTool.h"

#include "GaudiKernel/PhysicalConstants.h"
#include "HGTD_PrepRawData/HGTD_Cluster.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorElement.h"
#include "TrkTrack/Track.h"

HGTD::StraightLineTOFcorrectionTool::StraightLineTOFcorrectionTool(
    const std::string& t, const std::string& n, const IInterface* p)
    : base_class(t, n, p) {}

StatusCode HGTD::StraightLineTOFcorrectionTool::initialize() {

  ATH_CHECK(AlgTool::initialize());

  ATH_CHECK(detStore()->retrieve(m_hgtd_det_mgr, "HGTD"));

  return StatusCode::SUCCESS;
}

std::pair<float, float>
HGTD::StraightLineTOFcorrectionTool::correctTimeAndResolution(
    const Trk::Track& track, const HGTD_Cluster& cluster,
    const float time_of_arrival, const float time_of_arrival_err) const {

  ATH_MSG_DEBUG("Correcting input time: " << time_of_arrival);

  // FIXME: This should work with 'globalPosition' of the cluster alone, but no
  // det element set during P->T conversion!
  const InDetDD::HGTD_DetectorElement* det_el =
      m_hgtd_det_mgr->getDetectorElement(cluster.identify());

  // get the distance between the track origin and the hit position
  const Trk::Perigee* perigee = track.perigeeParameters();
  Amg::Vector3D glob_hit_pos = det_el->globalPosition(cluster.localPosition());

  float dx = glob_hit_pos.x() - perigee->position().x();
  float dy = glob_hit_pos.y() - perigee->position().y();
  float dz = glob_hit_pos.z() - perigee->position().z();

  // compute TOF between origin and hit position
  // FIXME define 1/c somewhere as constexpr?
  // FIXME since this is an approximation anyway, can we use a fast sqrt?
  float tof = std::sqrt(dx * dx + dy * dy + dz * dz) / Gaudi::Units::c_light;
  float corrected_toa = time_of_arrival - tof;

  ATH_MSG_DEBUG("Corrected time: " << corrected_toa);

  // TODO
  // the intrinsic resolution on the time of arrival needs to be combined
  // with the impact parameter resolution and the spatial resolution of the
  // cluster. no large impact is expected though

  return {corrected_toa, time_of_arrival_err};
}
