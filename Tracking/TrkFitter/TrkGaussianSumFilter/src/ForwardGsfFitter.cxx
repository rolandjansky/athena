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

#include "TrkGaussianSumFilter/IMultiStateMeasurementUpdator.h"
#include "TrkGaussianSumFilter/IMultiComponentStateCombiner.h"
#include "TrkGaussianSumFilter/IMultiStateExtrapolator.h"

#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkSurfaces/Surface.h"
#include "CxxUtils/make_unique.h"

Trk::ForwardGsfFitter::ForwardGsfFitter(const std::string& type, const std::string& name, const IInterface* parent)
  :
  AthAlgTool(type, name, parent),
  m_stateCombiner("Trk::MultiComponentStateCombiner/ForwardsFitterCombiner"),
  m_cutChiSquaredPerNumberDOF(50.),
  m_overideMaterialEffectsSwitch(false),
  m_overideMaterialEffects(4)
{

  declareInterface<IForwardGsfFitter>(this);

  declareProperty("StateChi2PerNDOFCut", m_cutChiSquaredPerNumberDOF);
  declareProperty("OverideForwardsMaterialEffects", m_overideMaterialEffectsSwitch);
  declareProperty("MaterialEffectsInForwardFitter", m_overideMaterialEffects);

}

StatusCode Trk::ForwardGsfFitter::initialize()
{
 
  // Request an instance of the state combiner
  if ( m_stateCombiner.retrieve().isFailure() ){
    msg(MSG::FATAL) << "Could not retrieve an instance of the multi component state combiner... Exiting!" << endmsg;
    return StatusCode::FAILURE;
  }

  if ( msgLvl(MSG::DEBUG)) 
    msg(MSG::DEBUG) << "A cut on Chi2 / NDOF: " << m_cutChiSquaredPerNumberDOF <<  " will be applied" << endmsg;

  Trk::ParticleSwitcher particleSwitcher;
  m_overideParticleHypothesis = particleSwitcher.particle[m_overideMaterialEffects];

  if (  m_overideMaterialEffectsSwitch )
    msg(MSG::INFO) << "Material effects in forwards fitter have been overiden by jobOptions... New Trk::ParticleHypothesis: " << m_overideMaterialEffects << endmsg;


  msg(MSG::INFO) << "Initialisation of " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;

}

StatusCode Trk::ForwardGsfFitter::finalize()
{

  msg(MSG::INFO) << "Finalisation of " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;

}

StatusCode Trk::ForwardGsfFitter::configureTools( const ToolHandle<IMultiStateExtrapolator> &extrapolator,
               const ToolHandle<IMultiStateMeasurementUpdator> &measurementUpdator,
               const ToolHandle<IRIO_OnTrackCreator> &rioOnTrackCreator )
{

  m_extrapolator      = extrapolator;
  m_updator           = measurementUpdator;
  m_rioOnTrackCreator = rioOnTrackCreator;
  
  msg(MSG::INFO) << "Configuration of " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;

}

/* =========================================================================================================================================
   =========================================================================================================================================

   Forwards fit on a set of PrepRawData 

   =========================================================================================================================================
   ========================================================================================================================================= */

