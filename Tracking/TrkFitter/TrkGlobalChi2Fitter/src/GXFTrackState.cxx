/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// GXFTrackState.cxx
// see header file for documentation.
//////////////////////////////////////////////////////////////////
//
#include "TrkGlobalChi2Fitter/GXFTrackState.h"
#include "TrkGlobalChi2Fitter/GXFMaterialEffects.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkExUtils/TransportJacobian.h"
#include "TrkSurfaces/Surface.h"

// #include "GaudiKernel/MsgStream.h"
// #include <string>

namespace Trk {
  GXFTrackState::GXFTrackState(GXFTrackState & rhs):
    m_measurement(rhs.m_measurement != nullptr ? std::unique_ptr<const MeasurementBase>(rhs.m_measurement->clone()) : nullptr), 
    m_tsType(rhs.m_tsType), 
    m_trackpar(std::unique_ptr<const TrackParameters>(rhs.m_trackpar != nullptr ? rhs.m_trackpar->clone() : nullptr)),
    m_materialEffects(std::unique_ptr<GXFMaterialEffects>(rhs.m_materialEffects != nullptr ? new GXFMaterialEffects(*rhs. m_materialEffects) : nullptr)),
    m_derivs(rhs.m_derivs),
    m_covariancematrix(rhs.m_covariancematrix),
    m_covariance_set(rhs.m_covariance_set),
    m_fitqual(rhs.m_fitqual ? std::make_unique<const FitQualityOnSurface>(*rhs.m_fitqual) : nullptr), 
    m_sinstereo(rhs.m_sinstereo),
    m_mType(rhs.m_mType), 
    m_recalib(rhs.m_recalib),
    m_measphi(rhs.m_measphi) 
  {
    for (int i = 0; i < 5; i++) {
      m_measerror[i] = rhs.m_measerror[i];
      for (int j = 0; j < 5; j++) {
        m_jacobian(i, j) = rhs.m_jacobian(i, j);
      }
    }
  }

  GXFTrackState::GXFTrackState(
    std::unique_ptr<const MeasurementBase> measurement,
    std::unique_ptr<const TrackParameters> trackpar
  ):
    m_measurement(std::move(measurement)),
    m_tsType(TrackState::Fittable), 
    m_trackpar(std::move(trackpar)),
    m_materialEffects(nullptr), 
    m_jacobian {}, 
    m_derivs(), 
    m_covariancematrix(),
    m_covariance_set(false),
    m_fitqual(nullptr),
    m_sinstereo(0), 
    m_mType(TrackState::unidentified), 
    m_recalib(false),
    m_measphi(false) {
    m_measerror[0] = m_measerror[1] = m_measerror[2] = m_measerror[3] = m_measerror[4] = -1;
  }

  GXFTrackState::GXFTrackState(
    const TrackParameters * trackpar,
    TrackState::TrackStateType tsType
  ):
    m_measurement(nullptr),
    m_tsType(tsType), 
    m_trackpar(std::unique_ptr<const TrackParameters>(trackpar != nullptr ? trackpar->clone() : nullptr)),
    m_materialEffects(nullptr), 
    m_jacobian {}, 
    m_derivs(), 
    m_covariancematrix(), 
    m_covariance_set(false),
    m_fitqual(nullptr),
    m_sinstereo(0), 
    m_mType(TrackState::unidentified), 
    m_recalib(false),
    m_measphi(false) 
  {
    m_measerror[0] = m_measerror[1] = m_measerror[2] = m_measerror[3] = m_measerror[4] = -1;
  }

  GXFTrackState::GXFTrackState(
    GXFMaterialEffects * mef,
    const TrackParameters * trackpar
  ):
    m_measurement(nullptr),
    m_tsType(TrackState::Scatterer), 
    m_trackpar(std::unique_ptr<const TrackParameters>(trackpar != nullptr ? trackpar->clone() : nullptr)),
    m_materialEffects(std::unique_ptr<GXFMaterialEffects>(mef)), 
    m_jacobian {}, 
    m_derivs(), 
    m_covariancematrix(),
    m_covariance_set(false), 
    m_fitqual(nullptr),
    m_sinstereo(0), 
    m_mType(TrackState::unidentified), 
    m_recalib(false),
    m_measphi(false) 
  {
    m_measerror[0] = m_measerror[1] = m_measerror[2] = m_measerror[3] = m_measerror[4] = -1;
    
    if (mef->sigmaDeltaTheta() == 0) {
      m_tsType = TrackState::Brem;
    }
  }

