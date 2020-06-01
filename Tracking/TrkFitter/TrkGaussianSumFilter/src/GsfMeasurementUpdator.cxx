/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      GsfMeasurementUpdator.cxx  -  description
      -----------------------------------------
begin                : Friday 25th February 2005
author               : amorley, atkinson
email                : Anthony.Morley@cern.ch, Tom.Atkinson@cern.ch
decription           : Implementation code for GsfMeasurementUpdator class
*********************************************************************************/

#include "TrkGaussianSumFilter/GsfMeasurementUpdator.h"
#include "GaudiKernel/Chrono.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkGaussianSumFilter/MultiComponentStateAssembler.h"
#include "TrkMeasurementBase/MeasurementBase.h"

#include "TrkGaussianSumFilter/PosteriorWeightsCalculator.h"

Trk::GsfMeasurementUpdator::GsfMeasurementUpdator(const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent)
  : AthAlgTool(type, name, parent)
{
  declareInterface<IMultiStateMeasurementUpdator>(this);
}

StatusCode
Trk::GsfMeasurementUpdator::initialize()
{
  // Retrieve the updator tool
  if (m_updator.retrieve().isFailure()) {
    ATH_MSG_FATAL(
      "Could not retrieve measurement updator AlgTool ... Exiting!");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Initialisation of " << name() << " was successful");
  return StatusCode::SUCCESS;
}

StatusCode
Trk::GsfMeasurementUpdator::finalize()
{
  ATH_MSG_INFO("Finalisation of " << name() << " was successful");
  return StatusCode::SUCCESS;
}

Trk::MultiComponentState
Trk::GsfMeasurementUpdator::update(
  Trk::MultiComponentState&& stateBeforeUpdate,
  const Trk::MeasurementBase& measurement) const
{
  MultiComponentState updatedState{};
  // Point to the correct member function of the linear fitter measurement
  // updator for fitting in the direction of momentum
  Updator updator = &Trk::IUpdator::addToState;
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
      std::move(stateWithInsertedErrors), measurement, updator);
    return updatedState;
  }

  // Perform the measurement update
  updatedState =
    calculateFilterStep(std::move(stateBeforeUpdate), measurement, updator);

  return updatedState;
}

Trk::MultiComponentState
Trk::GsfMeasurementUpdator::getUnbiasedTrackParameters(
  Trk::MultiComponentState&& stateBeforeUpdate,
  const MeasurementBase& measurement) const
{
  // Point to the correct member function of the linear fitter measurement
  // updator for fitting in the direction opposite to momentum (smoothing)
  Updator updator = &Trk::IUpdator::removeFromState;
  // Calculate the weight of each state after the measurement
  return calculateFilterStep(
    std::move(stateBeforeUpdate), measurement, updator);
}

