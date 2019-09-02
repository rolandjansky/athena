/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* *******************************************************************************
      ForwardGsfFitter.cxx  -  description
      ------------------------------------
begin                : Wednesday 9th March 2005
author               : amorley, atkinson
email                : Anthony.Morley@cern.ch, Tom.Atkinson@cern.ch
decription           : Implementation code for ForwardGsfFitter class
********************************************************************************** */

#include "TrkGaussianSumFilter/ForwardGsfFitter.h"

#include "TrkMultiComponentStateOnSurface/MultiComponentStateOnSurface.h"

#include "TrkGaussianSumFilter/IMultiStateExtrapolator.h"
#include "TrkGaussianSumFilter/IMultiStateMeasurementUpdator.h"

#include "CxxUtils/make_unique.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkSurfaces/Surface.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"

Trk::ForwardGsfFitter::ForwardGsfFitter(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_cutChiSquaredPerNumberDOF(50.)
  , m_overideMaterialEffectsSwitch(false)
  , m_overideMaterialEffects(4)
  , m_overideParticleHypothesis(nonInteracting)
{

  declareInterface<IForwardGsfFitter>(this);

  declareProperty("StateChi2PerNDOFCut", m_cutChiSquaredPerNumberDOF);
  declareProperty("OverideForwardsMaterialEffects", m_overideMaterialEffectsSwitch);
  declareProperty("MaterialEffectsInForwardFitter", m_overideMaterialEffects);
}

StatusCode
Trk::ForwardGsfFitter::initialize()
{

  // Request an instance of the state combiner
  ATH_CHECK(m_stateCombiner.retrieve());
  ATH_MSG_DEBUG( "A cut on Chi2 / NDOF: " << m_cutChiSquaredPerNumberDOF << " will be applied");

  Trk::ParticleSwitcher particleSwitcher;
  m_overideParticleHypothesis = particleSwitcher.particle[m_overideMaterialEffects];
  if (m_overideMaterialEffectsSwitch){
    ATH_MSG_INFO("Material effects in forwards fitter have been overiden by jobOptions... New Trk::ParticleHypothesis: "
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
Trk::ForwardGsfFitter::configureTools(const ToolHandle<IMultiStateExtrapolator>& extrapolator,
                                      const ToolHandle<IMultiStateMeasurementUpdator>& measurementUpdator,
                                      const ToolHandle<IRIO_OnTrackCreator>& rioOnTrackCreator)
{

  m_extrapolator = extrapolator;
  m_updator = measurementUpdator;
  m_rioOnTrackCreator = rioOnTrackCreator;

  ATH_MSG_INFO("Configuration of " << name() << " was successful");

  return StatusCode::SUCCESS;
}

/*====================================================================================================================
   Forwards fit on a set of PrepRawData
   ===================================================================================================================
 */

const Trk::ForwardTrajectory*
Trk::ForwardGsfFitter::fitPRD(const Trk::PrepRawDataSet& inputPrepRawDataSet,
                              const Trk::TrackParameters& estimatedTrackParametersNearOrigin,
                              const Trk::ParticleHypothesis particleHypothesis) const
{


  // Check that the updator is instansiated
  if (!m_updator) {
    ATH_MSG_ERROR ("The measurement updator is not configured... Exiting!");
    return 0;
  }

  if (!m_extrapolator) {
     ATH_MSG_ERROR("The extrapolator is not configured... Exiting!");
    return 0;
  }

  if (!m_rioOnTrackCreator) {
    ATH_MSG_ERROR("The RIO_OnTrackCreator is not configured for use with the PrepRawData set... Exiting!");
    return 0;
  }

  if (inputPrepRawDataSet.empty()) {
    ATH_MSG_ERROR("Input PrepRawDataSet is empty... Exiting!");
    return 0;
  }

  // Configure for forwards filtering material effects overide
  Trk::ParticleHypothesis configuredParticleHypothesis;

  if (m_overideMaterialEffectsSwitch){
    configuredParticleHypothesis = m_overideParticleHypothesis;
  }
  else{
    configuredParticleHypothesis = particleHypothesis;
  }

  // Extract PrepRawDataSet into new local object and check that the PrepRawData is associated with a detector element
  Trk::PrepRawDataSet prepRawDataSet;
  Trk::PrepRawDataSet::const_iterator prepRawData = inputPrepRawDataSet.begin();

  for (; prepRawData != inputPrepRawDataSet.end(); ++prepRawData) {

    if (!(*prepRawData)->detectorElement()){
      ATH_MSG_WARNING("PrepRawData has no Element link... disregard it");
    }
    else{
      prepRawDataSet.push_back(*prepRawData);
    }
  }

  // Create new trajectory
  Trk::ForwardTrajectory* forwardTrajectory = new ForwardTrajectory();

  // Prepare the multi-component state. For starting guess this has single component, weight 1
  const AmgVector(5)& par = estimatedTrackParametersNearOrigin.parameters();
  Trk::ComponentParameters componentParametersNearOrigin(
    estimatedTrackParametersNearOrigin.associatedSurface().createTrackParameters(
      par[Trk::loc1], par[Trk::loc2], par[Trk::phi], par[Trk::theta], par[Trk::qOverP], 0 /*no errors*/),1.);
  const Trk::MultiComponentState* multiComponentStateNearOrigin =
    new Trk::MultiComponentState(componentParametersNearOrigin);

  // Loop over all PrepRawData measurements
  prepRawData = prepRawDataSet.begin();

  for (; prepRawData != prepRawDataSet.end(); ++prepRawData) {

    // Every valid step the ForwardTrajectory object passed to the stepForwardFit method is updated
    bool stepIsValid = stepForwardFit(forwardTrajectory,
                                      *prepRawData,
                                      0,
                                      (*prepRawData)->detectorElement()->surface((*prepRawData)->identify()),
                                      multiComponentStateNearOrigin,
                                      configuredParticleHypothesis);

    if (!stepIsValid) {
      ATH_MSG_DEBUG("Fitter step is not valid... Exiting!");
      delete forwardTrajectory;
      return 0;
    }
  }

  delete multiComponentStateNearOrigin;

  return forwardTrajectory;
}

/* =================================================================================================================
   Forwards fit on a set of Measurements
   ===================================================================================================================
 */

const Trk::ForwardTrajectory*
Trk::ForwardGsfFitter::fitMeasurements(const Trk::MeasurementSet& inputMeasurementSet,
                                       const Trk::TrackParameters& estimatedTrackParametersNearOrigin,
                                       const Trk::ParticleHypothesis particleHypothesis) const
{
  // Check that the updator is instansiated
  if (!m_updator) {
    ATH_MSG_ERROR("The measurement updator is not configured... Exiting!");
    return 0;
  }

  if (!m_extrapolator) {
    ATH_MSG_ERROR("The extrapolator is not configured... Exiting!");
    return 0;
  }

  if (inputMeasurementSet.empty()) {
    ATH_MSG_ERROR("Input MeasurementSet is empty... Exiting!");
    return 0;
  }

  // Configure for forwards filtering material effects overide
  Trk::ParticleHypothesis configuredParticleHypothesis;

  if (m_overideMaterialEffectsSwitch){
    configuredParticleHypothesis = m_overideParticleHypothesis;
  }
  else{
    configuredParticleHypothesis = particleHypothesis;
  }

  // This memory should be freed by the fitter / smoother master method
  Trk::ForwardTrajectory* forwardTrajectory = new ForwardTrajectory();

  // Prepare the multi-component state. For starting guess this has single component, weight 1
  const AmgVector(5)& par = estimatedTrackParametersNearOrigin.parameters();

  AmgSymMatrix(5)* covariance = 0;

  Trk::ComponentParameters componentParametersNearOrigin(
    estimatedTrackParametersNearOrigin.associatedSurface().createTrackParameters(
      par[Trk::loc1], par[Trk::loc2], par[Trk::phi], par[Trk::theta], par[Trk::qOverP], covariance /*no errors*/),1.);

  const Trk::MultiComponentState* multiComponentStateNearOrigin =
    new Trk::MultiComponentState(componentParametersNearOrigin);

  // Loop over all MeasurementBase objects in set
  Trk::MeasurementSet::const_iterator measurement = inputMeasurementSet.begin();

  for (; measurement != inputMeasurementSet.end(); ++measurement) {

    bool stepIsValid = stepForwardFit(forwardTrajectory,
                                      0,
                                      *measurement,
                                      (*measurement)->associatedSurface(),
                                      multiComponentStateNearOrigin,
                                      configuredParticleHypothesis);

    if (!stepIsValid) {
      ATH_MSG_DEBUG("Fitter step is not valid... Exiting!");
      delete forwardTrajectory;
      if (multiComponentStateNearOrigin){
        delete multiComponentStateNearOrigin;
      }
      return 0;
    }
  }
  delete multiComponentStateNearOrigin;
  return forwardTrajectory;
}

/* ===============================================================================================================
   StepForwardFit() private method
   ==============================================================================================================
 */

bool
Trk::ForwardGsfFitter::stepForwardFit(ForwardTrajectory* forwardTrajectory,
                                      const Trk::PrepRawData* originalPrepRawData,
                                      const Trk::MeasurementBase* originalMeasurement,
                                      const Trk::Surface& surface,
                                      const Trk::MultiComponentState*& updatedState,
                                      const Trk::ParticleHypothesis particleHypothesis) const
{
  // Protect against undefined Measurement or PrepRawData
  if (!originalPrepRawData && !originalMeasurement) {
    ATH_MSG_WARNING("No measurement information passed to StepForwardFit... Exiting!");
    if (updatedState) {
      delete updatedState;
      updatedState = 0;
    }
    return false;
  }

  // Protect against ForwardTrajectory not defined
  if (!forwardTrajectory) {
    ATH_MSG_WARNING("ForwardTrajectory object is not defined... Exiting!");
    if (updatedState) {
      delete updatedState;
      updatedState = 0;
    }
    return false;
  }

  // =================================================================
  // Extrapolate multi-component state to the next measurement surface
  // =================================================================

  const Trk::MultiComponentState* extrapolatedState =
    m_extrapolator->extrapolate(*updatedState, surface, Trk::alongMomentum, false, particleHypothesis);

  if (extrapolatedState && extrapolatedState != updatedState) {
    delete updatedState;
    updatedState = 0;
  }

  if (!extrapolatedState) {
    ATH_MSG_DEBUG( "Extrapolation failed... returning false");
    delete updatedState;
    updatedState = 0;
    return false;
  } else {
    updatedState = 0;
  }
  // =======================
  // Measurement Preparation
  // =======================
  const Trk::TrackParameters* combinedState = 0;
  const Trk::MeasurementBase* measurement = 0;

  if (originalMeasurement) {
    // Clone original MeasurementBase object (refit with no new calibration)
    measurement = originalMeasurement->clone();
  }

  else {
    combinedState = m_stateCombiner->combine(*extrapolatedState);

    if (!combinedState) {
      ATH_MSG_WARNING("State combination failed... exiting");
      delete extrapolatedState;
      return false;
    }

    // Create a new MeasurementBase object from PrepRawData using new calibration
    measurement = m_rioOnTrackCreator->correct(*originalPrepRawData, *combinedState);

    delete combinedState;
    combinedState = 0;
  }

  // ==========================
  // Perform measurement update
  // ==========================
  if (!measurement) {
    ATH_MSG_WARNING("Cannot use MeasurementBase for measurement update, it is not defined... Exiting!");
    delete extrapolatedState;
    return false;
  }

  std::unique_ptr<Trk::FitQualityOnSurface> fitQuality;
  updatedState = m_updator->update(*extrapolatedState, *measurement, fitQuality).release();

  if (!updatedState) {
    delete measurement;
    delete extrapolatedState;
    ATH_MSG_DEBUG("Measurement update of the state failed... Exiting!");
    return false;
  }

   // =====================
  // Determine fit quality
  // =====================

  // Bail if the fit quality is not defined:
  if (!fitQuality) {
    ATH_MSG_DEBUG("Failed to make fit quality... rejecting forwards trajectory");
    // Clean up objects associated with removed measurement
    delete measurement;
    delete updatedState;
    updatedState = 0;
    delete extrapolatedState;
    return false;
  }
  // Reject hits with excessive Chi2
  else if (fitQuality->chiSquared() > m_cutChiSquaredPerNumberDOF * fitQuality->numberDoF()) {

    ATH_MSG_DEBUG("Update with new measurement caused track to fail Chi Squared test, removing the object");

    fitQuality = CxxUtils::make_unique<FitQuality>(1, 1);

    std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type(0);
    type.set(TrackStateOnSurface::Outlier);
    const Trk::MultiComponentStateOnSurface* multiComponentStateOnSurface =
      new MultiComponentStateOnSurface(measurement, extrapolatedState->clone(), fitQuality.release(), 0, type);

    forwardTrajectory->push_back(multiComponentStateOnSurface);

    // Clean up objects associated with removed measurement
    // delete measurement;
    // delete fitQuality;

    delete updatedState;
    updatedState = extrapolatedState;

    // Reset extrapolated state pointer
    extrapolatedState = 0;

  } else {
    const Trk::MultiComponentStateOnSurface* multiComponentStateOnSurface =
      new MultiComponentStateOnSurface(measurement, extrapolatedState, fitQuality.release());

    forwardTrajectory->push_back(multiComponentStateOnSurface);
  }

  return true;
}
