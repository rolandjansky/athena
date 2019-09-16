/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkMeasurementBase/MeasurementBase.h"

#include "CxxUtils/make_unique.h"
#include "TrkEventPrimitives/LocalParameters.h"

#include "GaudiKernel/Chrono.h"
#include "GaudiKernel/IChronoStatSvc.h"

Trk::GsfMeasurementUpdator::GsfMeasurementUpdator(const std::string type,
                                                  const std::string name,
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
    ATH_MSG_FATAL("Could not retrieve measurement updator AlgTool ... Exiting!");
    return StatusCode::FAILURE;
  }

  // Retrieve the Posterior Weights Calculator
  if (m_posteriorWeightsCalculator.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not find the Posterior Weights Calculator Service... Exiting!");
    return StatusCode::FAILURE;
  }

  // Request an instance of the MultiComponentStateAssembler
  if (m_stateAssembler.retrieve().isFailure()) {
    ATH_MSG_ERROR("Could not access the MultiComponentStateAssembler Service");
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

std::unique_ptr<Trk::MultiComponentState>
Trk::GsfMeasurementUpdator::update(const Trk::MultiComponentState& stateBeforeUpdate,
                                   const Trk::MeasurementBase& measurement) const
{

  ATH_MSG_VERBOSE("Updating using GsfMeasurementUpdator");

  std::unique_ptr<MultiComponentState> updatedState = nullptr;
  // Point to the correct member function of the linear fitter measurement updator for fitting in the direction of
  // momentum
  Updator updator = &Trk::IUpdator::addToState;
  // Check all components have associated error matricies
  Trk::MultiComponentState::const_iterator component = stateBeforeUpdate.begin();
  bool rebuildStateWithErrors = false;
  // Perform initial check of state awaiting update. If all states have associated error matricies then no need to
  // perform the rebuild
  for (; component != stateBeforeUpdate.end(); ++component) {
    rebuildStateWithErrors = rebuildStateWithErrors || invalidComponent(component->first);
  }

  if (rebuildStateWithErrors) {
    ATH_MSG_VERBOSE("Rebuilding state with errors");
    std::unique_ptr<Trk::MultiComponentState> stateWithInsertedErrors = rebuildState(stateBeforeUpdate);
    // Perform the measurement update with the modified state
    updatedState = calculateFilterStep(*stateWithInsertedErrors, measurement, updator);
    stateWithInsertedErrors.reset();
    if (!updatedState) {
      ATH_MSG_DEBUG("Updated state could not be calculated... Returning 0");
      return nullptr;
    }
    return updatedState;
  }

  // Perform the measurement update
  updatedState = calculateFilterStep(stateBeforeUpdate, measurement, updator);

  if (!updatedState) {
    ATH_MSG_DEBUG("Updated state could not be calculated... Returning 0");
    return nullptr;
  }

  return updatedState;
}

std::unique_ptr<Trk::MultiComponentState>
Trk::GsfMeasurementUpdator::getUnbiasedTrackParameters(const Trk::MultiComponentState& stateBeforeUpdate,
                                                       const MeasurementBase& measurement) const
{
  // Point to the correct member function of the linear fitter measurement
  // updator for fitting in the direction opposite to momentum (smoothing)
  Updator updator = &Trk::IUpdator::removeFromState;
  // Calculate the weight of each state after the measurement
  return calculateFilterStep(stateBeforeUpdate, measurement, updator);
}

const Trk::FitQualityOnSurface*
Trk::GsfMeasurementUpdator::fitQuality(const MultiComponentState& updatedState,
                                       const MeasurementBase& measurement) const
{

  // Fit quality assumes that a state that has been updated by the measurement updator has been supplied to it

  if (updatedState.empty()) {
    ATH_MSG_WARNING("Attempting to calculate chi2 of a hit with respect to an empty multiple-component state");
    return 0;
  }

  double chi2 = 0;
  int degreesOfFreedom = 0;
  Trk::MultiComponentState::const_iterator component = updatedState.begin();

  for (; component != updatedState.end(); ++component) {
    const Trk::TrackParameters* trackParameters = component->first;

    // IUpdator interface change (27/09/2005) to allow for fit quality calculations depending on if the track parameters
    // incorporate the information contained in the measurement. I ALWAYS do this - hence the fullStateFitQuality method
    // is used
    const Trk::FitQualityOnSurface* componentFitQuality =
      m_updator->fullStateFitQuality(*trackParameters, measurement.localParameters(), measurement.localCovariance());

    double componentChi2 = componentFitQuality->chiSquared();

    chi2 += component->second * componentChi2;

    // The same measurement is included in each update so it is the same for each component
    if (component == updatedState.begin())
      degreesOfFreedom = componentFitQuality->numberDoF();

    delete componentFitQuality;
  }

  if (std::isnan(chi2) || chi2 <= 0.) {

    return 0;
  }

  const Trk::FitQualityOnSurface* fitQualityOnSurface = new FitQualityOnSurface(chi2, degreesOfFreedom);

  return fitQualityOnSurface;
}

std::unique_ptr<Trk::MultiComponentState>
Trk::GsfMeasurementUpdator::calculateFilterStep(const Trk::MultiComponentState& stateBeforeUpdate,
                                                const Trk::MeasurementBase& measurement,
                                                const Updator updator) const
{

  ATH_MSG_VERBOSE("Calculate Filter Step");

  // state Assembler cache
  IMultiComponentStateAssembler::Cache cache;
  // Check that the assember is reset
  bool isAssemblerReset = m_stateAssembler->reset(cache);

  if (!isAssemblerReset) {
    ATH_MSG_DEBUG("Could not reset the state assembler... returning 0");
    return 0;
  }

  if (stateBeforeUpdate.empty()) {
    ATH_MSG_WARNING("Cannot update multi-state with no components!");
    return 0;
  }

  // Calculate the weight of each component after the measurement
  std::unique_ptr<Trk::MultiComponentState> stateWithNewWeights = m_posteriorWeightsCalculator->weights(stateBeforeUpdate, 
                                                                                                        measurement);

  if (!stateWithNewWeights) {
    ATH_MSG_DEBUG("Cacluation of state posterior weights failed... Exiting!");
    return nullptr;
  }

  // Update each component using the specified updator
  Trk::MultiComponentState::const_iterator component = stateWithNewWeights->begin();

  for (; component != stateWithNewWeights->end(); ++component) {

    ATH_MSG_VERBOSE("Performing update of predicted component state with measurement...");
    
    Trk::FitQualityOnSurface* fitQuality = 0;

    // Track updates using a pointer to the member function
    std::unique_ptr<Trk::TrackParameters> updatedTrackParameters( ((&(*m_updator))->*updator)(
        *(*component).first, measurement.localParameters(), measurement.localCovariance(), fitQuality));

    if (!updatedTrackParameters) {
      ATH_MSG_DEBUG("Update of state with Measurement has failed 1... Exiting!");
      if (fitQuality)
        delete fitQuality;
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

    ATH_MSG_VERBOSE("Successful measurement update with Measurement");

    // Updator does not change the weighting
    SimpleComponentParameters updatedComponentParameters(std::move(updatedTrackParameters), component->second);

    // Add component to state being prepared for assembly and check that it is valid
    bool componentAdded = m_stateAssembler->addComponent(cache, std::move(updatedComponentParameters));

    if (!componentAdded){
      ATH_MSG_DEBUG("Component could not be added to the state in the assembler");
    }  
  }

  std::unique_ptr<Trk::MultiComponentState> assembledUpdatedState = m_stateAssembler->assembledState(cache);

  if (!assembledUpdatedState) {
    return nullptr;
  }
  // Renormalise state
  assembledUpdatedState->renormaliseState();

  return assembledUpdatedState;
}

std::unique_ptr<Trk::MultiComponentState>
Trk::GsfMeasurementUpdator::update(const Trk::MultiComponentState& stateBeforeUpdate,
                                   const Trk::MeasurementBase& measurement,
                                   std::unique_ptr<FitQualityOnSurface>& fitQoS) const
{

  std::unique_ptr<Trk::MultiComponentState> updatedState = nullptr;

  // Check all components have associated error matricies
  Trk::MultiComponentState::const_iterator component = stateBeforeUpdate.begin();

  bool rebuildStateWithErrors = false;

  // Perform initial check of state awaiting update. If all states have associated error matricies then no need to
  // perform the rebuild
  for (; component != stateBeforeUpdate.end(); ++component) {
    rebuildStateWithErrors = rebuildStateWithErrors || invalidComponent(component->first);
  }

  if (rebuildStateWithErrors) {
    std::unique_ptr<Trk::MultiComponentState> stateWithInsertedErrors = rebuildState(stateBeforeUpdate);
    // Perform the measurement update with the modified state
    updatedState = calculateFilterStep(*stateWithInsertedErrors, measurement, fitQoS);
    if (!updatedState) {
      ATH_MSG_DEBUG("Updated state could not be calculated... Returning 0");
      fitQoS.reset();
      return nullptr;
    }
    return updatedState;
  }

  // Perform the measurement update
  updatedState = calculateFilterStep(stateBeforeUpdate, measurement, fitQoS);

  if (!updatedState) {
    ATH_MSG_DEBUG("Updated state could not be calculated... Returning 0");
    fitQoS.reset();
    return 0;
  }
  return updatedState;
}

std::unique_ptr<Trk::MultiComponentState>
Trk::GsfMeasurementUpdator::calculateFilterStep(const Trk::MultiComponentState& stateBeforeUpdate,
                                                const Trk::MeasurementBase& measurement,
                                                std::unique_ptr<FitQualityOnSurface>& fitQoS) const
{
  // state Assembler cache
  IMultiComponentStateAssembler::Cache cache;
  // Check that the assember is reset
  bool isAssemblerReset = m_stateAssembler->reset(cache);

  if (!isAssemblerReset) {
    ATH_MSG_ERROR("Could not reset the state assembler... returning 0");
    return 0;
  }

  if (stateBeforeUpdate.empty()) {
    ATH_MSG_WARNING("Cannot update multi-state with no components!");
    return 0;
  }

  // Calculate the weight of each component after the measurement
  std::unique_ptr<Trk::MultiComponentState> stateWithNewWeights = m_posteriorWeightsCalculator->weights(stateBeforeUpdate, 
                                                                                                       measurement);

  if (!stateWithNewWeights) {
    ATH_MSG_DEBUG("Cacluation of state posterior weights failed... Exiting!");
    return nullptr;
  }

  // Update each component using the specified updator
  Trk::MultiComponentState::const_iterator component = stateWithNewWeights->begin();

  double chiSquared = 0;
  int degreesOfFreedom = 0;

  for (; component != stateWithNewWeights->end(); ++component) {

    if (fabs((*component).first->parameters()[Trk::qOverP]) > 0.033333) {
      ATH_MSG_DEBUG("About to update component with p<30MeV...skipping component! (2)");
      continue;
    }
    Trk::FitQualityOnSurface* componentFitQuality = 0;
    // Track update alternates between update and getUnbiasedTrackParams
     std::unique_ptr<Trk::TrackParameters> updatedTrackParameters (m_updator->addToState(
      *(*component).first, measurement.localParameters(), measurement.localCovariance(), componentFitQuality));

    if (!updatedTrackParameters) {
      ATH_MSG_DEBUG("Update of state with Measurement has failed 2... Exiting!");
      if (componentFitQuality){
        delete componentFitQuality;
      }
      continue;
    }

    if (invalidComponent(updatedTrackParameters.get())) {
      ATH_MSG_DEBUG("Invalid cov matrix after update... Exiting!");
      if ((*component).first->covariance()){
        ATH_MSG_DEBUG("Original has a COV\n " << *(*component).first->covariance());
      }
      if (updatedTrackParameters->covariance()){
        ATH_MSG_DEBUG("Result has a COV\n" << *updatedTrackParameters->covariance());
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

    // The same measurement is included in each update so it is the same for each component
    if (component == stateWithNewWeights->begin()){
      degreesOfFreedom = componentFitQuality->numberDoF();
    }
    // Clean up memory
    delete componentFitQuality;

    // Updator does not change the weighting
    SimpleComponentParameters updatedComponentParameters(std::move(updatedTrackParameters), component->second);
    // Add component to state being prepared for assembly and check that it is valid
    bool componentAdded = m_stateAssembler->addComponent(cache, std::move(updatedComponentParameters));

    if (!componentAdded){
      ATH_MSG_DEBUG("Component could not be added to the state in the assembler");
    }
  }

  std::unique_ptr<Trk::MultiComponentState> assembledUpdatedState = m_stateAssembler->assembledState(cache);

  if (!assembledUpdatedState){
    return nullptr;
  }

  fitQoS = std::make_unique<FitQualityOnSurface>(chiSquared, degreesOfFreedom);
  // Renormalise state
  assembledUpdatedState->renormaliseState();
  ATH_MSG_VERBOSE("Successful calculation of filter step: " << assembledUpdatedState->size());
  return assembledUpdatedState;
}

bool
Trk::GsfMeasurementUpdator::invalidComponent(const Trk::TrackParameters* trackParameters) const
{
  auto measuredCov = trackParameters->covariance();
  bool rebuildCov = false;
  if (!measuredCov) {
    rebuildCov = true;
  } else {
    for (int i(0); i < 5; ++i) {
      if ((*measuredCov)(i, i) <= 0.)
        rebuildCov = true;
    }
  }
  return rebuildCov;
}

std::unique_ptr<Trk::MultiComponentState>
Trk::GsfMeasurementUpdator::rebuildState(const Trk::MultiComponentState& stateBeforeUpdate) const
{
  std::unique_ptr<Trk::MultiComponentState> stateWithInsertedErrors = std::make_unique<Trk::MultiComponentState>();
  const Trk::TrackParameters* trackParametersWithError = 0;

  auto component = stateBeforeUpdate.begin();

  for (; component != stateBeforeUpdate.end(); ++component) {

    auto trackParameters = component->first;
    auto weight = component->second;

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
      trackParametersWithError = trackParameters->associatedSurface().createTrackParameters(
        par[Trk::loc1], par[Trk::loc2], par[Trk::phi], par[Trk::theta], par[Trk::qOverP], bigNewCovarianceMatrix);

      Trk::ComponentParameters componentParametersWithError(trackParametersWithError, weight);
      stateWithInsertedErrors->push_back(componentParametersWithError);
    } else {
      stateWithInsertedErrors->push_back(*component);
    }
  }
  return stateWithInsertedErrors;
}
