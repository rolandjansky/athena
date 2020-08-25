/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   ForwardGsfFitter.cxx
 * @date   Wednesday 9th March 2005
 * @author Tom Athkinson, Anthony Morley, Christos Anastopoulos
 * @brief  Implementation code for ForwardGsfFitter class
 */

#include "TrkGaussianSumFilter/ForwardGsfFitter.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentStateOnSurface.h"

#include "TrkGaussianSumFilter/IMultiStateExtrapolator.h"
#include "TrkGaussianSumFilter/MultiComponentStateCombiner.h"

#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkSurfaces/Surface.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"

Trk::ForwardGsfFitter::ForwardGsfFitter(const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_updator{}
  , m_cutChiSquaredPerNumberDOF(50.)
  , m_overideMaterialEffects(4)
  , m_overideParticleHypothesis(nonInteracting)
  , m_overideMaterialEffectsSwitch(false)
{

  declareInterface<IForwardGsfFitter>(this);
  declareProperty("StateChi2PerNDOFCut", m_cutChiSquaredPerNumberDOF);
  declareProperty("OverideForwardsMaterialEffects",
                  m_overideMaterialEffectsSwitch);
  declareProperty("MaterialEffectsInForwardFitter", m_overideMaterialEffects);
}

StatusCode
Trk::ForwardGsfFitter::initialize()
{

  ATH_MSG_DEBUG("A cut on Chi2 / NDOF: " << m_cutChiSquaredPerNumberDOF
                                         << " will be applied");
  Trk::ParticleSwitcher particleSwitcher;
  m_overideParticleHypothesis =
    particleSwitcher.particle[m_overideMaterialEffects];
  if (m_overideMaterialEffectsSwitch) {
    ATH_MSG_INFO("Material effects in forwards fitter have been overiden by "
                 "jobOptions... New "
                 "Trk::ParticleHypothesis: "
                 << m_overideMaterialEffects);
  }

  ATH_MSG_INFO("Initialisation of " << name() << " was successful");

  return StatusCode::SUCCESS;
}

StatusCode
Trk::ForwardGsfFitter::finalize()
{
  ATH_MSG_INFO("Finalisation of " << name() << " was successful");
  return StatusCode::SUCCESS;
}

StatusCode
Trk::ForwardGsfFitter::configureTools(
  const ToolHandle<IMultiStateExtrapolator>& extrapolator,
  const ToolHandle<IRIO_OnTrackCreator>& rioOnTrackCreator)
{
  m_extrapolator = extrapolator;
  m_rioOnTrackCreator = rioOnTrackCreator;
  ATH_MSG_INFO("Configuration of " << name() << " was successful");

  return StatusCode::SUCCESS;
}

/*
 * Forwards fit on a set of PrepRawData
 */

std::unique_ptr<Trk::ForwardTrajectory>
Trk::ForwardGsfFitter::fitPRD(
  const EventContext& ctx,
  Trk::IMultiStateExtrapolator::Cache& extrapolatorCache,
  const Trk::PrepRawDataSet& inputPrepRawDataSet,
  const Trk::TrackParameters& estimatedTrackParametersNearOrigin,
  const Trk::ParticleHypothesis particleHypothesis) const
{

  // Configure for forwards filtering material effects overide
  Trk::ParticleHypothesis configuredParticleHypothesis;

  if (m_overideMaterialEffectsSwitch) {
    configuredParticleHypothesis = m_overideParticleHypothesis;
  } else {
    configuredParticleHypothesis = particleHypothesis;
  }

  // Extract PrepRawDataSet into new local object and check that the PrepRawData
  // is associated with a detector element
  Trk::PrepRawDataSet prepRawDataSet;
  Trk::PrepRawDataSet::const_iterator prepRawData = inputPrepRawDataSet.begin();

  for (; prepRawData != inputPrepRawDataSet.end(); ++prepRawData) {

    if (!(*prepRawData)->detectorElement()) {
      ATH_MSG_WARNING("PrepRawData has no Element link... disregard it");
    } else {
      prepRawDataSet.push_back(*prepRawData);
    }
  }

  // Create new trajectory
  auto forwardTrajectory = std::make_unique<Trk::ForwardTrajectory>();

  // Prepare the multi-component state. For starting guess this has single
  // component, weight 1
  const AmgVector(5)& par = estimatedTrackParametersNearOrigin.parameters();

  Trk::ComponentParameters componentParametersNearOrigin(
    estimatedTrackParametersNearOrigin.associatedSurface()
      .createTrackParameters(par[Trk::loc1],
                             par[Trk::loc2],
                             par[Trk::phi],
                             par[Trk::theta],
                             par[Trk::qOverP],
                             nullptr /*no errors*/),
    1.);

  Trk::MultiComponentState multiComponentStateNearOrigin{};
  multiComponentStateNearOrigin.push_back(
    std::move(componentParametersNearOrigin));

  // Loop over all PrepRawData measurements
  prepRawData = prepRawDataSet.begin();

  for (; prepRawData != prepRawDataSet.end(); ++prepRawData) {

    // Every valid step the ForwardTrajectory object passed to the
    // stepForwardFit method is updated
    bool stepIsValid = stepForwardFit(
      ctx,
      extrapolatorCache,
      forwardTrajectory.get(),
      *prepRawData,
      nullptr,
      (*prepRawData)->detectorElement()->surface((*prepRawData)->identify()),
      multiComponentStateNearOrigin,
      configuredParticleHypothesis);

    if (!stepIsValid) {
      ATH_MSG_DEBUG("Fitter step is not valid... Exiting!");
      return nullptr;
    }
  }
  return forwardTrajectory;
}

/*
 * Forwards fit on a set of Measurements
 */
std::unique_ptr<Trk::ForwardTrajectory>
Trk::ForwardGsfFitter::fitMeasurements(
  const EventContext& ctx,
  Trk::IMultiStateExtrapolator::Cache& extrapolatorCache,
  const Trk::MeasurementSet& inputMeasurementSet,
  const Trk::TrackParameters& estimatedTrackParametersNearOrigin,
  const Trk::ParticleHypothesis particleHypothesis) const
{

  if (!m_extrapolator) {
    ATH_MSG_ERROR("The extrapolator is not configured... Exiting!");
    return nullptr;
  }

  if (inputMeasurementSet.empty()) {
    ATH_MSG_ERROR("Input MeasurementSet is empty... Exiting!");
    return nullptr;
  }

  // Configure for forwards filtering material effects overide
  Trk::ParticleHypothesis configuredParticleHypothesis;

  if (m_overideMaterialEffectsSwitch) {
    configuredParticleHypothesis = m_overideParticleHypothesis;
  } else {
    configuredParticleHypothesis = particleHypothesis;
  }

  // This memory should be freed by the fitter / smoother master method
  auto forwardTrajectory = std::make_unique<Trk::ForwardTrajectory>();

  // Prepare the multi-component state. For starting guess this has single
  // component, weight 1
  const AmgVector(5)& par = estimatedTrackParametersNearOrigin.parameters();

  AmgSymMatrix(5)* covariance = nullptr;

  Trk::ComponentParameters componentParametersNearOrigin(
    estimatedTrackParametersNearOrigin.associatedSurface()
      .createTrackParameters(par[Trk::loc1],
                             par[Trk::loc2],
                             par[Trk::phi],
                             par[Trk::theta],
                             par[Trk::qOverP],
                             covariance /*no errors*/),
    1.);

  Trk::MultiComponentState multiComponentStateNearOrigin{};
  multiComponentStateNearOrigin.push_back(
    std::move(componentParametersNearOrigin));

  // Loop over all MeasurementBase objects in set
  Trk::MeasurementSet::const_iterator measurement = inputMeasurementSet.begin();

  for (; measurement != inputMeasurementSet.end(); ++measurement) {

    bool stepIsValid = stepForwardFit(ctx,
                                      extrapolatorCache,
                                      forwardTrajectory.get(),
                                      nullptr,
                                      *measurement,
                                      (*measurement)->associatedSurface(),
                                      multiComponentStateNearOrigin,
                                      configuredParticleHypothesis);

    if (!stepIsValid) {
      return nullptr;
    }
  }
  return forwardTrajectory;
}

/*
 *   StepForwardFit() private method
 */
bool
Trk::ForwardGsfFitter::stepForwardFit(
  const EventContext& ctx,
  Trk::IMultiStateExtrapolator::Cache& extrapolatorCache,
  ForwardTrajectory* forwardTrajectory,
  const Trk::PrepRawData* originalPrepRawData,
  const Trk::MeasurementBase* originalMeasurement,
  const Trk::Surface& surface,
  Trk::MultiComponentState& updatedState,
  const Trk::ParticleHypothesis particleHypothesis) const
{
  // Protect against undefined Measurement or PrepRawData
  if (!originalPrepRawData && !originalMeasurement) {
    ATH_MSG_WARNING(
      "No measurement information passed to StepForwardFit... Exiting!");
    return false;
  }

  // Protect against ForwardTrajectory not defined
  if (!forwardTrajectory) {
    ATH_MSG_WARNING("ForwardTrajectory object is not defined... Exiting!");
    return false;
  }

  // Extrapolate multi-component state to the next measurement surface
  Trk::MultiComponentState extrapolatedState =
    m_extrapolator->extrapolate(ctx,
                                extrapolatorCache,
                                updatedState,
                                surface,
                                Trk::alongMomentum,
                                false,
                                particleHypothesis);
  if (extrapolatedState.empty()) {
    ATH_MSG_DEBUG("Extrapolation failed... returning false");
    return false;
  } else {
    ATH_MSG_DEBUG(
      "Extrapolation worked... state size: " << extrapolatedState.size());
  }
  // =======================
  // Measurement Preparation
  // =======================
  std::unique_ptr<Trk::TrackParameters> combinedState = nullptr;
  std::unique_ptr<const Trk::MeasurementBase> measurement = nullptr;
  if (originalMeasurement) {
    // Clone original MeasurementBase object (refit with no new calibration)
    measurement.reset(originalMeasurement->clone());
  } else {
    combinedState = MultiComponentStateCombiner::combine(extrapolatedState);
    if (!combinedState) {
      ATH_MSG_WARNING("State combination failed... exiting");
      return false;
    }
    // Create a new MeasurementBase object from PrepRawData using new
    // calibration
    measurement.reset(
      m_rioOnTrackCreator->correct(*originalPrepRawData, *combinedState));
    combinedState.reset();
  }

  // Perform measurement update
  if (!measurement) {
    ATH_MSG_WARNING("Cannot use MeasurementBase for measurement update, it is "
                    "not defined... Exiting!");
    return false;
  }

  auto fitQuality = std::make_unique<Trk::FitQualityOnSurface>();
  updatedState = m_updator.update(
    std::move(*(MultiComponentStateHelpers::clone(extrapolatedState))),
    *measurement,
    *fitQuality);
  if (updatedState.empty()) {
    ATH_MSG_DEBUG("Measurement update of the state failed... Exiting!");
    return false;
  }

  // Reject hits with excessive Chi2
  if (fitQuality->chiSquared() >
      m_cutChiSquaredPerNumberDOF * fitQuality->numberDoF()) {
    ATH_MSG_DEBUG("Update with new measurement caused track to fail Chi "
                  "Squared test, removing the object");
    fitQuality = std::make_unique<FitQuality>(1, 1);
    std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type(0);
    type.set(TrackStateOnSurface::Outlier);
    const Trk::MultiComponentStateOnSurface* multiComponentStateOnSurface =
      new MultiComponentStateOnSurface(
        measurement.release(),
        MultiComponentStateHelpers::clone(extrapolatedState).release(),
        fitQuality.release(),
        nullptr,
        type);

    forwardTrajectory->push_back(multiComponentStateOnSurface);
    // Clean up objects associated with removed measurement
    updatedState = std::move(extrapolatedState);
  } else {
    const Trk::MultiComponentStateOnSurface* multiComponentStateOnSurface =
      new MultiComponentStateOnSurface(
        measurement.release(),
        MultiComponentStateHelpers::clone(extrapolatedState).release(),
        fitQuality.release());
    forwardTrajectory->push_back(multiComponentStateOnSurface);
  }
  return true;
}
