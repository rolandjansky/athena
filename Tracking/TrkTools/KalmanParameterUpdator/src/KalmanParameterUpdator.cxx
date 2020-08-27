/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "KalmanParameterUpdator/KalmanParameterUpdator.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/ParamDefs.h"

#include <cmath>

namespace {
constexpr double s_thetaGainDampingValue = 0.1;
}

Trk::KalmanParameterUpdator::KalmanParameterUpdator()
  : m_unitMatrix(AmgMatrix(5, 5)::Identity())
  , m_reMatrices(5)
{

  m_cov0Vec << 250., 250., 0.25, 0.25, 0.000001;
}
bool
Trk::KalmanParameterUpdator::filterStep(TrackParameters& trackParameters,
                                        FitQualityOnSurface& fitQos,
                                        const LocalParameters& measurement,
                                        const Amg::MatrixX& measCovariance,
                                        const int sign) const
{

  const AmgSymMatrix(5)* trkCov = trackParameters.covariance();
  if (!trkCov) {
    return false;
  }

  int nLocCoord = measCovariance.cols();
  if (!consistentParamDimensions(measurement, nLocCoord)) {
    return false;
  }

  switch (nLocCoord) {
    case 1: {
      return calculateFilterStep_1D(trackParameters,
                                    *trkCov,
                                    measurement(0),
                                    measCovariance(0, 0),
                                    measurement.parameterKey(),
                                    sign,
                                    fitQos);
    }
    case 2: {
      return calculateFilterStep_T<2>(trackParameters,
                                      *trkCov,
                                      measurement.block<2, 1>(0, 0),
                                      measCovariance.block<2, 2>(0, 0),
                                      measurement.parameterKey(),
                                      sign,
                                      fitQos);
    }
    case 3: {
      return calculateFilterStep_T<3>(trackParameters,
                                      *trkCov,
                                      measurement.block<3, 1>(0, 0),
                                      measCovariance.block<3, 3>(0, 0),
                                      measurement.parameterKey(),
                                      sign,
                                      fitQos);
    }
    case 4: {
      return calculateFilterStep_T<4>(trackParameters,
                                      *trkCov,
                                      measurement.block<4, 1>(0, 0),
                                      measCovariance.block<4, 4>(0, 0),
                                      measurement.parameterKey(),
                                      sign,
                                      fitQos);
    }
    case 5: {
      return calculateFilterStep_5D(trackParameters,
                                    *trkCov,
                                    measurement.block<5, 1>(0, 0),
                                    measCovariance.block<5, 5>(0, 0),
                                    sign,
                                    fitQos);
    }
    default: {
      return false;
    }
  }
}
bool
Trk::KalmanParameterUpdator::stateFitQuality(FitQualityOnSurface& updatedFitQoS,
                                             const TrackParameters& trkPar,
                                             const LocalParameters& position,
                                             const Amg::MatrixX& covariance,
                                             int predFull) const
{
  if (!trkPar.covariance()) {
    return false;
  }
  // For the LocalPos. version, need to get # meas. coord. from covariance
  // matrix.
  int nLocCoord = covariance.cols();
  switch (nLocCoord) {
    case 1: {
      return makeChi2_1D(updatedFitQoS,
                         trkPar.parameters(),
                         (*trkPar.covariance()),
                         position[Trk::locX],
                         covariance(0, 0),
                         1,
                         predFull);
    }
    case 2: {
      return makeChi2_T<2>(updatedFitQoS,
                           trkPar.parameters(),
                           (*trkPar.covariance()),
                           position,
                           covariance.block<2, 2>(0, 0),
                           3,
                           predFull);
    }
    default: {
      return false;
    }
  }
}

// calculations for Kalman updator and inverse Kalman filter
bool
Trk::KalmanParameterUpdator::calculateFilterStep_1D(
  TrackParameters& TP,
  const AmgSymMatrix(5) & trkCov,
  double measPar,
  double measCov,
  int paramKey,
  int sign,
  Trk::FitQualityOnSurface& fitQoS) const
{

  int mk = 0;
  if (paramKey != 1) {
    for (int i = 0; i < 5; ++i) {
      if (paramKey & (1 << i)) {
        mk = i;
        break;
      }
    }
  }
  // get the parameters from the
  const AmgVector(5)& trkPar = TP.parameters();
  // use measuring coordinate (variable "mk") instead of reduction matrix
  double r = measPar - trkPar(mk);
  double R = (sign * measCov) + trkCov(mk, mk);
  if (R == 0.0) {
    return false;
  }
  R = 1. / R;
  // --- compute Kalman gain matrix
  AmgMatrix(5, 1) K = trkCov.block<5, 1>(0, mk) * R;
  // --- compute local filtered state, here = TP+K*r = TP + TCov * H.T * R * r
  AmgVector(5) newPar = trkPar + trkCov.col(mk) * R * r;

  if (!thetaWithinRange_5D(newPar)) {

    if (mk != Trk::theta &&
        (std::abs(R * r) > 1.0 ||
         trkCov(Trk::theta, Trk::theta) > 0.1 * m_cov0Vec(Trk::theta))) {

      AmgVector(5) dampedCov = trkCov.col(mk);

      dampedCov(Trk::theta) = dampedCov(Trk::theta) * s_thetaGainDampingValue;

      newPar = trkPar + dampedCov * R * r;

      K(Trk::theta, 0) = K(Trk::theta, 0) * s_thetaGainDampingValue;
    }
  }
  // --- compute covariance matrix of local filteres state
  AmgMatrix(5, 5) KtimesH;
  KtimesH.setZero();
  KtimesH.col(mk) = K;
  AmgMatrix(5, 5) M = m_unitMatrix - KtimesH;
  AmgSymMatrix(5) newCov =
    M * trkCov * M.transpose() + sign * K * measCov * K.transpose();

  if ((!thetaPhiWithinRange_5D(newPar,
                               Trk::KalmanParameterUpdator::absoluteCheck))
        ? !correctThetaPhiRange_5D(
            newPar, newCov, Trk::KalmanParameterUpdator::absoluteCheck)
        : false) {
    return false;
  }

  double predictedResidual = (sign < 0) ? r : (measPar - newPar(mk));
  const AmgSymMatrix(5)& updatedCov =
    (sign < 0) ? trkCov : // when removing, the input are updated par
      newCov;             // when adding, chi2 is made from upd. par

  // for both signs (add/remove) the chi2 is now calculated like for updated
  // states
  double chiSquared = measCov - updatedCov(mk, mk);
  if (chiSquared != 0.0) {
    // get chi2 = r.T() * R^-1 * r
    chiSquared = predictedResidual * predictedResidual / chiSquared;
  }
  fitQoS = FitQualityOnSurface(chiSquared, 1);
  // In place update of parameters
  TP.updateParameters(newPar, newCov);
  return true;
}

