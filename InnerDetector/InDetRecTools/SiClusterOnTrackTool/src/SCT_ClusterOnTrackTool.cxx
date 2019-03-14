/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class SCT_ClusterOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for SCT_ClusterOnTrack object production
///////////////////////////////////////////////////////////////////
// started 21/04/2004 I.Gavrilenko -- see ChangeLog
///////////////////////////////////////////////////////////////////

#include "SiClusterOnTrackTool/SCT_ClusterOnTrackTool.h"

#include "CxxUtils/checker_macros.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkRIO_OnTrack/check_cast.h"

#include <cmath>

using CLHEP::micrometer;
using CLHEP::deg;

ATLAS_CHECK_FILE_THREAD_SAFETY;

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SCT_ClusterOnTrackTool::SCT_ClusterOnTrackTool
  (const std::string &t, const std::string &n, const IInterface *p) :
  AthAlgTool(t, n, p),
  m_distortionsTool("SCT_DistortionsTool", this),
  m_option_make2dimBarrelClusters(false),
  m_doDistortions(false),
  m_option_errorStrategy(-1),
  m_option_correctionStrategy(-1) {
  // declareInterface<SCT_ClusterOnTrackTool>(this);
  declareInterface<IRIO_OnTrackCreator>(this);

  declareProperty("MakeTwoDimBarrelClusters", m_option_make2dimBarrelClusters,
                  "flag if strip length should be part of the measurement");
  declareProperty("ErrorStrategy", m_option_errorStrategy,
                  "if ErrorStrategy < 0, keep previous errors else recompute");
  declareProperty("CorrectionStrategy", m_option_correctionStrategy,
                  "if CorrectionStrategy >= 0, apply a correction to the cluster position");
  declareProperty("doDistortions", m_doDistortions,
                  "Simulation of module distortions");
  declareProperty("SCTDistortionsTool", m_distortionsTool,
                  "Tool to retrieve SCT distortions");
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode
InDet::SCT_ClusterOnTrackTool::initialize() {
  StatusCode sc = AlgTool::initialize();

  msg(MSG::INFO) << "A strategy to ";
  switch (m_option_errorStrategy) {
  case -1:  msg(MSG::INFO) << "keep the PRD errors";
    break;

  case  0:  msg(MSG::INFO) << "apply simple pitch errors";
    break;

  case  1:  msg(MSG::INFO) << "assign tuned SCT errors";
    break;

  case  2:  msg(MSG::INFO) << "assign tuned, angle-dependent SCT errors";
    break;

  default:  msg(MSG::INFO) << " -- NO, UNKNOWN. Pls check jobOptions!";
    break;
  }
  msg(MSG::INFO) << " will be applied during SCT_ClusterOnTrack making" << endmsg;
  if (m_option_correctionStrategy == 0) {
    msg(MSG::INFO) << "SCT cluster positions will be corrected" << endmsg;
  }

  // get the error scaling tool
  if (!m_sctErrorScalingKey.key().empty()) {
    ATH_CHECK(m_sctErrorScalingKey.initialize());
    ATH_MSG_DEBUG("Detected need for scaling sct errors.");
  }

  // Get ISCT_ModuleDistortionsTool
  if (m_distortionsTool.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Could not retrieve distortions tool: " << m_distortionsTool.name() << endmsg;
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_lorentzAngleTool.retrieve());

  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode
InDet::SCT_ClusterOnTrackTool::finalize() {
  StatusCode sc = AlgTool::finalize();

  return sc;
}

///////////////////////////////////////////////////////////////////
// Trk::SCT_ClusterOnTrack  production
///////////////////////////////////////////////////////////////////

const InDet::SCT_ClusterOnTrack *
InDet::SCT_ClusterOnTrackTool::correct
  (const Trk::PrepRawData &rio, const Trk::TrackParameters &trackPar) const {
  const InDet::SCT_Cluster *SC = 0;

  if (!(SC = dynamic_cast<const InDet::SCT_Cluster *> (&rio))) {
    return 0;
  }

  const InDet::SiWidth width = SC->width();
  const Amg::Vector2D &colRow = width.colRow();

  // Get pointer to detector element
  //
  const InDetDD::SiDetectorElement *EL = SC->detectorElement();
  if (!EL) {
    return 0;
  }
  IdentifierHash iH = EL->identifyHash();

  // Get local position of track
  //
  Amg::Vector2D loct = trackPar.localPosition();

  // Find phi angle of track relative to Lorentz drift direction, if needed
  //
  double dphi(0.);
  double sinAlpha = EL->sinStereoLocal(SC->localPosition());
  double cosAlpha = std::sqrt(1 - sinAlpha * sinAlpha);
  if (m_option_errorStrategy == 2 || m_option_correctionStrategy == 0) {
    double pNormal = trackPar.momentum().dot(EL->normal());
    double pPhi = trackPar.momentum().dot(Amg::AngleAxis3D(asin(-sinAlpha), Amg::Vector3D::UnitZ()) * EL->phiAxis());
    dphi = std::atan(pPhi / pNormal) - std::atan(m_lorentzAngleTool->getTanLorentzAngle(iH));
  }
  Amg::Vector3D localstripdir(-sinAlpha, cosAlpha, 0.);
  Amg::Vector3D globalstripdir = trackPar.associatedSurface().transform().linear() * localstripdir;
  double distance = (trackPar.position() - SC->globalPosition()).mag();
  const Trk::TrapezoidBounds *tbounds =
    dynamic_cast<const Trk::TrapezoidBounds *>(&trackPar.associatedSurface().bounds());
  const Trk::RectangleBounds *rbounds =
    dynamic_cast<const Trk::RectangleBounds *>(&trackPar.associatedSurface().bounds());

  if (!tbounds && !rbounds) {
    return 0;
  }

  double boundsy = rbounds ? rbounds->halflengthY() : tbounds->halflengthY();
  if (distance * cosAlpha > boundsy) {
    distance = boundsy / cosAlpha - 1;
  }
  // SCT_ClusterOnTrack production
  //
  Trk::LocalParameters locpar;
  if (loct.y() < 0) {
    distance = -distance;
  }
  Amg::Vector3D glob(SC->globalPosition() + distance * globalstripdir);
  Amg::MatrixX oldcov = SC->localCovariance();
  // Local position and error matrix production
  //
  // let's start to re-compute cluster error if errorStrategy >=0
  if (m_option_errorStrategy > -1) {
    Amg::MatrixX mat(2, 2);
    mat.setZero();
    switch (m_option_errorStrategy) {
    case 0:
      mat(0, 0) = std::pow(width.phiR(), 2) /12.;
      mat(1, 1) = std::pow(width.z(), 2) /12.;
      break;

    case 1:
      if (colRow.x() == 1) {
        mat(0, 0) = std::pow(1.05 * width.phiR(), 2) /12.;
      } else if (colRow.x() == 2) {
        mat(0, 0) = std::pow(0.27 * width.phiR(), 2) /12.;
      } else {
        mat(0, 0) = std::pow(width.phiR(), 2) /12.;
      }
      mat(1, 1) = std::pow(width.z() / colRow.y(), 2) /12.;
      break;

    case 2:
      mat(0, 0) = std::pow(getError(dphi, int(colRow.x())) * (EL->phiPitch() / 0.080), 2);
      mat(1, 1) = std::pow(width.z() / colRow.y(), 2) /12.;
      break;

    default:
      // don't do anything....
      break;
    }
    // rotation for endcap SCT
    if (EL->design().shape() == InDetDD::Trapezoid) {
      double sn = EL->sinStereoLocal(SC->localPosition());
      double sn2 = sn * sn;
      double cs2 = 1. - sn2;
      double w = EL->phiPitch(SC->localPosition()) / EL->phiPitch();
      double v0 = mat(0, 0) * w * w;
      double v1 = mat(1, 1);
      mat(0, 0) = (cs2 * v0 + sn2 * v1);
      mat(1, 0) = (sn * std::sqrt(cs2) * (v0 - v1));
      mat(0, 1) = mat(1, 0);
      mat(1, 1) = (sn2 * v0 + cs2 * v1);
    }
    oldcov = mat;
  }

  Amg::MatrixX cov(oldcov);
  if (EL->design().shape() != InDetDD::Trapezoid) {                     // barrel
    Trk::DefinedParameter lpos1dim(SC->localPosition().x(), Trk::locX);
    locpar = (m_option_make2dimBarrelClusters)       ?
             Trk::LocalParameters(SC->localPosition()) :// PRDformation does 2-dim
             Trk::LocalParameters(lpos1dim);
    if (!m_option_make2dimBarrelClusters) {
      cov = Amg::MatrixX(1, 1);
      cov(0, 0) = oldcov(0, 0);
    }

    if (!m_sctErrorScalingKey.key().empty()) {
      //SG::ReadCondHandle<SCTRIO_OnTrackErrorScaling> error_scaling( m_sctErrorScalingKey );
      SG::ReadCondHandle<RIO_OnTrackErrorScaling> error_scaling( m_sctErrorScalingKey );
      cov = check_cast<SCTRIO_OnTrackErrorScaling>(*error_scaling)->getScaledCovariance( cov,  false, 0.0);
    }
  }else {                                           // endcap
    locpar = Trk::LocalParameters(SC->localPosition());
    if (!m_sctErrorScalingKey.key().empty()) {
      //      SG::ReadCondHandle<SCTRIO_OnTrackErrorScaling> error_scaling( m_sctErrorScalingKey );
      SG::ReadCondHandle<RIO_OnTrackErrorScaling> error_scaling( m_sctErrorScalingKey );
      cov = check_cast<SCTRIO_OnTrackErrorScaling>(*error_scaling)->getScaledCovariance( cov,   true,
                                                EL->sinStereoLocal(SC->localPosition()));
    }
    double Sn = EL->sinStereoLocal(SC->localPosition());
    double Sn2 = Sn * Sn;
    double Cs2 = (1. - Sn) * (1. + Sn);
    double SC = Sn * std::sqrt(Cs2);
    double W = EL->phiPitch(loct) / EL->phiPitch();
    double dV0 = (Cs2 * cov(0, 0) + Sn2 * cov(1, 1) +
                  2. * SC * cov(1, 0)) * (W * W - 1.);
    cov(0, 0) += (Cs2 * dV0);
    cov(1, 0) += (SC * dV0);
    cov(0, 1) = cov(1, 0);
    cov(1, 1) += (Sn2 * dV0);
  }

  if (m_doDistortions) {
    if (EL->isBarrel() == 1) {// Only apply disortions to barrel modules
      locpar[Trk::locX] -= m_distortionsTool->correctReconstruction(trackPar, *EL, locpar, loct);
    }
  }


  // Apply correction for cluster position bias
  //
  if (m_option_correctionStrategy == 0) {
    double correction = getCorrection(dphi, int(colRow.x())) * EL->hitDepthDirection();
    locpar[Trk::locX] += correction;
  }
  bool isbroad = (m_option_errorStrategy == 0) ? true : false;
  return new InDet::SCT_ClusterOnTrack(SC, locpar, cov, iH, glob, isbroad);
}

double
InDet::SCT_ClusterOnTrackTool::getCorrection(double phi, int nstrip) const {
  float corr1[30] = {
    0.3, 0.8, 1.1, 1.5, 1.9, 1.9, 2.1, 2.4, 2.3, 2.6,
    2.6, 2.7, 2.8, 2.7, 2.5, 2.6, 2.8, 2.6, 2.6, 2.7,
    2.2, 1.8, 1.8, 1.6, 1.5, 0.0, 0.0, 0.0, 0.0, 0.0
  };
  float corr2[30] = {
    0.0, 0.0, 0.0, 1.0, 1.5, 1.7, 1.7, 2.3, 2.1, 2.5,
    2.5, 2.7, 2.7, 2.9, 3.0, 3.0, 3.0, 3.0, 3.4, 3.4,
    3.0, 3.2, 2.6, 2.6, 3.0, 2.7, 2.5, 2.4, 1.7, 1.3
  };

  // Phi bins have 1 degree width, and cover 0-30 degrees
  int phiBin = int(std::fabs(phi) / deg);

  float correction(0.);

  if (phiBin < 30) {
    if (nstrip == 1) {
      correction = corr1[phiBin];
    }
    if (nstrip == 2) {
      correction = corr2[phiBin];
    }
  }

  if (phi > 0.) {
    correction *= -1.;
  }

  return correction * micrometer;
}

double
InDet::SCT_ClusterOnTrackTool::getError(double phi, int nstrip) const {
  float sigma1[60] = {
    22.1, 21.8, 21.4, 21.0, 20.5, 20.0, 19.6, 19.1, 18.5, 18.0,
    17.4, 17.0, 16.4, 15.8, 15.4, 14.9, 14.4, 14.1, 13.3, 13.1,
    12.9, 12.4, 12.6, 12.2, 12.3, 12.6, 13.4, 14.2, 15.6, 19.3,
    22.8, 29.5, 33.2, 41.8, 44.3, 48.4, 49.9, 54.0, 53.0, 56.3,
    57.5, 56.3, 64.5, 65.7, 66.1, 69.4, 74.8, 78.3, 78.8, 79.8,
    73.5, 73.8, 75.8, 84.3, 87.0, 99.9, 86.3, 0.0, 0.0, 0.0
  };
  float sigma2[60] = {
    22.2, 20.3, 18.8, 16.0, 14.6, 13.8, 12.9, 12.9, 12.7, 12.3,
    12.7, 12.6, 13.0, 13.3, 14.0, 14.6, 15.3, 15.9, 16.6, 17.6,
    18.4, 19.3, 19.9, 20.5, 21.0, 21.2, 21.5, 21.4, 21.3, 21.3,
    20.9, 20.8, 20.6, 20.7, 20.3, 20.7, 21.7, 24.4, 26.5, 29.5,
    34.6, 41.6, 48.5, 52.3, 54.5, 58.4, 61.8, 66.7, 69.9, 72.1,
    78.9, 79.2, 81.8, 80.9, 87.5, 99.2, 0.0, 0.0, 0.0, 0.0
  };
  float sigma3[60] = {
    70.1, 73.6, 71.7, 66.9, 68.3, 66.8, 66.2, 64.8, 66.6, 63.3,
    63.3, 60.4, 59.0, 57.1, 56.4, 54.4, 54.2, 54.4, 50.3, 48.9,
    48.1, 41.9, 38.0, 31.8, 28.3, 23.1, 23.0, 20.3, 18.5, 17.6,
    17.7, 16.8, 18.3, 19.3, 19.0, 20.0, 20.9, 21.6, 22.0, 22.2,
    22.7, 22.4, 24.3, 24.8, 24.6, 27.0, 29.8, 37.0, 47.7, 49.3,
    58.2, 60.2, 66.8, 70.8, 77.3, 80.6, 0.0, 0.0, 0.0, 0.0
  };
  float sigma4[60] = {
    103.2, 100.4, 100.7, 101.2, 107.4, 100.6, 100.9, 100.4, 96.3, 98.2,
    96.7, 94.5, 96.9, 91.7, 90.5, 89.5, 86.3, 90.6, 82.4, 89.3,
    87.3, 77.6, 75.7, 77.2, 77.3, 84.1, 80.1, 66.9, 73.7, 72.3,
    58.1, 65.6, 64.2, 54.7, 47.2, 44.4, 34.6, 36.4, 29.1, 25.8,
    18.8, 21.6, 18.6, 20.3, 22.7, 23.3, 24.1, 22.4, 24.7, 24.7,
    27.3, 30.4, 37.0, 46.4, 59.4, 62.6, 65.3, 0.0, 0.0, 0.0
  };
  float sigma5[60] = {
    150.9, 139.7, 133.9, 139.8, 141.4, 134.9, 138.4, 129.3, 137.9, 128.7,
    132.4, 130.1, 124.2, 115.8, 131.4, 115.2, 128.7, 112.8, 130.7, 129.0,
    115.8, 101.3, 115.9, 116.1, 121.7, 109.9, 110.0, 97.2, 96.4, 107.3,
    98.2, 80.0, 73.2, 87.0, 97.0, 88.5, 72.2, 73.9, 80.8, 75.7,
    69.5, 67.1, 54.1, 58.9, 47.3, 50.6, 29.5, 26.6, 25.8, 20.9,
    20.6, 21.9, 22.1, 21.1, 27.9, 41.6, 0.0, 0.0, 0.0, 0.0
  };

  // Phi bins have 1 degree width, and cover 0-60 degrees
  int phiBin = int(std::fabs(phi) / deg);

  float sigma(0.);

  if (phiBin < 60) {
    if (nstrip == 1) {
      sigma = sigma1[phiBin];
    }
    if (nstrip == 2) {
      sigma = sigma2[phiBin];
    }
    if (nstrip == 3) {
      sigma = sigma3[phiBin];
    }
    if (nstrip == 4) {
      sigma = sigma4[phiBin];
    }
    if (nstrip == 5) {
      sigma = sigma5[phiBin];
    }
  }
  if (sigma < 0.1) {
    sigma = std::max(100., float(nstrip) * 80. / std::sqrt(12));
  }

  return sigma * micrometer;
}