  void GXFTrackState::setMeasurement(std::unique_ptr<const MeasurementBase> meas) {
    m_measurement = std::move(meas);
    m_recalib = true;
  }

  const MeasurementBase *GXFTrackState::measurement(void) {
    return m_measurement.get();
  }  
  
  const MeasurementBase *GXFTrackState::takeMeasurement(void) {
    return m_measurement.get();
  }

  void GXFTrackState::setTrackParameters(std::unique_ptr<const TrackParameters> par) {
    m_trackpar = std::move(par);
  }

  void GXFTrackState::setJacobian(TransportJacobian & jac) {
    m_jacobian = jac;
  }

  void
    GXFTrackState::setDerivatives(Amg::MatrixX & deriv) {
    m_derivs = deriv;
  }

  void
    GXFTrackState::setTrackStateType(TrackState::TrackStateType tstype) {
    m_tsType = tstype;
  }

  double *GXFTrackState::measurementErrors() {
    return m_measerror;
  }

  void
    GXFTrackState::setMeasurementErrors(double *measerror) {
    m_measerror[0] = measerror[0];
    m_measerror[1] = measerror[1];
    m_measerror[2] = measerror[2];
    m_measerror[3] = measerror[3];
    m_measerror[4] = measerror[4];
  }

  double
    GXFTrackState::sinStereo() {
    return m_sinstereo;
  }

  void
    GXFTrackState::setSinStereo(double sinstereo) {
    m_sinstereo = sinstereo;
  }

  const Surface *GXFTrackState::surface() {
    if (m_measurement != nullptr) {
      return &m_measurement->associatedSurface();
    } if (m_trackpar != nullptr) {
      return &m_trackpar->associatedSurface();
    } if (m_materialEffects != nullptr) {
      return m_materialEffects->surface();
    } else {
      return nullptr;
    }
  }

  void
    GXFTrackState::setTrackCovariance(AmgSymMatrix(5) * covmat) {
    if (covmat == nullptr) {
      m_covariance_set = false;
    } else {
      m_covariance_set = true;
      m_covariancematrix = *covmat;
    }
  }

  void GXFTrackState::setFitQuality(std::unique_ptr<const FitQualityOnSurface> fitqual) {
    m_fitqual = std::move(fitqual);
  }

  const FitQualityOnSurface *GXFTrackState::fitQuality(void) {
    return m_fitqual.get();
  }
  
  const FitQualityOnSurface *GXFTrackState::takeFitQuality(void) {
    return m_fitqual.get();
  }

  int
    GXFTrackState::numberOfMeasuredParameters() {
    int nmeas = 0;

    if (m_tsType == TrackState::Fittable
        || m_tsType == TrackState::GeneralOutlier) {
      for (double i : m_measerror) {
        if (i > 0) {
          nmeas++;
        }
      }
    }
    // else if (m_tsType==TrackState::Scatterer) nmeas=2;
    return nmeas;
  }

  const Amg::Vector3D & GXFTrackState::position() {
    if (m_trackpar != nullptr) {
      return m_trackpar->position();
    } if (m_measurement != nullptr) {
      return m_measurement->globalPosition();
    }
    // return surface()->center();  // Better than nothing...
    return m_globpos;
  }

  void
    GXFTrackState::setPosition(Amg::Vector3D & pos) {
    m_globpos = pos;
  }

  bool
    GXFTrackState::measuresPhi() {
    return m_measphi;
  }

  void
    GXFTrackState::setMeasuresPhi(bool measphi) {
    m_measphi = measphi;
  }

  void
    GXFTrackState::setRecalibrated(bool isrecal) {
    m_recalib = isrecal;
  }

  bool GXFTrackState::hasTrackCovariance(void) {
    return m_covariance_set;
  }

  void GXFTrackState::zeroTrackCovariance(void) {
    m_covariance_set = true;
    m_covariancematrix.setZero();
  }
}
