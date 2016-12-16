/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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


using CLHEP::HepVector;
using CLHEP::HepMatrix;


namespace Trk {
  GXFTrackState::GXFTrackState()
    :
    m_measurement(nullptr),
    m_tsType(TrackState::Fittable),
    m_trackpar(nullptr),
    m_materialEffects(nullptr),
    m_jacobian{},
    m_derivs(nullptr),
    m_covariancematrix(nullptr),
    m_fitqual(nullptr),
    m_sinstereo(0),
    m_mType(TrackState::unidentified),
    m_recalib(false),
    m_owntp(false),
    m_ownmb(false),
    m_ownfq(false),
    m_owncov(false),
    m_measphi(false) {
    m_measerror[0] = m_measerror[1] = m_measerror[2] = m_measerror[3] = m_measerror[4] = -1;
  }

  GXFTrackState::GXFTrackState(GXFTrackState &rhs) :
    m_measurement(rhs.m_measurement ? (rhs.m_ownmb ? rhs.m_measurement->clone() : rhs.m_measurement) : 0),
    m_tsType(rhs.m_tsType),
    m_trackpar(rhs.m_trackpar), // ? (rhs.m_owntp ? rhs.m_trackpar->clone() : rhs.m_trackpar) : 0),
    m_materialEffects(rhs.m_materialEffects ? new GXFMaterialEffects(*rhs.m_materialEffects) : 0),
    m_derivs(rhs.m_derivs),
    m_covariancematrix(rhs.m_covariancematrix ? (rhs.m_owncov ? new AmgSymMatrix(5)(*rhs.m_covariancematrix) : rhs.
                                                  m_covariancematrix) : 0),
    m_fitqual(rhs.m_fitqual ? (rhs.m_ownfq ? new FitQualityOnSurface(*rhs.m_fitqual) : rhs.m_fitqual) : 0),
    m_sinstereo(rhs.m_sinstereo),
    m_mType(rhs.m_mType),
    m_recalib(rhs.m_recalib),
    // m_owntp(rhs.m_owntp),
    m_owntp(false),
    m_ownmb(rhs.m_ownmb),
    m_ownfq(rhs.m_ownfq),
    m_owncov(rhs.m_owncov),
    m_measphi(rhs.m_measphi) {
    for (int i = 0; i < 5; i++) {
      m_measerror[i] = rhs.m_measerror[i];
      for (int j = 0; j < 5; j++) {
        m_jacobian[i][j] = rhs.m_jacobian[i][j];
      }
    }
  }

  GXFTrackState::GXFTrackState(const MeasurementBase *measurement, const TrackParameters *trackpar, bool ownmb) :
    m_measurement(measurement),
    m_tsType(TrackState::Fittable),
    m_trackpar(trackpar),
    m_materialEffects(nullptr),
    m_jacobian{},
    m_derivs(nullptr),
    m_covariancematrix(nullptr),
    m_fitqual(nullptr),
    m_sinstereo(0),
    m_mType(TrackState::unidentified),
    m_recalib(false),
    m_owntp(false),
    m_ownmb(ownmb),
    m_ownfq(false),
    m_owncov(false),
    m_measphi(false) {
    m_measerror[0] = m_measerror[1] = m_measerror[2] = m_measerror[3] = m_measerror[4] = -1;
  }

  GXFTrackState::GXFTrackState(const TrackParameters *trackpar) :
    m_measurement(nullptr),
    m_tsType(TrackState::Hole),
    m_trackpar(trackpar),
    m_materialEffects(nullptr),
    m_jacobian{},
    m_derivs(nullptr),
    m_covariancematrix(nullptr),
    m_fitqual(nullptr),
    m_sinstereo(0),
    m_mType(TrackState::unidentified),
    m_recalib(false),
    m_owntp(false),
    m_ownmb(false),
    m_ownfq(false),
    m_owncov(false),
    m_measphi(false) {
    m_measerror[0] = m_measerror[1] = m_measerror[2] = m_measerror[3] = m_measerror[4] = -1;
  }

  GXFTrackState::GXFTrackState(GXFMaterialEffects *mef, const TrackParameters *trackpar) :
    m_measurement(nullptr),
    m_tsType(TrackState::Scatterer),
    m_trackpar(trackpar),
    m_materialEffects(mef),
    m_jacobian{},
    m_derivs(nullptr),
    m_covariancematrix(nullptr),
    m_fitqual(nullptr),
    m_sinstereo(0),
    m_mType(TrackState::unidentified),
    m_recalib(false),
    m_owntp(false),
    m_ownmb(false),
    m_ownfq(false),
    m_owncov(false),
    m_measphi(false) {
    m_measerror[0] = m_measerror[1] = m_measerror[2] = m_measerror[3] = m_measerror[4] = -1;
    if (mef->sigmaDeltaTheta() == 0) {
      m_tsType = TrackState::Brem;
    }
  }

  Trk::GXFTrackState::~GXFTrackState() {
    if (m_ownmb) {
      delete m_measurement;
    }
    if (m_owntp) {
      delete m_trackpar;
    }
    delete m_materialEffects;
    if (m_owncov) {
      delete m_covariancematrix;
    }
    if (m_ownfq) {
      delete m_fitqual;
    }
  }

