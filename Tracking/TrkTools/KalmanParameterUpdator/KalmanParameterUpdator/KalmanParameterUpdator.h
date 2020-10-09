/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   KalmanParameterUpdator.h
 * @date   31/07/2020
 * @author Anthony Morley, Christos Anastopoulos
 *
 * Implementation of the gain formalist for Kalman filters
 * allowing updating of parameters.
 *
 * Started from /based on KalmanUpdatorAmg
 */

#ifndef TRK_KALMANPARAMETERUPDATOR_H
#define TRK_KALMANPARAMETERUPDATOR_H

#include "EventPrimitives/EventPrimitives.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/ProjectionMatricesSet.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {

class KalmanParameterUpdator
{
public:
  enum RangeCheckDef
  {
    absoluteCheck = 0,
    differentialCheck = 1
  };

  KalmanParameterUpdator();

  /// calling the appropriate implementation for this rank of the measurement
  /// perform a filter step.
  /// Updates (in place) the current TrackParameters and FitQualityOnSurface
  /// to represent the predicted state containing
  /// the measurement LocalParameters and covariance.
  /// For prediction sign should be 1 for smoothing -1.
  bool filterStep(TrackParameters& trackParameters,
                  FitQualityOnSurface& fitQos,
                  const LocalParameters& measurement,
                  const Amg::MatrixX& measCovariance,
                  const int sign) const;

  bool stateFitQuality(FitQualityOnSurface& updatedFitQoS,
                       const TrackParameters& trkPar,
                       const LocalParameters& position,
                       const Amg::MatrixX& covariance,
                       int predFull) const;

private:
  /// 1D version
  bool calculateFilterStep_1D(TrackParameters& TP,
                              const AmgSymMatrix(5) & trkCov,
                              double measPar,
                              double measCov,
                              int paramKey,
                              int updateDirection,
                              FitQualityOnSurface& fQ) const;

  /// Templated version
  template<int DIM>
  bool calculateFilterStep_T(TrackParameters& TP,
                             const AmgSymMatrix(5) & trkCov,
                             const AmgVector(DIM) & measPar,
                             const AmgSymMatrix(DIM) & measCov,
                             int paramKey,
                             int updateDirection,
                             FitQualityOnSurface& fQ) const;

  /// 5D version
  bool calculateFilterStep_5D(TrackParameters& TP,
                              const AmgSymMatrix(5) & trkCov,
                              const AmgVector(5) & measPar,
                              const AmgSymMatrix(5) & measCov,
                              int updateDirection,
                              FitQualityOnSurface& fQ) const;
  /// Chi2 1D version
  bool makeChi2_1D(FitQualityOnSurface& updatedFitQoS,
                   const AmgVector(5) & trkPar,
                   const AmgSymMatrix(5) & trkCov,
                   double valRio,
                   double rioCov,
                   int paramKey,
                   int sign) const;

  /// Chi2 templated version
  template<int DIM>
  bool makeChi2_T(FitQualityOnSurface& updatedFitQoS,
                  const AmgVector(5) & trkPar,
                  const AmgSymMatrix(5) & trkCov,
                  const AmgVector(DIM) & measPar,
                  const AmgSymMatrix(DIM) & covPar,
                  int paramKey,
                  int sign) const;

  /// Avoid multiplications with sparse H matrices by cutting 2D rows&columns
  /// out of the full cov matrix.
  template<int DIM>
  AmgSymMatrix(DIM) projection_T(const AmgSymMatrix(5) &, int key) const;

  bool consistentParamDimensions(const LocalParameters&,
                                 const int nLocCoord) const;

  /** Absolute phi values should be in [-pi, pi]
      absolute theta values should be in [0, +pi]
      phi differences should also be in [-pi, pi] - else go other way round,
      theta differences should be smaller than pi but can be negative
      => other constraint than absolute theta.
  */
  bool thetaPhiWithinRange_5D(
    const AmgVector(5) & V,
    const KalmanParameterUpdator::RangeCheckDef rcd) const;