bool
Trk::KalmanParameterUpdator::calculateFilterStep_5D(
  TrackParameters& TP,
  const AmgSymMatrix(5) & trkCov,
  const AmgVector(5) & measPar,
  const AmgSymMatrix(5) & measCov,
  int sign,
  FitQualityOnSurface& fQ) const
{
  // get the parameter vector
  const AmgVector(5)& trkPar = TP.parameters();
  // Kalman gain K, residual r, combined covariance R
  const AmgVector(5) r = measPar - trkPar;
  // for full safety in Eigen see
  // http://eigen.tuxfamily.org/dox/classEigen_1_1FullPivLU.html
  const AmgSymMatrix(5) R = (sign * measCov + trkCov).inverse();
  const AmgMatrix(5, 5) K = trkCov * R;
  const AmgMatrix(5, 5) M = m_unitMatrix - K;
  // --- compute local filtered state
  AmgVector(5) newPar = trkPar + K * r;
  // --- compute filtered covariance matrix
  const AmgSymMatrix(5) newCov =
    trkCov.similarity(M) + sign * measCov.similarity(K);
  double chiSquared =
    (sign > 0) ? r.transpose() * R * r : r.transpose() * (-R) * r;
  // create the FQSonSurface
  fQ = FitQualityOnSurface(chiSquared, 5);
  TP.updateParameters(newPar, newCov);
  return true;
}

bool
Trk::KalmanParameterUpdator::makeChi2_1D(FitQualityOnSurface& updatedFitQoS,
                                         const AmgVector(5) & trkPar,
                                         const AmgSymMatrix(5) & trkCov,
                                         double valRio,
                                         double rioCov,
                                         int paramKey,
                                         int sign) const
{
  int mk = 0;
  if (paramKey != 1) {
    for (int i = 0; i < 5; ++i) {
      if (paramKey & (1 << i)) {
        mk = i;
        break;
      }
    }
  }
  // sign: -1 = updated, +1 = predicted parameters.
  double r = valRio - trkPar(mk);
  //  if (mk==3) catchPiPi;
  double chiSquared = rioCov + sign * trkCov(mk, mk);
  if (chiSquared == 0.0) {
    return false;
  }
  chiSquared = r * r / chiSquared;
  updatedFitQoS = FitQualityOnSurface(chiSquared, 1);
  return true;
}

bool
Trk::KalmanParameterUpdator::consistentParamDimensions(
  const Trk::LocalParameters& P,
  const int dimCov) const
{
  return P.dimension() == dimCov;
}

bool
Trk::KalmanParameterUpdator::correctThetaPhiRange_5D(
  AmgVector(5) & V,
  AmgSymMatrix(5) & M,
  const KalmanParameterUpdator::RangeCheckDef rcd) const
{
  static const AmgVector(2) thetaMin(0.0, -M_PI);

  // correct theta coordinate
  if (V(Trk::theta) < thetaMin((int)rcd) || V(Trk::theta) > M_PI) {
    // absolute theta: repair if between -pi and +2pi.
    // differential theta: repair if between -pi and +pi
    if ((V(Trk::theta) < -M_PI) ||
        (V(Trk::theta) > (rcd == Trk::KalmanParameterUpdator::differentialCheck
                            ? M_PI
                            : 2 * M_PI))) {
      return false;
    }
    if (V(Trk::theta) > M_PI) {
      V(Trk::theta) = 2 * M_PI - V(Trk::theta);
      V(Trk::phi) += (V(Trk::phi) > 0.0) ? -M_PI : M_PI;
    } else if (V(Trk::theta) < 0.0) {
      V(Trk::theta) = -V(Trk::theta);
      V(Trk::phi) += (V(Trk::phi) > 0.0) ? -M_PI : M_PI;
    }
    // correct also cov matrix:
    M.fillSymmetric(0, 3, -M(0, 3)); // cov(polar,locX)
    M.fillSymmetric(1, 3, -M(1, 3)); // cov(polar,locY)
    M.fillSymmetric(2, 3, -M(2, 3)); // cov(polar,azimuthal)
    M.fillSymmetric(3, 4, -M(3, 4)); // cov(polar,qOverP)
  }

  // correct phi coordinate if necessary
  if ((V(Trk::phi) > M_PI)) {
    V(Trk::phi) = fmod(V(Trk::phi) + M_PI, 2 * M_PI) - M_PI;
  } else if ((V(Trk::phi) < -M_PI)) {
    V(Trk::phi) = fmod(V(Trk::phi) - M_PI, 2 * M_PI) + M_PI;
  }

  return true;
}
