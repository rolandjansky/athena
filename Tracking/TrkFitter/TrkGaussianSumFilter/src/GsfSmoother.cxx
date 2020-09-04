/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   GsfSmoother.cxx
 * @date   Wednesday 9th March 2005
 * @author Tom Athkinson, Anthony Morley, Christos Anastopoulos
 * @brief  Implementation code for the smoothing part of the GSF fitter
 */

#include "TrkCaloCluster_OnTrack/CaloCluster_OnTrack.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkGaussianSumFilter/GaussianSumFitter.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkSurfaces/Surface.h"

#include "TrkGaussianSumFilter/IMultiStateExtrapolator.h"
#include "TrkGaussianSumFilter/MultiComponentStateCombiner.h"
#include "TrkGaussianSumFilter/QuickCloseComponentsMultiStateMerger.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentStateOnSurface.h"

Trk::SmoothedTrajectory*
Trk::GaussianSumFitter::fit(
  const EventContext& ctx,
  Trk::IMultiStateExtrapolator::Cache& extrapolatorCache,
  const ForwardTrajectory& forwardTrajectory,
  const ParticleHypothesis particleHypothesis,
  const Trk::CaloCluster_OnTrack* ccot) const
{
  // Check that the forward trajectory is filled
  if (forwardTrajectory.empty()) {
    ATH_MSG_ERROR(
      "Attempting to smooth an empty forward trajectory... Exiting!");
    return nullptr;
  }
  /* Instantiate the returned trajectoy. This is a vector of TrackStateOnSurface
     object In the GSF Smoother these TrackStateOnSurface objects are the base
     class for the MultiComponentStateOnSurface. This memory should be freed by
     the fitter / smoother master method */

  /*
   * In general for things we want to control their lifetime locally
   * we use unique_ptr
   *
   * For things where we get or pass ownership to a multistate and then continue
   * to use them we opt for const ptr* for now.
   */
  auto smoothedTrajectory = std::make_unique<Trk::SmoothedTrajectory>();
  auto smootherPredictionMultiState =
    std::make_unique<Trk::MultiComponentState>();
  /*
   * Get the initial smoother prediction. It is the last prediction in the
   * forwards trajectory
   */
  /* these can be  owned by the input so const ptr*/
  const Trk::TrackStateOnSurface* smootherPredictionStateOnSurface =
    forwardTrajectory.back();
  /*
   * We can have single or Multi components here, so we choose what to clone
   */
  const Trk::MultiComponentStateOnSurface*
    smootherPredictionMultiStateOnSurface = nullptr;

  // Check if we already have a MultiComponent state
  if (smootherPredictionStateOnSurface->variety() ==
      Trk::TrackStateOnSurface::MultiComponent) {
    smootherPredictionMultiStateOnSurface =
      static_cast<const Trk::MultiComponentStateOnSurface*>(
        smootherPredictionStateOnSurface);
  }

  if (!smootherPredictionMultiStateOnSurface) {
    auto dummyMultiState = std::make_unique<Trk::MultiComponentState>();
    Trk::ComponentParameters dummyParams(
      smootherPredictionStateOnSurface->trackParameters()->clone(), 1.0);
    dummyMultiState->push_back(std::move(dummyParams));
    smootherPredictionMultiState = std::move(dummyMultiState);
  } else {
    smootherPredictionMultiState = MultiComponentStateHelpers::clone(
      *(smootherPredictionMultiStateOnSurface->components()));
  }

  /*
   * Perform the measurement update
   * The first smoother prediction state is equivalent to the last forwards
   * prediction updated with the last measurement Retrieve the first
   * MeasurementBase object for the smoother
   */
  Trk::FitQualityOnSurface fitQuality;
  /*
   * The following 2 pointers if not null will be owned by the
   * MultiComponentStateOnSurface* updatedStateOnSurface
   * at the end of the code and we need to re-use them.
   * So const ptr
   */
  const Trk::MeasurementBase* firstSmootherMeasurementOnTrack =
    smootherPredictionStateOnSurface->measurementOnTrack()->clone();
  if (!firstSmootherMeasurementOnTrack) {
    ATH_MSG_WARNING(
      "Initial state on surface in smoother does not have an associated "
      "MeasurementBase object... returning 0");
    return nullptr;
  }
  Trk::MultiComponentState firstSmoothedState =
    m_updator.update(std::move(*smootherPredictionMultiState),
                     *firstSmootherMeasurementOnTrack,
                     fitQuality);

  if (firstSmoothedState.empty()) {
    ATH_MSG_DEBUG("First GSF smoothing update failed... Exiting!");
    delete firstSmootherMeasurementOnTrack;
    return nullptr;
  }

  std::unique_ptr<Trk::TrackParameters> combinedFirstSmoothedState =
    MultiComponentStateCombiner::combine(firstSmoothedState, true);

  /*Create updatedStateOnSurface which owned be the tragectory*/
  const Trk::MultiComponentStateOnSurface* updatedStateOnSurface =
    new MultiComponentStateOnSurface(
      firstSmootherMeasurementOnTrack,
      combinedFirstSmoothedState.release(),
      MultiComponentStateHelpers::clone(firstSmoothedState).release(),
      (std::make_unique<Trk::FitQualityOnSurface>(fitQuality)).release());
  smoothedTrajectory->push_back(updatedStateOnSurface);

  /*
   * Reset pointers
   * first the const ptr
   */
  smootherPredictionStateOnSurface = nullptr;
  updatedStateOnSurface = nullptr;
  smootherPredictionMultiStateOnSurface = nullptr;
  /*
   * Then the unique ptr
   */
  smootherPredictionMultiState.reset();

  if (!MultiComponentStateHelpers::isMeasured(firstSmoothedState)) {
    ATH_MSG_WARNING(
      "Updated state is not measured. Rejecting smoothed state... returning 0");
    return nullptr;
  }
  /*
   * Generate prediction by scaling the covariance of all components in the
   * initial state
   *
   * Generate a large prediction for extrapolation. This way there is no
   * dependance on error of prediction NB local Y and theta are not blown out
   * too much to help in the TRT
   */
  std::unique_ptr<Trk::MultiComponentState> smoothedStateWithScaledError =
    MultiComponentStateHelpers::cloneWithScaledError(
      firstSmoothedState, 15., 5., 15., 5., 15.);

  // Perform a measurement update on this new state
  Trk::MultiComponentState updatedState = m_updator.update(
    std::move(*smoothedStateWithScaledError), *firstSmootherMeasurementOnTrack);

  if (updatedState.empty()) {
    ATH_MSG_WARNING(
      "Smoother prediction could not be determined... returning 0");
    return nullptr;
  }

  // Clear rioOnTrack pointer
  firstSmootherMeasurementOnTrack = nullptr;

  // Loop over all remaining TrackStateOnSurface objects in the forward
  // trajectory
  Trk::ForwardTrajectory::const_reverse_iterator trackStateOnSurface =
    forwardTrajectory.rbegin() + 1;
  Trk::ForwardTrajectory::const_reverse_iterator lasttrackStateOnSurface =
    forwardTrajectory.rend() - 1;
  // TSOS that the cluster measuremenet will added on after .. can’t be teh last
  // as the impact parameters will be compromised
  Trk::ForwardTrajectory::const_reverse_iterator secondLastTrackStateOnSurface =
    forwardTrajectory.rend() - 2;

  for (; trackStateOnSurface != forwardTrajectory.rend();
       ++trackStateOnSurface) {
    // Retrieve the MeasurementBase object from the TrackStateOnSurface object
    const Trk::MeasurementBase* measurement_in =
      (*trackStateOnSurface)->measurementOnTrack();
    if (!measurement_in) {
      ATH_MSG_WARNING("MeasurementBase object could not be extracted from a "
                      "measurement... continuing");
      continue;
    }
    // Clone the MeasurementBase object
    std::unique_ptr<const Trk::MeasurementBase> measurement =
      std::unique_ptr<const Trk::MeasurementBase>(measurement_in->clone());
    /* Extrapolate the current multi-component state to the
     next measurement surface. For the smoother the direction of propagation
     is opposite to the direction of momentum */

    Trk::MultiComponentState extrapolatedState =
      m_extrapolator->extrapolate(ctx,
                                  extrapolatorCache,
                                  updatedState,
                                  measurement->associatedSurface(),
                                  Trk::oppositeMomentum,
                                  false,
                                  particleHypothesis);

    if (extrapolatedState.empty()) {
      ATH_MSG_DEBUG(
        "Extrapolation to measurement surface failed... rejecting track!");
      return nullptr;
    }

    // Original measurement was flagged as  an outlier
    if (!(*trackStateOnSurface)->type(TrackStateOnSurface::Measurement)) {
      updatedState = std::move(extrapolatedState);

      std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type(
        0);
      type.set(TrackStateOnSurface::Outlier);

      Trk::MultiComponentStateOnSurface* updatedStateOnSurface =
        new Trk::MultiComponentStateOnSurface(
          measurement.release(),
          MultiComponentStateHelpers::clone(updatedState).release(),
          new FitQuality(1, 1),
          nullptr,
          type);
      smoothedTrajectory->push_back(updatedStateOnSurface);
      continue;
    }

    // Update newly extrapolated state with MeasurementBase measurement
    updatedState =
      m_updator.update(std::move(extrapolatedState), *measurement, fitQuality);
    if (updatedState.empty()) {
      ATH_MSG_WARNING(
        "Could not update the multi-component state... rejecting track!");
      return nullptr;
    }

    /* =============================================================
       Optional combine smoother state with fitter state
       ============================================================= */

    if (m_combineWithFitter) {

      const Trk::MultiComponentState* forwardsMultiState = nullptr;
      std::unique_ptr<Trk::MultiComponentState> forwardsMultiStateOwn;

      const Trk::MultiComponentStateOnSurface* forwardsMultiStateOnSurface =
        nullptr;
      // Check if we already have a MultiComponent state on surface
      if ((*trackStateOnSurface)->variety() ==
          Trk::TrackStateOnSurface::MultiComponent) {
        forwardsMultiStateOnSurface =
          static_cast<const Trk::MultiComponentStateOnSurface*>(
            *trackStateOnSurface);
      }

      if (!forwardsMultiStateOnSurface) {
        // Create new multiComponentState from single state
        Trk::ComponentParameters componentParameters(
          (*trackStateOnSurface)->trackParameters()->clone(), 1.);
        forwardsMultiStateOwn = std::make_unique<Trk::MultiComponentState>();
        forwardsMultiStateOwn->push_back(std::move(componentParameters));
        forwardsMultiState = forwardsMultiStateOwn.get();
      } else {
        forwardsMultiState = forwardsMultiStateOnSurface->components();
      }
      Trk::MultiComponentState combinedState2 =
        combine(*forwardsMultiState, updatedState);
      if (combinedState2.empty()) {
        ATH_MSG_WARNING("Could not combine state from forward fit with "
                        "smoother state... rejecting track!");
        return nullptr;
      }
      auto combinedFitQuality = std::make_unique<Trk::FitQualityOnSurface>(
        m_updator.fitQuality(combinedState2, *measurement));
      // In the case of combination with forwards state - push back the combined
      // state
      const Trk::MultiComponentStateOnSurface* combinedStateOnSurface =
        new MultiComponentStateOnSurface(
          measurement.release(),
          MultiComponentStateHelpers::clone(combinedState2).release(),
          combinedFitQuality.release());
      smoothedTrajectory->push_back(combinedStateOnSurface);
    } else {
      // If combination with forwards state is not done - push back updated
      // state
      const Trk::MultiComponentStateOnSurface* updatedStateOnSurface = nullptr;

      if (trackStateOnSurface == lasttrackStateOnSurface) {
        std::unique_ptr<Trk::TrackParameters> combinedLastState =
          MultiComponentStateCombiner::combine(updatedState, true);

        if (combinedLastState) {
          updatedStateOnSurface = new Trk::MultiComponentStateOnSurface(
            measurement.release(),
            combinedLastState.release(),
            MultiComponentStateHelpers::clone(updatedState).release(),
            (std::make_unique<Trk::FitQualityOnSurface>(fitQuality)).release());
        } else {
          updatedStateOnSurface = new Trk::MultiComponentStateOnSurface(
            measurement.release(),
            MultiComponentStateHelpers::clone(updatedState).release(),
            (std::make_unique<Trk::FitQualityOnSurface>(fitQuality)).release());
        }
      } else {
        updatedStateOnSurface = new Trk::MultiComponentStateOnSurface(
          measurement.release(),
          MultiComponentStateHelpers::clone(updatedState).release(),
          (std::make_unique<Trk::FitQualityOnSurface>(fitQuality)).release());
      }

      smoothedTrajectory->push_back(updatedStateOnSurface);

      /* =============================================================
         Add measurement from calo if  it is present
         ============================================================= */

      if (ccot && trackStateOnSurface == secondLastTrackStateOnSurface) {

        Trk::MultiComponentState ccotState =
          addCCOT(ctx, updatedStateOnSurface, ccot, smoothedTrajectory.get());
        if (!ccotState.empty()) {
          updatedState = std::move(ccotState);
        }
      }
    }
  } // End for loop over all components
  return smoothedTrajectory.release();
}