const Trk::FitQualityOnSurface*
Trk::GsfMeasurementUpdator::fitQuality(const MultiComponentState& updatedState,
                                       const MeasurementBase& measurement) const
{

  // Fit quality assumes that a state that has been updated by the measurement
  // updator has been supplied to it

  if (updatedState.empty()) {
    ATH_MSG_WARNING("Attempting to calculate chi2 of a hit with respect to an "
                    "empty multiple-component state");
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
    const Trk::FitQualityOnSurface* componentFitQuality =
      m_updator->fullStateFitQuality(*trackParameters,
                                     measurement.localParameters(),
                                     measurement.localCovariance());

    double componentChi2 = componentFitQuality->chiSquared();

    chi2 += component->second * componentChi2;

    // The same measurement is included in each update so it is the same for
    // each component
    if (component == updatedState.begin()) {
      degreesOfFreedom = componentFitQuality->numberDoF();
    }

    delete componentFitQuality;
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
  const Updator updator) const
{
  // state Assembler cache
  MultiComponentStateAssembler::Cache cache;

  if (stateBeforeUpdate.empty()) {
    ATH_MSG_WARNING("Cannot update multi-state with no components!");
    return {};
  }

  // Calculate the weight of each component after the measurement
  PosteriorWeightsCalculator pwc;
  std::vector<Trk::ComponentParameters> stateWithNewWeights =
    pwc.weights(std::move(stateBeforeUpdate), measurement);
  if (stateWithNewWeights.empty()) {
    ATH_MSG_DEBUG("Cacluation of state posterior weights failed... Exiting!");
    return {};
  }

  // Update each component using the specified updator
  Trk::MultiComponentState::const_iterator component =
    stateWithNewWeights.begin();

  for (; component != stateWithNewWeights.end(); ++component) {

    Trk::FitQualityOnSurface* fitQuality = nullptr;

    // Track updates using a pointer to the member function
    std::unique_ptr<Trk::TrackParameters> updatedTrackParameters(
      ((&(*m_updator))->*updator)(*(*component).first,
                                  measurement.localParameters(),
                                  measurement.localCovariance(),
                                  fitQuality));

    if (!updatedTrackParameters) {
      ATH_MSG_DEBUG(
        "Update of state with Measurement has failed 1... Exiting!");
      if (fitQuality) {
        delete fitQuality;
      }
      continue;
    }
    if (fitQuality && fitQuality->chiSquared() <= 0.) {
      ATH_MSG_DEBUG("Fit quality of update failed... Exiting!");
      delete fitQuality;
      updatedTrackParameters.reset();
      continue;
    }
    // Clean up memory
    delete fitQuality;

    // Updator does not change the weighting
    ComponentParameters updatedComponentParameters(
      std::move(updatedTrackParameters), component->second);
    // Add component to state being prepared for assembly and check that it is
    // valid
    bool componentAdded = MultiComponentStateAssembler::addComponent(
      cache, std::move(updatedComponentParameters));

    if (!componentAdded) {
      ATH_MSG_DEBUG(
        "Component could not be added to the state in the assembler");
    }
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
      ATH_MSG_DEBUG("Updated state could not be calculated... Returning 0");
      fitQoS.reset();
      return {};
    }
    return updatedState;
  }

  // Perform the measurement update
  updatedState =
    calculateFilterStep(std::move(stateBeforeUpdate), measurement, fitQoS);

  if (updatedState.empty()) {
    ATH_MSG_DEBUG("Updated state could not be calculated... Returning 0");
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
    ATH_MSG_WARNING("Cannot update multi-state with no components!");
    return {};
  }

  // Calculate the weight of each component after the measurement
  PosteriorWeightsCalculator pwc;
  std::vector<Trk::ComponentParameters> stateWithNewWeights =
    pwc.weights(std::move(stateBeforeUpdate), measurement);

  if (stateWithNewWeights.empty()) {
    ATH_MSG_DEBUG("Cacluation of state posterior weights failed... Exiting!");
    return {};
  }

  // Update each component using the specified updator
  Trk::MultiComponentState::const_iterator component =
    stateWithNewWeights.begin();

  double chiSquared = 0;
  int degreesOfFreedom = 0;

  for (; component != stateWithNewWeights.end(); ++component) {

    if (fabs((*component).first->parameters()[Trk::qOverP]) > 0.033333) {
      ATH_MSG_DEBUG(
        "About to update component with p<30MeV...skipping component! (2)");
      continue;
    }
    Trk::FitQualityOnSurface* componentFitQuality = nullptr;
    // Track update alternates between update and getUnbiasedTrackParams
    std::unique_ptr<Trk::TrackParameters> updatedTrackParameters(
      m_updator->addToState(*(*component).first,
                            measurement.localParameters(),
                            measurement.localCovariance(),
                            componentFitQuality));

    if (!updatedTrackParameters) {
      ATH_MSG_DEBUG(
        "Update of state with Measurement has failed 2... Exiting!");
      if (componentFitQuality) {
        delete componentFitQuality;
      }
      continue;
    }

    if (invalidComponent(updatedTrackParameters.get())) {
      ATH_MSG_DEBUG("Invalid cov matrix after update... Exiting!");
      if ((*component).first->covariance()) {
        ATH_MSG_DEBUG("Original has a COV\n "
                      << *(*component).first->covariance());
      }
      if (updatedTrackParameters->covariance()) {
        ATH_MSG_DEBUG("Result has a COV\n"
                      << *updatedTrackParameters->covariance());
      }
      updatedTrackParameters.reset();
      delete componentFitQuality;
      continue;
    }

    if (!componentFitQuality || componentFitQuality->chiSquared() <= 0.) {
      ATH_MSG_DEBUG("Fit quality of update failed... Exiting!");
      updatedTrackParameters.reset();
      delete componentFitQuality;
      continue;
    }

    double componentChi2 = componentFitQuality->chiSquared();
    chiSquared += component->second * componentChi2;

    // The same measurement is included in each update so it is the same for
    // each component
    if (component == stateWithNewWeights.begin()) {
      degreesOfFreedom = componentFitQuality->numberDoF();
    }
    // Clean up memory
    delete componentFitQuality;

    // Updator does not change the weighting
    ComponentParameters updatedComponentParameters(
      std::move(updatedTrackParameters), component->second);
    // Add component to state being prepared for assembly and check that it is
    // valid
    bool componentAdded = MultiComponentStateAssembler::addComponent(
      cache, std::move(updatedComponentParameters));

    if (!componentAdded) {
      ATH_MSG_DEBUG(
        "Component could not be added to the state in the assembler");
    }
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
