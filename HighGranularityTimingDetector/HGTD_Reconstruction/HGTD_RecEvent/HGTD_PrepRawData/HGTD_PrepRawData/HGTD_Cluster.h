/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_PrepRawData/HGTD_Cluster.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date Apr, 2021
 * @brief Class representing clusters used for tracking purposes in the HGTD
 *        system, extends the Trk::PrepRawData interface. A cluster can contain
 *        single LGAD pixel or several (connected) LGAD pixels.
 *
 *        NOTE:
 *        The time of arrival (TOA) and time over threshold (TOT) as read out by
 *        the ASIC are 7 bit and 8 bit words respectively. TOA encodes the
 *        active time window of 2.5ns, and is read out in 20ps time steps. TOT
 *        is read out in 40ps steps. Due to the way LGAD digi is currently set
 *        up, the analog->digital conversion of the time information is not
 *        included. For this reason TOA/TOT are represented as float/int and
 *        also stored as such in the persistent format. This can be optimised in
 *        the future to a char type, but needs further treatment internally to
 *        make sense in connection with a time resolution.
 */

#ifndef HGTD_PREPRAWDATA_HGTD_CLUSTER_H
#define HGTD_PREPRAWDATA_HGTD_CLUSTER_H

#include "HGTD_ReadoutGeometry/HGTD_DetectorElement.h"
#include "Identifier/IdentifierHash.h"
#include "InDetPrepRawData/SiWidth.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkSurfaces/Surface.h"
#include <memory>
#include <numeric>

class HGTD_Cluster : public Trk::PrepRawData {

public:
  HGTD_Cluster();
  HGTD_Cluster(const HGTD_Cluster&);
  HGTD_Cluster(HGTD_Cluster&&);
  HGTD_Cluster& operator=(const HGTD_Cluster&);
  HGTD_Cluster& operator=(HGTD_Cluster&&);

  HGTD_Cluster(const Identifier& rdo_id, const Amg::Vector2D& loc_pos,
               const std::vector<Identifier>& rdo_list,
               const InDet::SiWidth& width,
               const InDetDD::SolidStateDetectorElementBase* det_el,
               const Amg::MatrixX& loc_err_matx, const float time_of_arrival,
               const float time_of_arrival_err,
               const std::vector<int>& time_over_threshold);

  // Constructor for use from tp converter.
  HGTD_Cluster(const Identifier& rdo_id, const Amg::Vector2D& loc_pos,
               std::vector<Identifier>&& rdo_list, const InDet::SiWidth& width,
               const InDetDD::SolidStateDetectorElementBase* det_el,
               Amg::MatrixX&& loc_err_matx,
               const float time_of_arrival, const float time_of_arrival_err,
               const std::vector<int>& time_over_threshold);

  virtual ~HGTD_Cluster() {}

  // return width class reference
  virtual const InDet::SiWidth& width() const;

  // return global position reference
  virtual const Amg::Vector3D& globalPosition() const;

  virtual const InDetDD::SolidStateDetectorElementBase* detectorElement() const override;

  /** Interface method checking the type*/
  virtual bool type(Trk::PrepRawDataType type) const override;

  // return time of arrival
  virtual float time() const;

  // return the resolution of the time of arrival
  virtual float timeResolution() const;

  // return time over threshold list
  virtual const std::vector<int>& totList() const;

private:
  InDet::SiWidth m_width; // col, row, and width in mm

  mutable std::unique_ptr<const Amg::Vector3D> m_glob_pos;

  const InDetDD::SolidStateDetectorElementBase* m_det_el;

  float m_time;
  float m_time_resolution;
  std::vector<int> m_time_over_threshold;
};

///////////////////////////////////////////////////////////////////
// Inline methods:

inline const InDet::SiWidth& HGTD_Cluster::width() const { return m_width; }

inline const Amg::Vector3D& HGTD_Cluster::globalPosition() const {
  if (m_glob_pos == nullptr and m_det_el) {
    // Trk::Surface::localToGlobal returns const Amg::Vector3D&
    auto temp_ref =
        m_det_el->surface(identify()).localToGlobal(localPosition());
    m_glob_pos.reset(&temp_ref);
  }
  if (not m_glob_pos) {
    throw Trk::PrepRawDataUndefinedVariable();
  }
  return *m_glob_pos;
}

inline const InDetDD::SolidStateDetectorElementBase*
HGTD_Cluster::detectorElement() const {
  return m_det_el;
}

inline bool
HGTD_Cluster::type(Trk::PrepRawDataType type) const
{
  return type == Trk::PrepRawDataType::HGTD_Cluster;
}

inline float HGTD_Cluster::time() const { return m_time; }

inline float HGTD_Cluster::timeResolution() const { return m_time_resolution; }

inline const std::vector<int>& HGTD_Cluster::totList() const {
  return m_time_over_threshold;
}

#endif // HGTD_PREPRAWDATA_HGTD_CLUSTER_H
