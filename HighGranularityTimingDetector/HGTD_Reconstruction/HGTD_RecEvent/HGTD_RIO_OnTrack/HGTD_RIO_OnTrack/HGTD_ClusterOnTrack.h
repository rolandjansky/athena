/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_RIO_OnTrack/HGTD_ClusterOnTrack.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date July, 2021
 * @brief RIO_OnTrack class for HGTD. The HGTD_ClusterOnTrack contains,
 * compared to the HGTD_Cluster, a calibrated time and time resolution.
 * This means that the time is corrected e.g. for time of flight. The
 * time resolution takes into account the impact parameter resolution, which
 * is propagated to the time resolution.
 */

#ifndef HGTD_CLUSTERONTRACK_H
#define HGTD_CLUSTERONTRACK_H

#include "HGTD_PrepRawData/HGTD_Cluster.h"
#include "HGTD_PrepRawData/HGTD_ClusterContainer.h"
#include "Identifier/IdentifierHash.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include <memory>

namespace Trk {
class LocalParameters;
}

class HGTD_ClusterOnTrack : public Trk::RIO_OnTrack {

public:
  HGTD_ClusterOnTrack();
  HGTD_ClusterOnTrack(const HGTD_ClusterOnTrack&) = default;
  HGTD_ClusterOnTrack& operator=(const HGTD_ClusterOnTrack&) = default;
  HGTD_ClusterOnTrack(HGTD_ClusterOnTrack&&) = default;
  HGTD_ClusterOnTrack& operator=(HGTD_ClusterOnTrack&&) = default;

  /**
   * @brief   Constructor with parameters. The time and time resolution are
   * not the time and time resolution of the HGTD_Cluster, but calibrated
   * using information from the track the hit got assigned to.
   *
   * @param [in] rio Cluster in HGTD associated to the tack.
   * @param [in] locpars Local track parameters.
   * @param [in] locerr Track parameter ovariance matrix.
   * @param [in] calib_time Calibrated time measured in HGTD.
   * @param [in] calib_time_res Time resolution including propagated impact
   *             parameter resolution.
   * @param [in] id_hash Hash identifier of the HGTD_Cluster. Needed to
   *             fullfill the Trk::RIO_OnTrack interface.
   */
  HGTD_ClusterOnTrack(const HGTD_Cluster* rio,
                      const Trk::LocalParameters& locpars,
                      const Amg::MatrixX& locerr, const float calib_time,
                      const float calib_time_res,
                      const IdentifierHash& id_hash);

  /**
   * @brief   Constructor with parameters. The time and time resolution are
   * not the time and time resolution of the HGTD_Cluster, but calibrated
   * using information from the track the hit got assigned to.
   *
   * @param [in] rio Cluster in HGTD associated to the tack.
   * @param [in] locpars Local track parameters.
   * @param [in] locerr Track parameter ovariance matrix.
   * @param [in] calib_time Calibrated time measured in HGTD.
   * @param [in] calib_time_res Time resolution including propagated impact
   *             parameter resolution.
   * @param [in] id_hash Hash identifier of the HGTD_Cluster. Needed to
   *             fullfill the Trk::RIO_OnTrack interface.
   */
  HGTD_ClusterOnTrack(const HGTD_Cluster* rio,
                      const Trk::LocalParameters& locpars,
                      const Amg::MatrixX& locerr, const float calib_time,
                      const float calib_time_res, const IdentifierHash& idDE,
                      const Amg::Vector3D& global_position);

  /** Pseudo-constructor : needed to avoid excessive RTTI*/
  virtual HGTD_ClusterOnTrack* clone() const override final;

  /** returns the surface for the local to global transformation
    - fullfills Trk::MeasurementBase interface*/
  virtual const Trk::Surface& associatedSurface() const override final;

  virtual bool rioType(Trk::RIO_OnTrackType::Type type) const override final
  {
    return (type == Trk::RIO_OnTrackType::HGTD_Cluster);
  }

  /** returns the detector element, assoicated with the PRD of this class
    - fullfills Trk::RIO_OnTrack interface*/
  virtual const InDetDD::SolidStateDetectorElementBase* detectorElement() const override final;

  /** returns the PrepRawData - is a SiCluster in this scope
    - fullfills Trk::RIO_OnTrack interface*/
  virtual const HGTD_Cluster* prepRawData() const override final;

  /** returns global position (gathered through Surface constraint)
  - fullfills Trk::MeasurementBase interface */
  virtual const Amg::Vector3D& globalPosition() const override final;

  /** returns the DE hashID*
  - fullfills Trk::RIO_OnTrack interface*/
  virtual IdentifierHash idDE() const override final;

  /**returns some information about this RIO_OnTrack.
- fullfills Trk::RIO_OnTrack interface*/
  virtual MsgStream& dump(MsgStream& out) const override final;

  /**returns some information about this RIO_OnTrack.
- fullfills Trk::RIO_OnTrack interface*/
  virtual std::ostream& dump(std::ostream& out) const override final;

  virtual float time() const;

  virtual float timeResolution() const;

private:
  virtual void setValues(const Trk::TrkDetElementBase* det_el,
                         const Trk::PrepRawData* prd) override final;

protected:
  /** Detector element associated to this cluster on track. The COT does not
   * manage the object! */
  const InDetDD::SolidStateDetectorElementBase* m_det_el;

  /** The IdentifierHash - needed to comply with RIO_OnTrack interface,
   * currently not used*/
  IdentifierHash m_id_hash;

  /** The global position, managed by the cluster on track */
  Amg::Vector3D m_global_position;

private:
  float m_calibrated_time;

  float m_calibrated_time_resolution;

  ElementLink<HGTD_ClusterContainer> m_rio;
};

inline IdentifierHash HGTD_ClusterOnTrack::idDE() const { return m_id_hash; }

inline HGTD_ClusterOnTrack* HGTD_ClusterOnTrack::clone() const {
  return new HGTD_ClusterOnTrack(*this);
}

inline const HGTD_Cluster* HGTD_ClusterOnTrack::prepRawData() const {
  if (m_rio.isValid())
    return m_rio.cachedElement();
  else
    return nullptr;
}

inline const InDetDD::SolidStateDetectorElementBase*
HGTD_ClusterOnTrack::detectorElement() const {
  return m_det_el;
}

inline float HGTD_ClusterOnTrack::time() const { return m_calibrated_time; }

inline float HGTD_ClusterOnTrack::timeResolution() const {
  return m_calibrated_time_resolution;
}

#endif // HGTD_CLUSTERONTRACK_H
