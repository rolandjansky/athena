/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_RIO_OnTrack/src/HGTD_ClusterOnTrack.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date July, 2021
 * @brief
 */

#include "HGTD_RIO_OnTrack/HGTD_ClusterOnTrack.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include <ostream>
#include <utility>

namespace HGTD {

// Default constructor:
HGTD_ClusterOnTrack::HGTD_ClusterOnTrack()
    : RIO_OnTrack(),
      m_det_el(nullptr),
      m_id_hash(),
      m_global_position(nullptr),
      m_calibrated_time(0),
      m_calibrated_time_resolution(0) {}

// copy constructor:
HGTD_ClusterOnTrack::HGTD_ClusterOnTrack(const HGTD_ClusterOnTrack& rhs)
    : RIO_OnTrack(rhs),
      m_det_el(nullptr),
      m_id_hash(rhs.m_id_hash),
      m_global_position(rhs.m_global_position ? std::make_unique<Amg::Vector3D>(
                                                    *rhs.m_global_position)
                                              : nullptr),
      m_calibrated_time(rhs.m_calibrated_time),
      m_calibrated_time_resolution(rhs.m_calibrated_time_resolution) {}

// assignment operator:
HGTD_ClusterOnTrack&
HGTD_ClusterOnTrack::operator=(const HGTD_ClusterOnTrack& rhs) {
  if (&rhs != this) {
    Trk::RIO_OnTrack::operator=(rhs);
    m_id_hash = rhs.m_id_hash;
    m_global_position =
        rhs.m_global_position
            ? std::make_unique<Amg::Vector3D>(*rhs.m_global_position)
            : nullptr;
    m_det_el = rhs.m_det_el;
    m_calibrated_time = rhs.m_calibrated_time;
    m_calibrated_time_resolution = rhs.m_calibrated_time_resolution;
    m_rio = rhs.m_rio;
  }
  return *this;
}

// move constructor:
HGTD_ClusterOnTrack::HGTD_ClusterOnTrack(HGTD_ClusterOnTrack&& rhs)
    : RIO_OnTrack(std::move(rhs)) {
  m_id_hash = std::move(rhs.m_id_hash);
  m_global_position = std::move(rhs.m_global_position);
  m_det_el = std::exchange(rhs.m_det_el, nullptr);
  m_calibrated_time = std::exchange(rhs.m_calibrated_time, 0);
  m_calibrated_time_resolution =
      std::exchange(rhs.m_calibrated_time_resolution, 0);
  m_rio = std::move(rhs.m_rio);
}

// move assignment:
HGTD_ClusterOnTrack& HGTD_ClusterOnTrack::operator=(HGTD_ClusterOnTrack&& rhs) {
  if (&rhs != this) {
    RIO_OnTrack::operator=(std::move(rhs));
    m_id_hash = std::move(rhs.m_id_hash);
    m_global_position = std::move(rhs.m_global_position);
    m_det_el = std::exchange(rhs.m_det_el, nullptr);
    m_calibrated_time = std::exchange(rhs.m_calibrated_time, 0);
    m_calibrated_time_resolution =
        std::exchange(rhs.m_calibrated_time_resolution, 0);
    m_rio = std::move(rhs.m_rio);
  }
  return *this;
}

// Destructor:
HGTD_ClusterOnTrack::~HGTD_ClusterOnTrack() {}

// Constructor with parameters - no global position specified
HGTD_ClusterOnTrack::HGTD_ClusterOnTrack(const HGTD_Cluster* rio,
                                         const Trk::LocalParameters& locpars,
                                         const Amg::MatrixX& locerr,
                                         const float calib_time,
                                         const float calib_time_res,
                                         const IdentifierHash& id_hash)
    : RIO_OnTrack(locpars, locerr, rio->identify()),
      m_det_el(rio->detectorElement()),
      m_id_hash(id_hash),
      m_calibrated_time(calib_time),
      m_calibrated_time_resolution(calib_time_res) {
  m_rio.setElement(rio);
}

// Constructor with parameters
HGTD_ClusterOnTrack::HGTD_ClusterOnTrack(const HGTD_Cluster* rio,
                                         const Trk::LocalParameters& locpars,
                                         const Amg::MatrixX& locerr,
                                         const float calib_time,
                                         const float calib_time_res,
                                         const IdentifierHash& id_hash,
                                         const Amg::Vector3D& global_position)
    : RIO_OnTrack(locpars, locerr, rio->identify()),
      m_det_el(rio->detectorElement()),
      m_id_hash(id_hash),
      m_global_position(std::make_unique<const Amg::Vector3D>(global_position)),
      m_calibrated_time(calib_time),
      m_calibrated_time_resolution(calib_time_res) {
  m_rio.setElement(rio);
}

const Trk::Surface& HGTD_ClusterOnTrack::associatedSurface() const {
  return (detectorElement()->surface());
}

const Amg::Vector3D& HGTD_ClusterOnTrack::globalPosition() const {
  if (not m_global_position)
    m_global_position = std::make_unique<const Amg::Vector3D>(
        *(associatedSurface().localToGlobal(localParameters())));
  return (*m_global_position);
}

MsgStream& HGTD_ClusterOnTrack::dump(MsgStream& sl) const {

  sl << "HGTD_ClusterOnTrack {" << endreq;
  Trk::RIO_OnTrack::dump(sl);

  sl << "Global position (x,y,z) = (";
  this->globalPosition();
  if (m_global_position) {
    sl << this->globalPosition().x() << ", " << this->globalPosition().y()
       << ", " << this->globalPosition().z() << ")" << endreq;
  } else {
    sl << "NULL!), " << endreq;
  }
  sl << "}" << endreq;
  return sl;
}

std::ostream& HGTD_ClusterOnTrack::dump(std::ostream& sl) const {
  sl << "HGTD_ClusterOnTrack {" << std::endl;

  Trk::RIO_OnTrack::dump(sl);

  sl << "Global position (x,y,z) = (";
  this->globalPosition();
  if (m_global_position) {
    sl << this->globalPosition().x() << ", " << this->globalPosition().y()
       << ", " << this->globalPosition().z() << ")" << std::endl;
  } else {
    sl << "NULL!), " << std::endl;
  }

  sl << "}" << std::endl;
  return sl;
}

void HGTD_ClusterOnTrack::setValues(const Trk::TrkDetElementBase* det_el,
                                    const Trk::PrepRawData* /*prd*/) {
  m_det_el =
      dynamic_cast<const InDetDD::SolidStateDetectorElementBase*>(det_el);
}

} // namespace HGTD
