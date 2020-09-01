/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// GXFTrackState.h
//   Class grouping all fitter relevant info on a surface-by-surface
//   basis
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_GXFTRACKSTATE_H
#define TRK_GXFTRACKSTATE_H

#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"

/**
 * These headers, as well as other headers in the TrkGlobalChi2Fitter package
 * use modern C++11 memory ownership semantics expressed through smart
 * pointers. See GlobalChi2Fitter.h for more information.
 */

namespace Trk {

  class MeasurementBase;
  class TrackStateOnSurface;
  class GXFMaterialEffects;
  class TransportJacobian;

  class GXFTrackState {
  public:
    GXFTrackState() = delete;
    GXFTrackState(GXFTrackState &);

    GXFTrackState(std::unique_ptr<const MeasurementBase>, std::unique_ptr<const TrackParameters>);
    GXFTrackState(std::unique_ptr<const TrackParameters>, TrackStateOnSurface::TrackStateOnSurfaceType);
    GXFTrackState(std::unique_ptr<GXFMaterialEffects>, std::unique_ptr<const TrackParameters>);
    GXFTrackState & operator=(GXFTrackState &) = delete;

    void setMeasurement(std::unique_ptr<const MeasurementBase>);
    const MeasurementBase *measurement(void);

    void setTrackParameters(std::unique_ptr<const TrackParameters>);
    const TrackParameters *trackParameters(void);
    
    GXFMaterialEffects *materialEffects();
    const Surface *surface() const;
    void setJacobian(TransportJacobian &);
    Eigen::Matrix<double, 5, 5> & jacobian();
    Amg::MatrixX & derivatives();
    void setDerivatives(Amg::MatrixX &);

    void setTrackCovariance(AmgSymMatrix(5) *);
    AmgSymMatrix(5) & trackCovariance(void);
    bool hasTrackCovariance(void);
    void zeroTrackCovariance(void);

    void setFitQuality(std::unique_ptr<const FitQualityOnSurface>);
    const FitQualityOnSurface *fitQuality(void);

    TrackState::MeasurementType measurementType();
    void setMeasurementType(TrackState::MeasurementType);

    double sinStereo();
    void setSinStereo(double);

    double *measurementErrors();
    void setMeasurementErrors(double *);

    int numberOfMeasuredParameters();

    bool isRecalibrated();
    void setRecalibrated(bool);

    const Amg::Vector3D & position();
    void setPosition(Amg::Vector3D &);

    bool measuresPhi();
    void setMeasuresPhi(bool);

    void resetStateType(TrackStateOnSurface::TrackStateOnSurfaceType, bool v=true);
    void setStateType(TrackStateOnSurface::TrackStateOnSurfaceType, bool v=true);
    bool getStateType(TrackStateOnSurface::TrackStateOnSurfaceType);

  private:
    std::unique_ptr<const MeasurementBase> m_measurement;       //!< The measurement defining the track state
    std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> m_tsType;      //!< type of track state, eg Fittable, Outlier, Scatterer, Brem, Hole
    std::unique_ptr<const TrackParameters> m_trackpar;  //!< track parameters
    std::unique_ptr<GXFMaterialEffects> m_materialEffects;      //!< Material effects on track (ie scatterer, brem)
    Eigen::Matrix<double, 5, 5> m_jacobian;    //!< Transport jacobian wrt previous state
    Amg::MatrixX m_derivs;  //!< Derivatives of local parameters wrt fit parameters

    AmgSymMatrix(5) m_covariancematrix;     //!< Covariance matrix of track parameters at this surface
    bool m_covariance_set;

    std::unique_ptr<const FitQualityOnSurface> m_fitqual;
    double m_measerror[5];      //!< Measurement errors (corrected for stereo angle)
    double m_sinstereo;         //!< stereo angle
    TrackState::MeasurementType m_mType;      //!< Measurement type, eg pixel, SCT, ...
    bool m_recalib;             //!< Has this measurement already been recalibrated?
    bool m_measphi;
    Amg::Vector3D m_globpos;
    std::optional<std::vector<const TrackParameters *>> m_preholes;

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  };

  inline Eigen::Matrix<double, 5, 5> & GXFTrackState::jacobian() {
    return m_jacobian;
  } 
  
  inline Amg::MatrixX & GXFTrackState::derivatives() {
    return m_derivs;
  }

  inline AmgSymMatrix(5) & GXFTrackState::trackCovariance(void) {
    return m_covariancematrix;
  }

  inline const TrackParameters *GXFTrackState::trackParameters(void) {
    return m_trackpar.get();
  }

  inline GXFMaterialEffects *GXFTrackState::materialEffects() {
    return m_materialEffects.get();
  }

  inline TrackState::MeasurementType GXFTrackState::measurementType() {
    return m_mType;
  }

  inline void GXFTrackState::setMeasurementType(TrackState::MeasurementType mt) {
    m_mType = mt;
  }

  inline bool GXFTrackState::isRecalibrated() {
    return m_recalib;
  }
}
#endif
