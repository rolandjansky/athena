/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_PrepRawData/src/HGTD_Cluster.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date Apr, 2021
 * @brief Implementation file for class HGTD_Cluster.
 */

#include "HGTD_PrepRawData/HGTD_Cluster.h"

#include <utility>

HGTD_Cluster::HGTD_Cluster(const Identifier& rdo_id,
                           const Amg::Vector2D& loc_pos,
                           const std::vector<Identifier>& rdo_list,
                           const InDet::SiWidth& width,
                           const InDetDD::SolidStateDetectorElementBase* det_el,
                           const Amg::MatrixX& loc_err_matx,
                           const float time_of_arrival,
                           const float time_of_arrival_err,
                           const std::vector<int>& time_over_threshold)
    : PrepRawData(rdo_id, loc_pos, rdo_list, loc_err_matx),
      m_width(width),
      m_det_el(det_el),
      m_time(time_of_arrival),
      m_time_resolution(time_of_arrival_err),
      m_time_over_threshold(time_over_threshold)
{
}

// Constructor for use from tp converter
HGTD_Cluster::HGTD_Cluster(const Identifier& rdo_id,
                           const Amg::Vector2D& loc_pos,
                           std::vector<Identifier>&& rdo_list,
                           const InDet::SiWidth& width,
                           const InDetDD::SolidStateDetectorElementBase* det_el,
                           Amg::MatrixX&& loc_err_matx,
                           const float time_of_arrival,
                           const float time_of_arrival_err,
                           const std::vector<int>& time_over_threshold)
    : PrepRawData(rdo_id, loc_pos, std::move(rdo_list), std::move(loc_err_matx)),
      m_width(width),
      m_det_el(det_el),
      m_time(time_of_arrival),
      m_time_resolution(time_of_arrival_err),
      m_time_over_threshold(time_over_threshold) {}

// Default constructor:
HGTD_Cluster::HGTD_Cluster() : PrepRawData() {
  m_det_el = nullptr;
}

// move constructor:
HGTD_Cluster::HGTD_Cluster(HGTD_Cluster&& rhs) : PrepRawData(std::move(rhs)) {
  m_width = std::move(rhs.m_width);
  m_glob_pos = std::move(rhs.m_glob_pos);
  m_det_el = std::exchange(rhs.m_det_el, nullptr);
  m_time = std::exchange(rhs.m_time, 0);
  m_time_resolution = std::exchange(rhs.m_time_resolution, 0);
  m_time_over_threshold = std::move(rhs.m_time_over_threshold);
}

// move
HGTD_Cluster& HGTD_Cluster::operator=(HGTD_Cluster&& rhs) {
  if (&rhs != this) {
    PrepRawData::operator=(std::move(rhs));
    m_width = std::move(rhs.m_width);
    m_glob_pos = std::move(rhs.m_glob_pos);
    m_det_el = std::exchange(rhs.m_det_el, nullptr);
    m_time = std::exchange(rhs.m_time, 0);
    m_time_resolution = std::exchange(rhs.m_time_resolution, 0);
    m_time_over_threshold = std::move(rhs.m_time_over_threshold);
  }
  return *this;
}