  //!< Test if theta angle is inside boundaries. No differential-check option.
  bool thetaWithinRange_5D(const AmgVector(5) & V) const;

  /// method correcting the calculated angles back to their defined ranges phi
  /// (-pi, pi) and theta (0, pi).
  bool correctThetaPhiRange_5D(
    AmgVector(5) &,
    AmgSymMatrix(5) &,
    const KalmanParameterUpdator::RangeCheckDef) const;

private:
  AmgVector(5) m_cov0Vec;
  AmgSymMatrix(5) m_unitMatrix;
  ProjectionMatricesSet m_reMatrices;
};

template<int DIM>
bool
KalmanParameterUpdator::calculateFilterStep_T(TrackParameters& TP,
                                              const AmgSymMatrix(5) & trkCov,
                                              const AmgVector(DIM) & measPar,
                                              const AmgSymMatrix(DIM) & measCov,
                                              int paramKey,
                                              int sign,
                                              FitQualityOnSurface& fQ) const
{
  // get the parameter vector
  const AmgVector(5)& trkPar = TP.parameters();
  // reduction matrix
  AmgMatrix(DIM, 5) H =
    m_reMatrices.expansionMatrix(paramKey).block<DIM, 5>(0, 0);
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
  const AmgMatrix(5, 5) M = m_unitMatrix - K * H;
  // --- compute local filtered state
  const AmgVector(5) newPar = trkPar + K * r;
  // --- compute filtered covariance matrix
  // C = M * trkCov * M.T() +/- K * covRio * K.T()
  const AmgSymMatrix(5) newCov =
    M * trkCov * M.transpose() + sign * K * measCov * K.transpose();
  const double chiSquared =
    (sign > 0) ? r.transpose() * R * r : r.transpose() * (-R) * r;
  // create the FQSonSurface
  fQ = FitQualityOnSurface(chiSquared, DIM);
  // In place update of parameters
  TP.updateParameters(newPar, newCov);
  return true;
}

template<int DIM>
bool
Trk::KalmanParameterUpdator::makeChi2_T(FitQualityOnSurface& updatedFitQoS,
                                        const AmgVector(5) & trkPar,
                                        const AmgSymMatrix(5) & trkCov,
                                        const AmgVector(DIM) & measPar,
                                        const AmgSymMatrix(DIM) & covPar,
                                        int paramKey,
                                        int sign) const
{ // sign: -1 = updated, +1 = predicted parameters.

  const AmgMatrix(DIM, 5) H =
    m_reMatrices.expansionMatrix(paramKey).block<DIM, 5>(0, 0);
  const AmgVector(DIM) r = measPar - H * trkPar;
  // get the projected matrix
  AmgSymMatrix(DIM) R = sign * projection_T<DIM>(trkCov, paramKey);
  R += covPar;
  // calcualte the chi2 value
  double chiSquared = 0.0;
  if (R.determinant() != 0.0) {
    chiSquared = r.transpose() * R.inverse() * r;
  }
  updatedFitQoS = FitQualityOnSurface(chiSquared, DIM);
  return true;
}

template<int DIM>
AmgSymMatrix(DIM) Trk::KalmanParameterUpdator::projection_T(
  const AmgSymMatrix(5) & M,
  int key) const
{
  if (key == 3 || key == 7 ||
      key == 15) { // shortcuts for the most common use cases
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

inline bool
KalmanParameterUpdator::thetaPhiWithinRange_5D(
  const AmgVector(5) & V,
  const KalmanParameterUpdator::RangeCheckDef rcd) const
{
  static const AmgVector(2) thetaMin(0.0, -M_PI);
  return ((std::abs(V(Trk::phi)) <= M_PI) && (V(Trk::theta) >= thetaMin(rcd)) &&
          (V(Trk::theta) <= M_PI));
}

inline bool
KalmanParameterUpdator::thetaWithinRange_5D(const AmgVector(5) & V) const
{
  return (V(Trk::theta) >= 0.0 && (V(Trk::theta) <= M_PI));
}

} // end of namespace Trk

#endif // TRK_KALMANPARAMETERUPDATOR_H