  GXFTrackState &
  GXFTrackState::operator = (GXFTrackState &rhs) {
    if (this != &rhs) {
      if (m_ownmb) {
        delete m_measurement;
      }
      if (m_owntp) {
        delete m_trackpar;
      }
      m_measurement = rhs.m_measurement ? (rhs.m_ownmb ? rhs.m_measurement->clone() : rhs.m_measurement) : 0;
      m_tsType = rhs.m_tsType;
      m_trackpar = rhs.m_trackpar;// ? (rhs.m_owntp ? rhs.m_trackpar->clone() : rhs.m_trackpar ) : 0;
      m_materialEffects = rhs.m_materialEffects ? new GXFMaterialEffects(*rhs.m_materialEffects) : 0;
      m_derivs = rhs.m_derivs;
      delete m_covariancematrix;
      m_covariancematrix =
        rhs.m_covariancematrix ? (rhs.m_owncov ? new AmgSymMatrix(5)(*rhs.m_covariancematrix) : rhs.m_covariancematrix)
        :
        0;

      m_fitqual = rhs.m_fitqual ? (rhs.m_ownfq ? new FitQualityOnSurface(*rhs.m_fitqual) : rhs.m_fitqual) : 0;
      m_sinstereo = rhs.m_sinstereo;
      m_mType = rhs.m_mType;
      m_recalib = rhs.m_recalib;
      m_owntp = false;// rhs.m_owntp;
      m_ownmb = rhs.m_ownmb;
      m_ownfq = rhs.m_ownfq;
      m_owncov = rhs.m_owncov;
      m_measphi = rhs.m_measphi;

      for (int i = 0; i < 5; i++) {
        m_measerror[i] = rhs.m_measerror[i];
        for (int j = 0; j < 5; j++) {
          m_jacobian[i][j] = rhs.m_jacobian[i][j];
        }
      }
    }
    return *this;
  }

  void
  GXFTrackState::setMeasurement(const MeasurementBase *meas) {
    if (m_ownmb) {
      delete m_measurement;
    }
    m_measurement = meas;
    m_recalib = true;
    m_ownmb = true;
  }

  const MeasurementBase *
  GXFTrackState::measurement(bool takeownership) {
    const MeasurementBase *meas = m_measurement;

    if (meas && !m_ownmb && takeownership) {
      meas = meas->clone();
    }
    if (takeownership) {
      m_ownmb = false;
    }
    return meas;
  }

  void
  GXFTrackState::setTrackParameters(const TrackParameters *par) {
    if (m_trackpar && m_owntp) {
      delete m_trackpar;
    }
    m_trackpar = par;
    m_owntp = true;
  }

  void
  GXFTrackState::setMaterialEffects(GXFMaterialEffects *mef) {
    if (m_materialEffects) {
      delete m_materialEffects;
    }
    m_materialEffects = mef;
  }

  void
  GXFTrackState::setJacobian(TransportJacobian *jac) {
    /*if (m_jacobian) {
       delete m_jacobian;
       }
       m_jacobian = jac;*/
    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 5; j++) {
        m_jacobian[i][j] = (*jac)(i, j);
      }
    }
    delete jac;
  }

  void
  GXFTrackState::setDerivatives(HepMatrix &deriv) {
    m_derivs = &deriv;
  }

  void
  GXFTrackState::setTrackStateType(TrackState::TrackStateType tstype) {
    m_tsType = tstype;
  }

  double *
  GXFTrackState::measurementErrors() {
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

  const Surface *
  GXFTrackState::surface() {
    if (m_measurement) {
      return &m_measurement->associatedSurface();
    } else if (m_trackpar) {
      return &m_trackpar->associatedSurface();
    } else if (m_materialEffects) {
      return m_materialEffects->surface();
    } else {
      return 0;
    }
  }

  void
  GXFTrackState::setTrackCovariance(AmgSymMatrix(5) *covmat) {
    if (m_covariancematrix && m_owncov) {
      delete m_covariancematrix;
    }
    m_covariancematrix = covmat;
    if (covmat) {
      m_owncov = true;
    }
  }

  void
  GXFTrackState::setFitQuality(const FitQualityOnSurface *fitqual) {
    if (m_fitqual && m_ownfq) {
      delete m_fitqual;
    }
    m_fitqual = fitqual;
    m_ownfq = true;
  }

  const FitQualityOnSurface *
  GXFTrackState::fitQuality(bool takeownership) {
    if (takeownership) {
      m_ownfq = false;
    }
    return m_fitqual;
  }

  int
  GXFTrackState::numberOfMeasuredParameters() {
    int nmeas = 0;

    if (m_tsType == TrackState::Fittable || m_tsType == TrackState::GeneralOutlier) {
      for (int i = 0; i < 5; i++) {
        if (m_measerror[i] > 0) {
          nmeas++;
        }
      }
    }
    // else if (m_tsType==TrackState::Scatterer) nmeas=2;
    return nmeas;
  }

  const Amg::Vector3D &
  GXFTrackState::position() {
    if (m_trackpar) {
      return m_trackpar->position();
    } else if (m_measurement) {
      return m_measurement->globalPosition();
    }
    // return surface()->center();  // Better than nothing...
    return m_globpos;
  }

  void
  GXFTrackState::setPosition(Amg::Vector3D &pos) {
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
}
