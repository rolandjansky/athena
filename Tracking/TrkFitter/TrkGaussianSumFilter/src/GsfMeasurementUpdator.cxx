/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file   GsfMeasurementUpdator.cxx
 * @date   Friday 25th February 2005
 * @author Tom Athkinson, Anthony Morley, Christos Anastopoulos
 * @brief  Implementation code for GsfMeasurementUpdator class
 */

#include "TrkGaussianSumFilter/GsfMeasurementUpdator.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkGaussianSumFilter/MultiComponentStateAssembler.h"
#include "TrkGaussianSumFilter/PosteriorWeightsCalculator.h"
#include "TrkMeasurementBase/MeasurementBase.h"

Trk::MultiComponentState
Trk::GsfMeasurementUpdator::update(
  Trk::MultiComponentState&& stateBeforeUpdate,
  const Trk::MeasurementBase& measurement) const
{
  MultiComponentState updatedState{};
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
    updatedState =
      calculateFilterStep(std::move(stateWithInsertedErrors), measurement, 1);
    return updatedState;
  }

  // Perform the measurement update
  updatedState =
    calculateFilterStep(std::move(stateBeforeUpdate), measurement, 1);

  return updatedState;
}

const Trk::FitQualityOnSurface*
Trk::GsfMeasurementUpdator::fitQuality(const MultiComponentState& updatedState,
                                       const MeasurementBase& measurement) const
{

  // Fit quality assumes that a state that has been updated by the measurement
  // updator has been supplied to it
  if (updatedState.empty()) {
    return nullptr;
  }
  double chi2 = 0;
  int degreesOfFreedom = 0;
  Trk::MultiComponentState::const_iterator component = updatedState.begin();

  for (; component != updatedState.end(); ++component) {
    const Trk::TrackParameters* trackParameters = component->first.get();

    // IUpdator interface change (27/09/2005) to allow for fit quality
    // calculations depending on if the track parameters incorporate the
    // information contained in the measurement. I ALWAYS do this - hence the
    // fullStateFitQuality method is used
    Trk::FitQualityOnSurface componentFitQuality;
    m_updator.stateFitQuality(componentFitQuality,
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

  if (std::isnan(chi2) || chi2 <= 0.) {
    return nullptr;
  }
  const Trk::FitQualityOnSurface* fitQualityOnSurface =
    new FitQualityOnSurface(chi2, degreesOfFreedom);

  return fitQualityOnSurface;
}

Trk::MultiComponentState
Trk::GsfMeasurementUpdator::calculateFilterStep(
  Trk::MultiComponentState&& stateBeforeUpdate,
  const Trk::MeasurementBase& measurement,
  int addRemoveFlag) const
{
  if (stateBeforeUpdate.empty()) {
    return {};
  }

  // state Assembler cache
  MultiComponentStateAssembler::Cache cache;

  // Calculate the weight of each component after the measurement
  std::vector<Trk::ComponentParameters> stateWithNewWeights =
    PosteriorWeightsCalculator::weights(std::move(stateBeforeUpdate),
                                        measurement);

  if (stateWithNewWeights.empty()) {
    return {};
  }

  for (Trk::ComponentParameters& component : stateWithNewWeights) {

    auto fitQuality = std::make_unique<Trk::FitQualityOnSurface>();
    /// Update the component in place
    bool updateSuccess = m_updator.filterStep(*(component.first),
                                              *fitQuality,
                                              measurement.localParameters(),
                                              measurement.localCovariance(),
                                              addRemoveFlag);

    // If we fail we need to erase the element
    if (!updateSuccess || fitQuality->chiSquared() <= 0.) {
      fitQuality.reset();
      continue;
    }
    // Move component to state being prepared for assembly
    MultiComponentStateAssembler::addComponent(cache, std::move(component));
  }

  Trk::MultiComponentState assembledUpdatedState =
    MultiComponentStateAssembler::assembledState(cache);

  if (assembledUpdatedState.empty()) {
    return {};
  }
  // Renormalise state
  MultiComponentStateHelpers::renormaliseState(assembledUpdatedState);
  return assembledUpdatedState;
}

Trk::MultiComponentState
Trk::GsfMeasurementUpdator::update(
  Trk::MultiComponentState&& stateBeforeUpdate,
  const Trk::MeasurementBase& measurement,
  std::unique_ptr<FitQualityOnSurface>& fitQoS) const
{

  Trk::MultiComponentState updatedState{};

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
    updatedState = calculateFilterStep(
      std::move(stateWithInsertedErrors), measurement, fitQoS);
    if (updatedState.empty()) {
      fitQoS.reset();
      return {};
    }
    return updatedState;
  }

  // Perform the measurement update
  updatedState =
    calculateFilterStep(std::move(stateBeforeUpdate), measurement, fitQoS);

  if (updatedState.empty()) {
    fitQoS.reset();
    return {};
  }
  return updatedState;
}

Trk::MultiComponentState
Trk::GsfMeasurementUpdator::calculateFilterStep(
  Trk::MultiComponentState&& stateBeforeUpdate,
  const Trk::MeasurementBase& measurement,
  std::unique_ptr<FitQualityOnSurface>& fitQoS) const
{
  // state Assembler cache
  MultiComponentStateAssembler::Cache cache;
  if (stateBeforeUpdate.empty()) {
    return {};
  }

  // Calculate the weight of each component after the measurement
  std::vector<Trk::ComponentParameters> stateWithNewWeights =
    PosteriorWeightsCalculator::weights(std::move(stateBeforeUpdate),
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
    bool updateSuccess = m_updator.filterStep(*(component.first),
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
    MultiComponentStateAssembler::addComponent(cache, std::move(component));
  }

  Trk::MultiComponentState assembledUpdatedState =
    MultiComponentStateAssembler::assembledState(cache);

  if (assembledUpdatedState.empty()) {
    return {};
  }

  fitQoS = std::make_unique<FitQualityOnSurface>(chiSquared, degreesOfFreedom);
  // Renormalise state
  MultiComponentStateHelpers::renormaliseState(assembledUpdatedState);
  return assembledUpdatedState;
}

bool
Trk::GsfMeasurementUpdator::invalidComponent(
  const Trk::TrackParameters* trackParameters) const
{
  auto measuredCov = trackParameters->covariance();
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
Trk::GsfMeasurementUpdator::rebuildState(
  Trk::MultiComponentState&& stateBeforeUpdate) const
{
  Trk::MultiComponentState stateWithInsertedErrors{};

  auto component = stateBeforeUpdate.begin();
  for (; component != stateBeforeUpdate.end(); ++component) {

    const Trk::TrackParameters* trackParameters = component->first.get();
    double weight = component->second;
    bool rebuildCov = invalidComponent(trackParameters);
    if (rebuildCov) {
      AmgSymMatrix(5)* bigNewCovarianceMatrix = new AmgSymMatrix(5);
      bigNewCovarianceMatrix->setZero();
      double covarianceScaler = 1.;
      (*bigNewCovarianceMatrix)(0, 0) = 250. * covarianceScaler;
      (*bigNewCovarianceMatrix)(1, 1) = 250. * covarianceScaler;
      (*bigNewCovarianceMatrix)(2, 2) = 0.25;
      (*bigNewCovarianceMatrix)(3, 3) = 0.25;
      (*bigNewCovarianceMatrix)(4, 4) = 0.001 * 0.001;

      AmgVector(5) par = trackParameters->parameters();
      Trk::TrackParameters* trackParametersWithError =
        trackParameters->associatedSurface().createTrackParameters(
          par[Trk::loc1],
          par[Trk::loc2],
          par[Trk::phi],
          par[Trk::theta],
          par[Trk::qOverP],
          bigNewCovarianceMatrix);
      stateWithInsertedErrors.emplace_back(trackParametersWithError, weight);
    } else {
      stateWithInsertedErrors.push_back(std::move(*component));
    }
  }
  return stateWithInsertedErrors;
}
