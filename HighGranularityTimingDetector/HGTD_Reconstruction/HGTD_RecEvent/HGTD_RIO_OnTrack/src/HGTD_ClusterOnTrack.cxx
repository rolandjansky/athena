/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
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

// Default constructor:
HGTD_ClusterOnTrack::HGTD_ClusterOnTrack()
    : RIO_OnTrack(),
      m_det_el(nullptr),
      m_id_hash(),
      m_global_position(),
      m_calibrated_time(0),
      m_calibrated_time_resolution(0) {}

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
      m_global_position(global_position),
      m_calibrated_time(calib_time),
      m_calibrated_time_resolution(calib_time_res) {
  m_rio.setElement(rio);
}

const Trk::Surface& HGTD_ClusterOnTrack::associatedSurface() const {
  return (detectorElement()->surface());
}

const Amg::Vector3D& HGTD_ClusterOnTrack::globalPosition() const {
  return m_global_position;
}

MsgStream& HGTD_ClusterOnTrack::dump(MsgStream& sl) const {
  sl << "HGTD_ClusterOnTrack {" << endmsg;

  Trk::RIO_OnTrack::dump(sl);

  sl << "Global position (x,y,z) = (";
  sl << this->globalPosition().x() << ", " << this->globalPosition().y()
     << ", " << this->globalPosition().z() << ")" << endmsg;
  sl << "}" << endmsg;
  return sl;
}

std::ostream& HGTD_ClusterOnTrack::dump(std::ostream& sl) const {
  sl << "HGTD_ClusterOnTrack {" << std::endl;

  Trk::RIO_OnTrack::dump(sl);

  sl << "Global position (x,y,z) = (";
  sl << this->globalPosition().x() << ", " << this->globalPosition().y()
     << ", " << this->globalPosition().z() << ")" << std::endl;
  sl << "}" << std::endl;
  return sl;
}

void HGTD_ClusterOnTrack::setValues(const Trk::TrkDetElementBase* det_el,
                                    const Trk::PrepRawData* /*prd*/) {
  m_det_el =
      dynamic_cast<const InDetDD::SolidStateDetectorElementBase*>(det_el);
}