Trk::MultiComponentState
Trk::GaussianSumFitter::combine(
  const Trk::MultiComponentState& forwardsMultiState,
  const Trk::MultiComponentState& smootherMultiState) const
{

  std::unique_ptr<Trk::MultiComponentState> combinedMultiState =
    std::make_unique<MultiComponentState>();
  /* ================================================
     Loop over all components in forwards multi-state
     ================================================ */

  for (auto& forwardsComponent : forwardsMultiState) {
    // Need to check that all components have associated weight matricies
    const AmgSymMatrix(5)* forwardMeasuredCov =
      forwardsComponent.first->covariance();

    /* ====================================================
       Loop over all components in the smoother multi-state
       ==================================================== */

    for (auto& smootherComponent : smootherMultiState) {
      // Need to check that all components have associated weight matricies
      const AmgSymMatrix(5)* smootherMeasuredCov =
        smootherComponent.first->covariance();
      if (!smootherMeasuredCov && !forwardMeasuredCov) {
        ATH_MSG_WARNING("Cannot combine two components both without associated "
                        "errors... returning 0");
        return {};
      }

      if (!forwardMeasuredCov) {
        Trk::ComponentParameters smootherComponentOnly(
          smootherComponent.first->clone(), smootherComponent.second);
        combinedMultiState->push_back(std::move(smootherComponentOnly));
        continue;
      }

      if (!smootherMeasuredCov) {
        Trk::ComponentParameters forwardComponentOnly(
          forwardsComponent.first->clone(), forwardsComponent.second);
        combinedMultiState->push_back(std::move(forwardComponentOnly));
        continue;
      }

      const AmgSymMatrix(5) summedCovariance =
        *forwardMeasuredCov + *smootherMeasuredCov;
      const AmgSymMatrix(5) K =
        *forwardMeasuredCov * summedCovariance.inverse();
      const Amg::VectorX newParameters =
        forwardsComponent.first->parameters() +
        K * (smootherComponent.first->parameters() -
             forwardsComponent.first->parameters());
      const Amg::VectorX parametersDiff =
        forwardsComponent.first->parameters() -
        smootherComponent.first->parameters();

      AmgSymMatrix(5)* covarianceOfNewParameters =
        new AmgSymMatrix(5)(K * *smootherMeasuredCov);

      Trk::TrackParameters* combinedTrackParameters =
        (forwardsComponent.first)
          ->associatedSurface()
          .createTrackParameters(newParameters[Trk::loc1],
                                 newParameters[Trk::loc2],
                                 newParameters[Trk::phi],
                                 newParameters[Trk::theta],
                                 newParameters[Trk::qOverP],
                                 covarianceOfNewParameters);
      // Covariance matrix object now owned by TrackParameters object. Reset
      // pointer to prevent delete
      covarianceOfNewParameters = nullptr;
      const AmgSymMatrix(5) invertedSummedCovariance =
        summedCovariance.inverse();
      // Determine the scaling factor for the new weighting. Determined from the
      // PDF of the many-dimensional gaussian
      double exponent =
        parametersDiff.transpose() * invertedSummedCovariance * parametersDiff;
      double weightScalingFactor = exp(-0.5 * exponent);
      double combinedWeight = smootherComponent.second *
                              forwardsComponent.second * weightScalingFactor;
      Trk::ComponentParameters combinedComponent(combinedTrackParameters,
                                                 combinedWeight);
      combinedMultiState->push_back(std::move(combinedComponent));
    }
  }

  // Component reduction on the combined state
  Trk::MultiComponentState mergedState =
    QuickCloseComponentsMultiStateMerger::merge(std::move(*combinedMultiState),
                                                m_maximumNumberOfComponents);

  // Before return the weights of the states need to be renormalised to one.
  MultiComponentStateHelpers::renormaliseState(mergedState);

  return mergedState;
}

