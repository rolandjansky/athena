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
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"

namespace Trk {

  class MeasurementBase;
  class TrackStateOnSurface;
  class GXFMaterialEffects;
  class TransportJacobian;

  class GXFTrackState {
  public:
    GXFTrackState();
    GXFTrackState(GXFTrackState &);
    ~GXFTrackState();

    GXFTrackState(const MeasurementBase *, const TrackParameters * trackpar = nullptr, bool ownmb = false);
    GXFTrackState(const TrackParameters *);
    GXFTrackState(GXFMaterialEffects *, const TrackParameters * trackpar = nullptr);
    GXFTrackState & operator=(GXFTrackState & rhs);

    void setMeasurement(const MeasurementBase *);
    const MeasurementBase *measurement(bool takeownership = false);
    TrackState::TrackStateType trackStateType();
    void setTrackStateType(TrackState::TrackStateType);
    void setTrackParameters(const TrackParameters *);
    const TrackParameters *trackParameters(bool takeownership = false);
    void setMaterialEffects(GXFMaterialEffects *);
    GXFMaterialEffects *materialEffects();
    const Surface *surface();
    void setJacobian(TransportJacobian *);
    double (* (jacobian)())[5];
    Amg::MatrixX & derivatives();
    void setDerivatives(Amg::MatrixX &);
    AmgSymMatrix(5) * trackCovariance(bool takeownership = false);
    void setTrackCovariance(AmgSymMatrix(5) *);
    const FitQualityOnSurface *fitQuality(bool takeownership = false);
    void setFitQuality(const FitQualityOnSurface *);
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

  private:
    const MeasurementBase *m_measurement;       //!< The measurement defining the track state
    TrackState::TrackStateType m_tsType;      //!< type of track state, eg Fittable, Outlier, Scatterer, Brem, Hole
    const TrackParameters *m_trackpar;  //!< track parameters
    GXFMaterialEffects *m_materialEffects;      //!< Material effects on track (ie scatterer, brem)
    double m_jacobian[5][5];    //!< Transport jacobian wrt previous state
    Amg::MatrixX * m_derivs;  //!< Derivatives of local parameters wrt fit parameters
    AmgSymMatrix(5) * m_covariancematrix;     //!< Covariance matrix of track parameters at this surface
    const FitQualityOnSurface *m_fitqual;
    double m_measerror[5];      //!< Measurement errors (corrected for stereo angle)
    double m_sinstereo;         //!< stereo angle
    TrackState::MeasurementType m_mType;      //!< Measurement type, eg pixel, SCT, ...
    bool m_recalib;             //!< Has this measurement already been recalibrated?
    bool m_owntp;
    bool m_ownmb;
    bool m_ownfq;
    bool m_owncov;
    bool m_measphi;
    Amg::Vector3D m_globpos;
  };

  inline double (* (GXFTrackState::jacobian)())[5] {
    return m_jacobian;
  } 
  
  inline Amg::MatrixX & GXFTrackState::derivatives() {
    return *m_derivs;
  }

  inline AmgSymMatrix(5) * GXFTrackState::trackCovariance(bool takeownership) {
    AmgSymMatrix(5) * tmpcov = m_covariancematrix;
    if (takeownership) {
      m_owncov = false;
    }
    return tmpcov;
  }

  inline const TrackParameters *GXFTrackState::trackParameters(bool takeownership) {
    if (takeownership)
      m_owntp = false;
    return m_trackpar;
  }

  inline GXFMaterialEffects *GXFTrackState::materialEffects() {
    return m_materialEffects;
  }

  inline TrackState::MeasurementType GXFTrackState::measurementType() {
    return m_mType;
  }

  inline TrackState::TrackStateType GXFTrackState::trackStateType() {
    return m_tsType;
  }

  inline void GXFTrackState::setMeasurementType(TrackState::MeasurementType mt) {
    m_mType = mt;
  }

  inline bool GXFTrackState::isRecalibrated() {
    return m_recalib;
  }
}
#endif