const Trk::ForwardTrajectory* Trk::ForwardGsfFitter::fitPRD ( const Trk::PrepRawDataSet& inputPrepRawDataSet,
                    const Trk::TrackParameters& estimatedTrackParametersNearOrigin,
                    const Trk::ParticleHypothesis particleHypothesis ) const
{
   
  if (msgLvl(MSG::VERBOSE)) msg() << "Trk::ForwardGsfFitter::fitPRD()" << endmsg;

  // Check that the updator is instansiated
  if (!m_updator) {
    msg(MSG::ERROR) << "The measurement updator is not configured... Exiting!" << endmsg;
    return 0;
  }

  if (!m_extrapolator) {
    msg(MSG::ERROR) << "The extrapolator is not configured... Exiting!" << endmsg;
    return 0;
  }

  if (!m_rioOnTrackCreator) {
    msg(MSG::ERROR) << "The RIO_OnTrackCreator is not configured for use with the PrepRawData set... Exiting!" << endmsg;
    return 0;
  }

  if ( inputPrepRawDataSet.empty() ) {
    msg(MSG::ERROR) << "Input PrepRawDataSet is empty... Exiting!" << endmsg;
    return 0;
  }

  // Configure for forwards filtering material effects overide
  Trk::ParticleHypothesis configuredParticleHypothesis;

  if (m_overideMaterialEffectsSwitch)
    configuredParticleHypothesis = m_overideParticleHypothesis;

  else
    configuredParticleHypothesis = particleHypothesis;
  
  if ( msgLvl(MSG::VERBOSE) ){
    if ( configuredParticleHypothesis == Trk::nonInteracting )
      msg(MSG::VERBOSE) << "Material effects are switched off in the Gsf Forwards Fitter" << endmsg;
  
    else
      msg(MSG::VERBOSE) << "Material effects are switched on in the Forward Gsf Fitter (type): " << configuredParticleHypothesis << endmsg;
  }
  
  // Extract PrepRawDataSet into new local object and check that the PrepRawData is associated with a detector element
  Trk::PrepRawDataSet prepRawDataSet;
  Trk::PrepRawDataSet::const_iterator prepRawData = inputPrepRawDataSet.begin();
  
  for ( ; prepRawData != inputPrepRawDataSet.end(); ++prepRawData ) {

    if ( !(*prepRawData)->detectorElement() )
      msg(MSG::WARNING) << "PrepRawData has no Element link... disregard it" << endmsg;

    else
      prepRawDataSet.push_back( *prepRawData );

  }

  // Create new trajectory
  Trk::ForwardTrajectory* forwardTrajectory = new ForwardTrajectory();

  // Prepare the multi-component state. For starting guess this has single component, weight 1
  const AmgVector(5)& par = estimatedTrackParametersNearOrigin.parameters();
  Trk::ComponentParameters componentParametersNearOrigin( estimatedTrackParametersNearOrigin.associatedSurface().createTrackParameters(par[Trk::loc1],par[Trk::loc2],par[Trk::phi],par[Trk::theta],par[Trk::qOverP],0/*no errors*/), 1. );
  const Trk::MultiComponentState* multiComponentStateNearOrigin = new Trk::MultiComponentState( componentParametersNearOrigin );

  // Loop over all PrepRawData measurements
  prepRawData = prepRawDataSet.begin();
  
  for ( ; prepRawData != prepRawDataSet.end(); ++prepRawData ) {

    // Every valid step the ForwardTrajectory object passed to the stepForwardFit method is updated
    bool stepIsValid = stepForwardFit ( forwardTrajectory, 
                                        *prepRawData, 
                                        0,
                                        (*prepRawData)->detectorElement()->surface( (*prepRawData)->identify() ),
                                        multiComponentStateNearOrigin,
                                        configuredParticleHypothesis );

    if (!stepIsValid) {
      if (msgLvl(MSG::DEBUG)) msg() << "Fitter step is not valid... Exiting!" << endmsg;
      delete forwardTrajectory;
      return 0;
    }
  
  }

  delete multiComponentStateNearOrigin;

  return forwardTrajectory;

}

/* =========================================================================================================================================
   =========================================================================================================================================

   Forwards fit on a set of Measurements

   =========================================================================================================================================
   ========================================================================================================================================= */

const Trk::ForwardTrajectory* Trk::ForwardGsfFitter::fitMeasurements ( const Trk::MeasurementSet& inputMeasurementSet,
                       const Trk::TrackParameters& estimatedTrackParametersNearOrigin,
                       const Trk::ParticleHypothesis particleHypothesis ) const
{
  
  if (msgLvl(MSG::VERBOSE))
    msg() << "Entering GSF forward fitter... fitting with MeasurementSet" << endmsg;

  // Check that the updator is instansiated
  if (!m_updator) {
    msg(MSG::ERROR) << "The measurement updator is not configured... Exiting!" << endmsg;
    return 0;
  }

  if (!m_extrapolator) {
    msg(MSG::ERROR) << "The extrapolator is not configured... Exiting!" << endmsg;
    return 0;
  }

  if ( inputMeasurementSet.empty() ) {
    msg(MSG::ERROR) << "Input MeasurementSet is empty... Exiting!" << endmsg;
    return 0;
  }

  // Configure for forwards filtering material effects overide
  Trk::ParticleHypothesis configuredParticleHypothesis;

  if (m_overideMaterialEffectsSwitch)
    configuredParticleHypothesis = m_overideParticleHypothesis;

  else
    configuredParticleHypothesis = particleHypothesis;
  
  if (msgLvl(MSG::VERBOSE)){
    if ( configuredParticleHypothesis == Trk::nonInteracting )
      msg(MSG::VERBOSE) << "Material effects are switched off in the Forward Gsf Fitter" << endmsg;
  
    else
      msg(MSG::VERBOSE) << "Material effects are switched on in the Forward Gsf Fitter (type): " << configuredParticleHypothesis << endmsg;
  }
  
  // This memory should be freed by the fitter / smoother master method
  Trk::ForwardTrajectory* forwardTrajectory = new ForwardTrajectory();
  
  // Prepare the multi-component state. For starting guess this has single component, weight 1
  const AmgVector(5)& par = estimatedTrackParametersNearOrigin.parameters();
  
  AmgSymMatrix(5)* covariance = 0 ; 
  
  Trk::ComponentParameters componentParametersNearOrigin( estimatedTrackParametersNearOrigin.associatedSurface().createTrackParameters(par[Trk::loc1],par[Trk::loc2],par[Trk::phi],par[Trk::theta],par[Trk::qOverP],covariance/*no errors*/), 1. );
  
  const Trk::MultiComponentState* multiComponentStateNearOrigin = new Trk::MultiComponentState( componentParametersNearOrigin );
  
  // Loop over all MeasurementBase objects in set
  Trk::MeasurementSet::const_iterator measurement = inputMeasurementSet.begin();

  for ( ; measurement != inputMeasurementSet.end(); ++measurement ) {

    bool stepIsValid = stepForwardFit ( forwardTrajectory, 
                                        0, 
                                        *measurement,
                                        (*measurement)->associatedSurface(),
                                        multiComponentStateNearOrigin,
                                        configuredParticleHypothesis );

    if (!stepIsValid){
      if (msgLvl(MSG::DEBUG)) msg() << "Fitter step is not valid... Exiting!" << endmsg;
      delete forwardTrajectory;
      if ( multiComponentStateNearOrigin ) delete multiComponentStateNearOrigin;
      return 0;
    }

  }

  delete multiComponentStateNearOrigin;

  return forwardTrajectory;

}