Trk::MultiComponentState
Trk::GaussianSumFitter::addCCOT(
  const EventContext& ctx,
  const Trk::TrackStateOnSurface* currentState,
  const Trk::CaloCluster_OnTrack* ccot,
  Trk::SmoothedTrajectory* smoothedTrajectory) const
{

  const Trk::MultiComponentStateOnSurface* currentMultiStateOS = nullptr;

  // Check if we already have a MultiComponent state
  if (currentState->variety() == Trk::TrackStateOnSurface::MultiComponent) {
    currentMultiStateOS =
      static_cast<const Trk::MultiComponentStateOnSurface*>(currentState);
  }

  if (!currentMultiStateOS || !ccot) {
    return {};
  }

  const Trk::MultiComponentState* currentMultiComponentState =
    currentMultiStateOS->components();
  const Trk::MeasurementBase* measurement = currentState->measurementOnTrack();
  const Trk::Surface* currentSurface(nullptr);

  if (measurement) {
    currentSurface = &(measurement->associatedSurface());
  }
  Trk::MultiComponentState extrapolatedState{};
  // Extrapolate to the Calo
  if (currentSurface) {
    extrapolatedState =
      m_extrapolator->extrapolateDirectly(ctx,
                                          *currentMultiComponentState,
                                          ccot->associatedSurface(),
                                          Trk::alongMomentum,
                                          false,
                                          Trk::nonInteracting);
  }

  // Extrapolation Failed continue
  if (extrapolatedState.empty()) {
    return {};
  }
  // Update newly extrapolated state with MeasurementBase measurement
  Trk::FitQualityOnSurface fitQuality;
  Trk::MultiComponentState updatedState =
    m_updator.update(std::move(extrapolatedState), *ccot, fitQuality);

  if (updatedState.empty()) {
    return {};
  }

  // Build TSOS with CCOT at the surface of the surface of calo
  Trk::MultiComponentStateOnSurface* updatedMCSOS =
    new Trk::MultiComponentStateOnSurface(
      ccot->clone(),
      MultiComponentStateHelpers::clone(updatedState).release(),
      (std::make_unique<Trk::FitQualityOnSurface>(fitQuality)).release());

  // Extrapolate back to the surface nearest the origin
  extrapolatedState = m_extrapolator->extrapolateDirectly(ctx,
                                                          updatedState,
                                                          *currentSurface,
                                                          Trk::oppositeMomentum,
                                                          false,
                                                          Trk::nonInteracting);

  if (extrapolatedState.empty()) {
    delete updatedMCSOS;
    return {};
  }

  // Now build a dummy measurement ....  we dont want to a double count the
  // measurement but we need to extrapolate back to origin to allow for the
  // perigee parameters to be estimated Note this only important if the track is
  // refit otherwise it has no influence.
  AmgSymMatrix(5) covMatrix;
  covMatrix.setZero();
  covMatrix(0, 0) = 1e6;

  Trk::DefinedParameter locX(0, Trk::locX);
  std::vector<Trk::DefinedParameter> defPar;
  defPar.push_back(locX);
  Trk::LocalParameters locpars(defPar);

  Trk::PseudoMeasurementOnTrack* pseudoMeasurement =
    new PseudoMeasurementOnTrack(locpars, covMatrix, *currentSurface);

  //  Combine the state using and find the mode of the distribution
  std::unique_ptr<Trk::TrackParameters> combinedState =
    MultiComponentStateCombiner::combine(extrapolatedState, true);
  auto combinedFitQuality = std::make_unique<Trk::FitQualityOnSurface>(
    m_updator.fitQuality(extrapolatedState, *ccot));

  // Build a TSOS using the dummy measurement and combined state
  Trk::MultiComponentStateOnSurface* finalSOS =
    new Trk::MultiComponentStateOnSurface(
      pseudoMeasurement,
      combinedState.release(),
      MultiComponentStateHelpers::clone(extrapolatedState).release(),
      combinedFitQuality.release());
  smoothedTrajectory->push_back(updatedMCSOS);
  smoothedTrajectory->push_back(finalSOS);

  return extrapolatedState;
}
