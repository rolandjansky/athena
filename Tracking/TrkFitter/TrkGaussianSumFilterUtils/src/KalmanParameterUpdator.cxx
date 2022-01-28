/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkGaussianSumFilterUtils/KalmanParameterUpdator.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include <cmath>

namespace {

// constants
constexpr double s_thetaGainDampingValue = 0.1;
const AmgVector(5) s_cov0Vec = [] {
  AmgVector(5) tmp;
  tmp << 250., 250., 0.25, 0.25, 0.000001;
  return tmp;
}();
const AmgMatrix(5, 5) s_unitMatrix(AmgMatrix(5, 5)::Identity());
const AmgVector(2) thetaMin(0.0, -M_PI);
enum RangeCheckDef
{
  absoluteCheck = 0,
  differentialCheck = 1
};

/**
 * Avoid multiplications with sparse H matrices by cutting 2D rows&columns
 * out of the full cov matrix.
 */
template<int DIM>
AmgSymMatrix(DIM) projection_T(const AmgSymMatrix(5) & M, int key)
{
  if (key == 3 || key == 7 || key == 15) { // shortcuts for the most common use
                                           // cases
    return M.block<DIM, DIM>(0, 0);
  } else {
    Eigen::Matrix<int, DIM, 1, 0, DIM, 1> iv;
    iv.setZero();
    for (int i = 0, k = 0; i < 5; ++i) {
      if (key & (1 << i))
        iv[k++] = i;
    }
    AmgSymMatrix(DIM) covSubMatrix;
    covSubMatrix.setZero();
    for (int i = 0; i < DIM; ++i) {
      for (int j = 0; j < DIM; ++j) {
        covSubMatrix(i, j) = M(iv(i), iv(j));
      }
    }
    return covSubMatrix;
  }
}

/** Absolute phi values should be in [-pi, pi]
    absolute theta values should be in [0, +pi]
    phi differences should also be in [-pi, pi] - else go other way round,
    theta differences should be smaller than pi but can be negative
    => other constraint than absolute theta.
*/
bool
thetaPhiWithinRange_5D(const AmgVector(5) & V, const RangeCheckDef rcd)
{
  static const AmgVector(2) thetaMin(0.0, -M_PI);
  return ((std::abs(V(Trk::phi)) <= M_PI) && (V(Trk::theta) >= thetaMin(rcd)) &&
          (V(Trk::theta) <= M_PI));
}

/**
 * Test if theta angle is inside boundaries. No differential-check option.
 */
bool
thetaWithinRange_5D(const AmgVector(5) & V)
{
  return (V(Trk::theta) >= 0.0 && (V(Trk::theta) <= M_PI));
}

bool
correctThetaPhiRange_5D(AmgVector(5) & V,
                        AmgSymMatrix(5) & M,
                        const RangeCheckDef rcd)
{

  // correct theta coordinate
  if (V(Trk::theta) < thetaMin((int)rcd) || V(Trk::theta) > M_PI) {
    // absolute theta: repair if between -pi and +2pi.
    // differential theta: repair if between -pi and +pi
    if ((V(Trk::theta) < -M_PI) ||
        (V(Trk::theta) > (rcd == differentialCheck ? M_PI : 2 * M_PI))) {
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

/**
 * calculations for Kalman updator and inverse Kalman filter
 */
bool
calculateFilterStep_1D(Trk::TrackParameters& TP,
                       const AmgSymMatrix(5) & trkCov,
                       double measPar,
                       double measCov,
                       int paramKey,
                       int sign,
                       Trk::FitQualityOnSurface& fitQoS)
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
  const double r = measPar - trkPar(mk);
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
         trkCov(Trk::theta, Trk::theta) > 0.1 * s_cov0Vec(Trk::theta))) {

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
  AmgMatrix(5, 5) M = s_unitMatrix - KtimesH;
  AmgSymMatrix(5) newCov =
    M * trkCov * M.transpose() + sign * K * measCov * K.transpose();

  if ((!thetaPhiWithinRange_5D(newPar, absoluteCheck))
        ? !correctThetaPhiRange_5D(newPar, newCov, absoluteCheck)
        : false) {
    return false;
  }

  const double predictedResidual = (sign < 0) ? r : (measPar - newPar(mk));
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
  fitQoS = Trk::FitQualityOnSurface(chiSquared, 1);
  // In place update of parameters
  TP.updateParameters(newPar, newCov);
  return true;
}

bool
calculateFilterStep_5D(Trk::TrackParameters& TP,
                       const AmgSymMatrix(5) & trkCov,
                       const AmgVector(5) & measPar,
                       const AmgSymMatrix(5) & measCov,
                       int sign,
                       Trk::FitQualityOnSurface& fQ)
{
  // get the parameter vector
  const AmgVector(5)& trkPar = TP.parameters();
  // Kalman gain K, residual r, combined covariance R
  const AmgVector(5) r = measPar - trkPar;
  // for full safety in Eigen see
  // http://eigen.tuxfamily.org/dox/classEigen_1_1FullPivLU.html
  const AmgSymMatrix(5) R = (sign * measCov + trkCov).inverse();
  const AmgMatrix(5, 5) K = trkCov * R;
  const AmgMatrix(5, 5) M = s_unitMatrix - K;
  // --- compute local filtered state
  const AmgVector(5) newPar = trkPar + K * r;
  // --- compute filtered covariance matrix
  const AmgSymMatrix(5) newCov =
    trkCov.similarity(M) + sign * measCov.similarity(K);
  double chiSquared =
    (sign > 0) ? r.transpose() * R * r : r.transpose() * (-R) * r;
  // create the FQSonSurface
  fQ = Trk::FitQualityOnSurface(chiSquared, 5);
  TP.updateParameters(newPar, newCov);
  return true;
}

template<int DIM>
bool
calculateFilterStep_T(Trk::TrackParameters& TP,
                      const AmgSymMatrix(5) & trkCov,
                      const AmgVector(DIM) & measPar,
                      const AmgSymMatrix(DIM) & measCov,
                      int paramKey,
                      int sign,
                      Trk::FitQualityOnSurface& fQ)
{

  // get the parameter vector
  const AmgVector(5)& trkPar = TP.parameters();
  static const Trk::ProjectionMatricesSet s_reMatrices(5);
  // reduction matrix
  AmgMatrix(DIM, 5) H =
    s_reMatrices.expansionMatrix(paramKey).block<DIM, 5>(0, 0);
  // the projected parameters from the TrackParameters
  AmgVector(DIM) projTrkPar;
  if (paramKey == 3 || paramKey == 7 || paramKey == 15) {
    projTrkPar = trkPar.block<DIM, 1>(0, 0);
  } else {
    projTrkPar = H * trkPar;
  }

  // reduction matrix H, Kalman gain K, residual r, combined covariance R
  // residual after reduction
  const AmgVector(DIM) r = measPar - projTrkPar;
  // combined covariance after reduction
  const AmgSymMatrix(DIM) R =
    (sign * measCov + projection_T<DIM>(trkCov, paramKey)).inverse();
  // Kalman gain matrix
  const AmgMatrix(5, DIM) K = trkCov * H.transpose() * R;
  const AmgMatrix(5, 5) M = s_unitMatrix - K * H;
  // --- compute local filtered state
  const AmgVector(5) newPar = trkPar + K * r;
  // --- compute filtered covariance matrix
  // C = M * trkCov * M.T() +/- K * covRio * K.T()
  const AmgSymMatrix(5) newCov =
    M * trkCov * M.transpose() + sign * K * measCov * K.transpose();
  const double chiSquared =
    (sign > 0) ? r.transpose() * R * r : r.transpose() * (-R) * r;
  // create the FQSonSurface
  fQ = Trk::FitQualityOnSurface(chiSquared, DIM);
  // In place update of parameters
  TP.updateParameters(newPar, newCov);
  return true;
}

/*
 * chi2 methods
 */
bool
makeChi2_1D(Trk::FitQualityOnSurface& updatedFitQoS,
            const AmgVector(5) & trkPar,
            const AmgSymMatrix(5) & trkCov,
            double valRio,
            double rioCov,
            int paramKey,
            int sign)
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
  updatedFitQoS = Trk::FitQualityOnSurface(chiSquared, 1);
  return true;
}

template<int DIM>
bool
makeChi2_T(Trk::FitQualityOnSurface& updatedFitQoS,
           const AmgVector(5) & trkPar,
           const AmgSymMatrix(5) & trkCov,
           const AmgVector(DIM) & measPar,
           const AmgSymMatrix(DIM) & covPar,
           int paramKey,
           int sign)

{ // sign: -1 = updated, +1 = predicted parameters.
  static const Trk::ProjectionMatricesSet s_reMatrices(5);
  const AmgMatrix(DIM, 5) H =
    s_reMatrices.expansionMatrix(paramKey).block<DIM, 5>(0, 0);
  const AmgVector(DIM) r = measPar - H * trkPar;
  // get the projected matrix
  AmgSymMatrix(DIM) R = sign * projection_T<DIM>(trkCov, paramKey);
  R += covPar;
  // calcualte the chi2 value
  double chiSquared = 0.0;
  if (R.determinant() != 0.0) {
    chiSquared = r.transpose() * R.inverse() * r;
  }
  updatedFitQoS = Trk::FitQualityOnSurface(chiSquared, DIM);
  return true;
}

bool
consistentParamDimensions(const Trk::LocalParameters& P, const int dimCov)
{
  return P.dimension() == dimCov;
}

} // end of anonymous namespace

bool
Trk::KalmanParameterUpdator::filterStep(TrackParameters& trackParameters,
                                        Trk::FitQualityOnSurface& fitQos,
                                        const LocalParameters& measurement,
                                        const Amg::MatrixX& measCovariance,
                                        const int sign)
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
Trk::KalmanParameterUpdator::stateFitQuality(
  Trk::FitQualityOnSurface& updatedFitQoS,
  const TrackParameters& trkPar,
  const LocalParameters& position,
  const Amg::MatrixX& covariance,
  int predFull)
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