/* =========================================================================================================================================
   =========================================================================================================================================

   StepForwardFit() private method

   =========================================================================================================================================
   ========================================================================================================================================= */

bool Trk::ForwardGsfFitter::stepForwardFit ( ForwardTrajectory* forwardTrajectory,
               const Trk::PrepRawData* originalPrepRawData,
               const Trk::MeasurementBase* originalMeasurement,
               const Trk::Surface& surface,
               const Trk::MultiComponentState*& updatedState,
               const Trk::ParticleHypothesis particleHypothesis ) const
{

  if (msgLvl(MSG::VERBOSE))
    msg() << "Trk::ForwardGsfFitter::stepForwardFit()" << endmsg;

  // Protect against undefined Measurement or PrepRawData
  if ( !originalPrepRawData && !originalMeasurement ) {
    msg(MSG::WARNING) << "No measurement information passed to StepForwardFit... Exiting!" << endmsg;
    if (updatedState){
      delete updatedState;
      updatedState=0; 
    }
    return false;
  }

  // Protect against ForwardTrajectory not defined
  if ( !forwardTrajectory ) {
    msg(MSG::WARNING) << "ForwardTrajectory object is not defined... Exiting!" << endmsg;
    if (updatedState){
      delete updatedState;
      updatedState=0; 
    }
    return false;
  }

  const Trk::TrackParameters* combinedState = 0;

  // ===============
  // Debug print-out
  // ===============
  if (msgLvl(MSG::DEBUG)){
    combinedState = m_stateCombiner->combine( *updatedState );

    if ( !combinedState ){
      msg(MSG::WARNING) << "State combination failed... exiting" << endmsg;
      return false;
    }
 
    msg(MSG::DEBUG) << "Extrapolation started with (phi, theta, q/p):\t" 
          << combinedState->parameters()[Trk::phi] << "\t"
          << combinedState->parameters()[Trk::theta] << "\t" 
          << combinedState->parameters()[Trk::qOverP] << endmsg;

    delete combinedState;
    combinedState = 0;
  }
  
  // =================================================================
  // Extrapolate multi-component state to the next measurement surface
  // =================================================================

  const Trk::MultiComponentState* extrapolatedState = m_extrapolator->extrapolate( *updatedState, 
                                                                                    surface, 
                                                                                    Trk::alongMomentum, 
                                                                                    false, 
                                                                                    particleHypothesis );



  if ( extrapolatedState && extrapolatedState != updatedState ){
    delete updatedState;
    updatedState = 0;
  }

  if ( !extrapolatedState ){
    if (msgLvl(MSG::DEBUG)) msg() << "Extrapolation failed... returning false" << endmsg;
    delete updatedState;
    updatedState = 0;
    return false;
  } else {
    updatedState = 0; 
  }

  if (msgLvl(MSG::VERBOSE)) 
    msg() << "Number of components in extrapolated state: " << extrapolatedState->size() << endmsg;


  // ===============
  // Debug print-out
  // ===============

  if (msgLvl(MSG::DEBUG)){ 
    combinedState = m_stateCombiner->combine(*extrapolatedState);

    if ( !combinedState ){
      msg(MSG::WARNING) << "State combination failed... exiting" << endmsg;
      return false;
    }

    msg() << "Extrapolation stopped with (phi, theta, q/p):\t" 
          << combinedState->parameters()[Trk::phi] << "\t"
          << combinedState->parameters()[Trk::theta] << "\t" 
          << combinedState->parameters()[Trk::qOverP] << endmsg;

    delete combinedState;
    combinedState = 0;
  }
  // =======================
  // Measurement Preparation
  // =======================

  const Trk::MeasurementBase* measurement = 0;

  if (originalMeasurement) {

    if (msgLvl(MSG::VERBOSE)) msg() << "Measurement: " << *originalMeasurement << endmsg;

    // Clone original MeasurementBase object (refit with no new calibration)
    measurement = originalMeasurement->clone();
  }

  else {
    combinedState = m_stateCombiner->combine(*extrapolatedState);
  
    if ( !combinedState ){
      msg(MSG::WARNING) << "State combination failed... exiting" << endmsg;
      delete extrapolatedState;
      return false;
    }

    // Create a new MeasurementBase object from PrepRawData using new calibration
    if (msgLvl(MSG::VERBOSE)) 
      msg() << "Create new Measurement object using predicted parameters... " << *combinedState << endmsg;

    measurement = m_rioOnTrackCreator->correct(*originalPrepRawData, *combinedState);

    delete combinedState;
    combinedState = 0;

  }

  // ==========================
  // Perform measurement update
  // ==========================

  if (!measurement) {
    msg(MSG::WARNING) << "Cannot use MeasurementBase for measurement update, it is not defined... Exiting!" << endmsg;
    delete extrapolatedState;
    return false;
  }

  std::unique_ptr<Trk::FitQualityOnSurface> fitQuality;
  updatedState = m_updator->update(*extrapolatedState, *measurement, fitQuality);
  
  if (!updatedState) {
    delete measurement;
    delete extrapolatedState;
    if (msgLvl(MSG::DEBUG)) msg() << "Measurement update of the state failed... Exiting!" << endmsg;
    return false;
  }

  if (msgLvl(MSG::VERBOSE)) {
    msg() << "Update of track parameters was successful: " << updatedState->size()<< endmsg;

  // ===============
  // Debug print-out
  // ===============
  
    combinedState = m_stateCombiner->combine( *updatedState );

    if ( !combinedState ){
      msg(MSG::WARNING) << "State combination failed... exiting" << endmsg;
      delete extrapolatedState;
      return false;
    }

    msg(MSG::DEBUG) << "Update complete with (phi, theta, q/p):\t\t" 
          << combinedState->parameters()[Trk::phi] << "\t"
          << combinedState->parameters()[Trk::theta] << "\t" 
          << combinedState->parameters()[Trk::qOverP] << endmsg;
    msg(MSG::DEBUG) << "-----------------------------------------------------------------------------" << endmsg;

  
    delete combinedState;
    combinedState = 0;
  }
  // =====================
  // Determine fit quality
  // =====================

  
  // Bail if the fit quality is not defined:
  if ( !fitQuality ){
    if (msgLvl(MSG::DEBUG))
      msg() << "Failed to make fit quality... rejecting forwards trajectory" << endmsg;
    
    // Clean up objects associated with removed measurement
    delete measurement;
    delete updatedState;
    updatedState=0;
    delete extrapolatedState;

    return false;

  }
  // Reject hits with excessive Chi2
  else if ( fitQuality->chiSquared() > m_cutChiSquaredPerNumberDOF * fitQuality->numberDoF() ) {

    if (msgLvl(MSG::DEBUG))
      msg() << "Update with new measurement caused track to fail Chi Squared test, removing the object" << endmsg;
    
    
    //

    fitQuality=  CxxUtils::make_unique<FitQuality>(1,1);
  
    std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type(0);
    type.set( TrackStateOnSurface::Outlier );
    const Trk::MultiComponentStateOnSurface* multiComponentStateOnSurface = new MultiComponentStateOnSurface(measurement, 
                                                                                                             extrapolatedState->clone(), 
                                                                                                             fitQuality.release(),
                                                                                                             0,
                                                                                                             type);
    
    
    forwardTrajectory->push_back( multiComponentStateOnSurface );
      
    
    
    // Clean up objects associated with removed measurement
    //delete measurement;
    //delete fitQuality;

    delete updatedState;
    updatedState = extrapolatedState;

    // Reset extrapolated state pointer
    extrapolatedState = 0;
  
  } else {
    if (msgLvl(MSG::VERBOSE))
      msg() << "Track with new measurement passed the chi squared test... adding state to trajectory" << endmsg;

    const Trk::MultiComponentStateOnSurface* multiComponentStateOnSurface = new MultiComponentStateOnSurface(measurement, extrapolatedState, fitQuality.release());

    forwardTrajectory->push_back( multiComponentStateOnSurface );
    
  }

  return true;

}
