/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
  bool filterStep(
    TrackParameters& trackParameters,
    FitQualityOnSurface& fitQos,
    const LocalParameters& measurement,
    const Amg::MatrixX& measCovariance,
    const int sign) const;

  bool stateFitQuality(
    FitQualityOnSurface& updatedFitQoS,
    const TrackParameters& trkPar,
    const LocalParameters& position,
    const Amg::MatrixX& covariance,
    int predFull) const;

private:
  /// 1D version
  bool calculateFilterStep_1D(
    TrackParameters& TP,
    const AmgSymMatrix(5) & trkCov,
    double measPar,
    double measCov,
    int paramKey,
    int updateDirection,
    FitQualityOnSurface& fQ) const;

  /// Templated version
  template<int DIM>
  bool calculateFilterStep_T(
    TrackParameters& TP,
    const AmgSymMatrix(5) & trkCov,
    const AmgVector(DIM) & measPar,
    const AmgSymMatrix(DIM) & measCov,
    int paramKey,
    int updateDirection,
    FitQualityOnSurface& fQ) const;

  /// 5D version
  bool calculateFilterStep_5D(
    TrackParameters& TP,
    const AmgSymMatrix(5) & trkCov,
    const AmgVector(5) & measPar,
    const AmgSymMatrix(5) & measCov,
    int updateDirection,
    FitQualityOnSurface& fQ) const;

  /// Chi2 templated version
  template<int DIM>
  bool makeChi2_T(
    FitQualityOnSurface& updatedFitQoS,
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

private:
  AmgVector(5) m_cov0Vec;
  AmgSymMatrix(5) m_unitMatrix;
  ProjectionMatricesSet m_reMatrices;
};

} // end of namespace Trk

#include "TrkGaussianSumFilterUtils/KalmanParameterUpdator.icc"
#endif // TRK_KALMANPARAMETERUPDATOR_H
