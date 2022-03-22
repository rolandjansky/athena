/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file   GsfMeasurementUpdator.cxx
 * @date   Friday 25th February 2005
 * @author Tom Athkinson, Anthony Morley, Christos Anastopoulos
 * @brief  Implementation code for GsfMeasurementUpdator class
 */

#include "TrkGaussianSumFilterUtils/GsfMeasurementUpdator.h"
#include "TrkGaussianSumFilterUtils/MultiComponentStateAssembler.h"
#include "TrkGaussianSumFilterUtils/PosteriorWeightsCalculator.h"
//
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include <memory>

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

bool
filterStep(Trk::TrackParameters& trackParameters,
           Trk::FitQualityOnSurface& fitQos,
           const Trk::LocalParameters& measurement,
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
stateFitQuality(Trk::FitQualityOnSurface& updatedFitQoS,
                const Trk::TrackParameters& trkPar,
                const Trk::LocalParameters& position,
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

bool
invalidComponent(const Trk::TrackParameters* trackParameters)
{
  const auto* measuredCov = trackParameters->covariance();
  bool rebuildCov = false;
  if (!measuredCov) {
    rebuildCov = true;
  } else {
    for (int i(0); i < 5; ++i) {
      if ((*measuredCov)(i, i) <= 0.) {
        rebuildCov = true;
      }
    }
  }
  return rebuildCov;
}

Trk::MultiComponentState
rebuildState(Trk::MultiComponentState&& stateBeforeUpdate)
{
  Trk::MultiComponentState stateWithInsertedErrors =
    std::move(stateBeforeUpdate);
  // We need to loop checking for invalid componets i.e negative covariance
  // diagonal elements and update them with a large covariance matrix
  for (auto& component : stateWithInsertedErrors) {
    const Trk::TrackParameters* trackParameters = component.first.get();
    const bool rebuildCov = invalidComponent(trackParameters);
    if (rebuildCov) {
      AmgSymMatrix(5) bigNewCovarianceMatrix = AmgSymMatrix(5)::Zero();
      const double covarianceScaler = 1.;
      bigNewCovarianceMatrix(0, 0) = 250. * covarianceScaler;
      bigNewCovarianceMatrix(1, 1) = 250. * covarianceScaler;
      bigNewCovarianceMatrix(2, 2) = 0.25;
      bigNewCovarianceMatrix(3, 3) = 0.25;
      bigNewCovarianceMatrix(4, 4) = 0.001 * 0.001;
      component.first->updateParameters(trackParameters->parameters(),
                                        bigNewCovarianceMatrix);
    }
  }
  return stateWithInsertedErrors;
}

Trk::MultiComponentState
calculateFilterStep(Trk::MultiComponentState&& stateBeforeUpdate,
                    const Trk::MeasurementBase& measurement,
                    int addRemoveFlag)
{
  if (stateBeforeUpdate.empty()) {
    return {};
  }

  // state Assembler cache
  Trk::MultiComponentStateAssembler::Cache cache;

  // Calculate the weight of each component after the measurement
  Trk::MultiComponentState stateWithNewWeights =
    Trk::PosteriorWeightsCalculator::weights(std::move(stateBeforeUpdate),
                                             measurement);

  if (stateWithNewWeights.empty()) {
    return {};
  }

  for (Trk::ComponentParameters& component : stateWithNewWeights) {

    Trk::FitQualityOnSurface fitQuality{};
    /// Update the component in place
    bool updateSuccess = filterStep(*(component.first),
                                    fitQuality,
                                    measurement.localParameters(),
                                    measurement.localCovariance(),
                                    addRemoveFlag);

    // If we fail we need to erase the element
    if (!updateSuccess || fitQuality.chiSquared() <= 0.) {
      continue;
    }
    // Move component to state being prepared for assembly
    Trk::MultiComponentStateAssembler::addComponent(cache,
                                                    std::move(component));
  }

  Trk::MultiComponentState assembledUpdatedState =
    Trk::MultiComponentStateAssembler::assembledState(std::move(cache));

  if (assembledUpdatedState.empty()) {
    return {};
  }
  // Renormalise state
  Trk::MultiComponentStateHelpers::renormaliseState(assembledUpdatedState);
  return assembledUpdatedState;
}

Trk::MultiComponentState
calculateFilterStep(Trk::MultiComponentState&& stateBeforeUpdate,
                    const Trk::MeasurementBase& measurement,
                    Trk::FitQualityOnSurface& fitQoS)
{
  // state Assembler cache
  Trk::MultiComponentStateAssembler::Cache cache;
  if (stateBeforeUpdate.empty()) {
    return {};
  }

  // Calculate the weight of each component after the measurement
  Trk::MultiComponentState stateWithNewWeights =
    Trk::PosteriorWeightsCalculator::weights(std::move(stateBeforeUpdate),
                                             measurement);

  if (stateWithNewWeights.empty()) {
    return {};
  }

  double chiSquared = 0;
  int degreesOfFreedom = 0;
  for (Trk::ComponentParameters& component : stateWithNewWeights) {
    if (stateWithNewWeights.size() > 1 &&
        std::abs(component.first->parameters()[Trk::qOverP]) > 0.033333) {
      continue;
    }
    Trk::FitQualityOnSurface componentFitQuality;
    /// Update the component in place
    bool updateSuccess = filterStep(*(component.first),
                                    componentFitQuality,
                                    measurement.localParameters(),
                                    measurement.localCovariance(),
                                    1);
    if (!updateSuccess) {
      continue;
    }

    if (invalidComponent(component.first.get())) {
      continue;
    }

    if (componentFitQuality.chiSquared() <= 0.) {
      continue;
    }

    double componentChi2 = componentFitQuality.chiSquared();
    chiSquared += component.second * componentChi2;

    // The same measurement is included in each update
    // so we can update the degree of freedom only
    if (degreesOfFreedom == 0.0) {
      degreesOfFreedom = componentFitQuality.numberDoF();
    }

    // Add component to state being prepared for assembly
    Trk::MultiComponentStateAssembler::addComponent(cache,
                                                    std::move(component));
  }

  Trk::MultiComponentState assembledUpdatedState =
    Trk::MultiComponentStateAssembler::assembledState(std::move(cache));

  if (assembledUpdatedState.empty()) {
    return {};
  }

  fitQoS.setChiSquared(chiSquared);
  fitQoS.setNumberDoF(degreesOfFreedom);
  // Renormalise state
  Trk::MultiComponentStateHelpers::renormaliseState(assembledUpdatedState);
  return assembledUpdatedState;
}

} // end of anonymous namespace

Trk::MultiComponentState
Trk::GsfMeasurementUpdator::update(Trk::MultiComponentState&& stateBeforeUpdate,
                                   const Trk::MeasurementBase& measurement)
{
  // Check all components have associated error matricies
  Trk::MultiComponentState::iterator component = stateBeforeUpdate.begin();
  bool rebuildStateWithErrors = false;
  // Perform initial check of state awaiting update. If all states have
  // associated error matricies then no need to perform the rebuild
  for (; component != stateBeforeUpdate.end(); ++component) {
    rebuildStateWithErrors =
      rebuildStateWithErrors || invalidComponent(component->first.get());
  }

  if (rebuildStateWithErrors) {
    Trk::MultiComponentState stateWithInsertedErrors =
      rebuildState(std::move(stateBeforeUpdate));
    // Perform the measurement update with the modified state
    return calculateFilterStep(
      std::move(stateWithInsertedErrors), measurement, 1);
  }

  // Perform the measurement update
  return calculateFilterStep(std::move(stateBeforeUpdate), measurement, 1);
}

Trk::MultiComponentState
Trk::GsfMeasurementUpdator::update(Trk::MultiComponentState&& stateBeforeUpdate,
                                   const Trk::MeasurementBase& measurement,
                                   FitQualityOnSurface& fitQoS)
{

  // Check all components have associated error matricies
  Trk::MultiComponentState::iterator component = stateBeforeUpdate.begin();

  bool rebuildStateWithErrors = false;

  // Perform initial check of state awaiting update. If all states have
  // associated error matricies then no need to perform the rebuild
  for (; component != stateBeforeUpdate.end(); ++component) {
    rebuildStateWithErrors =
      rebuildStateWithErrors || invalidComponent(component->first.get());
  }

  if (rebuildStateWithErrors) {
    Trk::MultiComponentState stateWithInsertedErrors =
      rebuildState(std::move(stateBeforeUpdate));
    // Perform the measurement update with the modified state

    Trk::MultiComponentState updatedState = calculateFilterStep(
      std::move(stateWithInsertedErrors), measurement, fitQoS);
    if (updatedState.empty()) {
      return {};
    }
    return updatedState;
  }

  // Perform the measurement update
  Trk::MultiComponentState updatedState =
    calculateFilterStep(std::move(stateBeforeUpdate), measurement, fitQoS);

  if (updatedState.empty()) {
    return {};
  }
  return updatedState;
}

Trk::FitQualityOnSurface
Trk::GsfMeasurementUpdator::fitQuality(const MultiComponentState& updatedState,
                                       const MeasurementBase& measurement)
{
  double chi2 = 0;
  int degreesOfFreedom = 0;
  Trk::MultiComponentState::const_iterator component = updatedState.begin();

  for (; component != updatedState.end(); ++component) {
    const Trk::TrackParameters* trackParameters = component->first.get();
    Trk::FitQualityOnSurface componentFitQuality;
    stateFitQuality(componentFitQuality,
                    *trackParameters,
                    measurement.localParameters(),
                    measurement.localCovariance(),
                    1);

    double componentChi2 = componentFitQuality.chiSquared();
    chi2 += component->second * componentChi2;
    // The same measurement is included in each update
    if (component == updatedState.begin()) {
      degreesOfFreedom = componentFitQuality.numberDoF();
    }
  }
  return { chi2, degreesOfFreedom };
}

