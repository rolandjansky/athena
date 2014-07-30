/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* *******************************************************************************
      GsfSmoother.cxx  -  description
      -------------------------------
begin                : Wednesday 9th March 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Implementation code for the class GsfSmoother
********************************************************************************** */

#include "TrkGaussianSumFilter/GsfSmoother.h"

#include "TrkMultiComponentStateOnSurface/MultiComponentStateOnSurface.h"
#include "TrkGaussianSumFilter/IMultiStateMeasurementUpdator.h"
#include "TrkGaussianSumFilter/IMultiComponentStateCombiner.h"
#include "TrkGaussianSumFilter/IMultiStateExtrapolator.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkSurfaces/Surface.h"


#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkCaloCluster_OnTrack/CaloCluster_OnTrack.h"


#include "GaudiKernel/ToolHandle.h"

Trk::GsfSmoother::GsfSmoother(const std::string& type, const std::string& name, const IInterface* parent)
  :
  AthAlgTool(type, name, parent),
  m_outputlevel(0),
  m_combineWithFitter(false),
  m_extrapolator(0),
  m_updator(0),
  m_merger("Trk::CloseComponentsMultiStateMerger/CloseComponentsMultiStateMerger"),
  m_combiner("Trk::MultiComponentStateCombiner/GsfSmootherCombiner")
{
  declareInterface<IGsfSmoother>(this);
  declareProperty("ComponentMerger",    m_merger);
  declareProperty("CombineStateWithFitter", m_combineWithFitter);
}

StatusCode Trk::GsfSmoother::initialize()
{

  m_outputlevel = msg().level()-MSG::DEBUG;   // save the threshold for debug printout in private member
  
  // Retrieve an instance of the component merger
  if ( m_merger.retrieve().isFailure() ){
    msg(MSG::FATAL) << "Could not retrieve the component merger tool... Exiting!" << endreq;
    return StatusCode::FAILURE;
  }

  // Request an instance of the state combiner
  if ( m_combiner.retrieve().isFailure() ){
    msg(MSG::FATAL) << "Could not retrieve an instance of the multi component state combiner... Exiting!" << endreq;
    return StatusCode::FAILURE;
  }
  
  msg(MSG::INFO) << "Initialisation of " << name() << " was successful" << endreq;

  return StatusCode::SUCCESS;

}

StatusCode Trk::GsfSmoother::finalize()
{

  msg(MSG::INFO) << "Finalisation of " << name() << " was successful" << endreq;

  return StatusCode::SUCCESS;

}

StatusCode Trk::GsfSmoother::configureTools(ToolHandle<IMultiStateExtrapolator> extrapolator, ToolHandle<IMultiStateMeasurementUpdator> measurementUpdator)
{

  m_extrapolator = extrapolator;
  m_updator      = measurementUpdator;

  msg(MSG::INFO) << "Configuration of " << name() << " was successful" << endreq;

  return StatusCode::SUCCESS;

}

Trk::SmoothedTrajectory* Trk::GsfSmoother::fit (const ForwardTrajectory& forwardTrajectory,
                                                const ParticleHypothesis particleHypothesis,
                                                const Trk::CaloCluster_OnTrack * ccot ) const
{

  if (m_outputlevel<0)
    msg(MSG::VERBOSE) << "This is the GSF Smoother!" << endreq;

  // Check that extrapolator and updator are instansiated
  if (!m_updator) {
    msg(MSG::ERROR) << "The measurement updator is not configured... Exiting!" << endreq;
    return 0;
  }

  if (!m_extrapolator) {
    msg(MSG::ERROR) << "The extrapolator is not configured... Exiting!" << endreq;
    return 0;
  }

  // Check that the forward trajectory is filled
  if ( forwardTrajectory.empty() ){
    msg(MSG::ERROR) << "Attempting to smooth an empty forward trajectory... Exiting!" << endreq;
    return 0;
  }
  
  if (m_outputlevel<0){
  
    if ( particleHypothesis == Trk::nonInteracting )
      msg(MSG::VERBOSE) << "Material effects are switched off in the Gsf Smoother" << endreq;
  
    else
      msg(MSG::VERBOSE) << "Material effects are switched on in the Gsf Smoother (type): " << particleHypothesis << endreq;

  }
    
  /* Instansiate the return trajectory. This is a vectory of TrackStateOnSurface object
     In the GSF Smoother these TrackStateOnSurface objects are the base class for the
     MultiComponentStateOnSurface. This memory should be freed by the fitter / smoother master method */

  Trk::SmoothedTrajectory* smoothedTrajectory = new SmoothedTrajectory();

  // ==========================================================================================
  // Get the initial smoother prediction. It is the last prediction in the forwards trajectory
  // ==========================================================================================

  // Check to see if the last TrackStateOnSurface object in the trajectory is a MultiComponentStateOnSurface

  const Trk::MultiComponentState* smootherPredictionMultiState = 0;

  const Trk::TrackStateOnSurface* smootherPredictionStateOnSurface = forwardTrajectory.back();

  const Trk::MultiComponentStateOnSurface* smootherPredictionMultiStateOnSurface = dynamic_cast<const Trk::MultiComponentStateOnSurface*>(smootherPredictionStateOnSurface);

  if (!smootherPredictionMultiStateOnSurface) {
    msg(MSG::DEBUG) << "GSF smoother has a single component state as starting point" << endreq;

    // Build new multi-component state
    Trk::ComponentParameters smootherPredictionComponent(smootherPredictionStateOnSurface->trackParameters(), 1.);
    smootherPredictionMultiState = new Trk::MultiComponentState( smootherPredictionComponent );
  }

  else
    smootherPredictionMultiState = smootherPredictionMultiStateOnSurface->components();

  // ==============================
  // Perform the measurement update
  // ==============================

  // The first smoother prediction state is equivalent to the last forwards prediction updated with the last measurement

  // Retrieve the first MeasurementBase object for the smoother
  const Trk::MeasurementBase* firstSmootherMeasurementOnTrack = smootherPredictionStateOnSurface->measurementOnTrack()->clone();

  if ( !firstSmootherMeasurementOnTrack ){
    msg(MSG::WARNING) << "Initial state on surface in smoother does not have an associated MeasurementBase object... returning 0" << endreq;
    delete smoothedTrajectory;
    return 0;
  }

  Trk::FitQualityOnSurface* fitQuality = 0;
  const Trk::MultiComponentState* firstSmoothedState = m_updator->update(*smootherPredictionMultiState, *firstSmootherMeasurementOnTrack, fitQuality);

  if (!firstSmoothedState) {
    delete firstSmootherMeasurementOnTrack;

    if (!smootherPredictionMultiStateOnSurface)
      delete smootherPredictionMultiState;

    if(fitQuality)
      delete fitQuality;
    
    delete smoothedTrajectory;



    if (m_outputlevel<=0)
      msg(MSG::DEBUG) << "First GSF smoothing update failed... Exiting!" << endreq;
    return 0;
  }

  // ========================
  // Get the first fitQuality
  // ========================

  //const Trk::FitQualityOnSurface* fitQuality = m_updator->fitQuality(*firstSmoothedState, *firstSmootherMeasurementOnTrack);

  const Trk::TrackParameters* combinedFirstSmoothedState = m_combiner->combine(*firstSmoothedState,true);

  const Trk::MultiComponentStateOnSurface* updatedStateOnSurface 
                = new MultiComponentStateOnSurface(firstSmootherMeasurementOnTrack, combinedFirstSmoothedState, firstSmoothedState, fitQuality);

  smoothedTrajectory->push_back(updatedStateOnSurface);

  // =====================================================
  // Clean up pointers and free memory from the prediction
  // =====================================================

  smootherPredictionStateOnSurface = 0;
  updatedStateOnSurface = 0;
  fitQuality = 0;

  if (!smootherPredictionMultiStateOnSurface)
    delete smootherPredictionMultiState;

  smootherPredictionMultiStateOnSurface = 0;
  smootherPredictionMultiState = 0;

  // =============================================================================================================================
  // Generate prediction by scaling the covariance of all components in the initial state
  // =============================================================================================================================

  if ( !firstSmoothedState->isMeasured() ){
    msg(MSG::WARNING) << "Updated state is not measured. Rejecting smoothed state... returning 0" << endreq;
    delete smoothedTrajectory;
    return 0;
  }

  // Generate a large prediction for extrapolation. This way there is no dependance on error of prediction
  // NB local Y and theta are not blown out too much to help in the TRT
  //const Trk::MultiComponentState* smoothedStateWithScaledError = firstSmoothedState->cloneWithScaledError( 200. );
  const Trk::MultiComponentState* smoothedStateWithScaledError = firstSmoothedState->cloneWithScaledError( 15., 5., 15., 5., 15. );

  if ( !smoothedStateWithScaledError ){
    msg(MSG::WARNING) << "Covariance scaling could not be performed... returning 0" << endreq;
    delete smoothedTrajectory;
    return 0;
  }

  // Perform a measurement update on this new state
  const Trk::MultiComponentState* firstUpdatedState = m_updator->update(*smoothedStateWithScaledError, *firstSmootherMeasurementOnTrack);

  if ( !firstUpdatedState ){
    msg(MSG::WARNING) << "Smoother prediction could not be determined... returning 0" << endreq;
    delete smoothedTrajectory;
    return 0;
  }

  if ( firstUpdatedState != smoothedStateWithScaledError )
    delete smoothedStateWithScaledError;

  // Clear rioOnTrack pointer
  firstSmootherMeasurementOnTrack = 0;

  // =============================================================================
  // Loop over all remaining TrackStateOnSurface objects in the forward trajectory
  // =============================================================================
  
  // This is needed to ensure correct memory management
  const Trk::MultiComponentState* updatedState = firstUpdatedState;

  Trk::ForwardTrajectory::const_reverse_iterator trackStateOnSurface = forwardTrajectory.rbegin() + 1;
 
  Trk::ForwardTrajectory::const_reverse_iterator lasttrackStateOnSurface = forwardTrajectory.rend() - 1;
  // TSOS that the cluster measuremenet will added on after .. can’t be teh last as the impact parameters will be compromised
  Trk::ForwardTrajectory::const_reverse_iterator secondLastTrackStateOnSurface = forwardTrajectory.rend() - 2;
 
 
  for ( ; trackStateOnSurface != forwardTrajectory.rend(); ++trackStateOnSurface ) {

    // Retrieve the MeasurementBase object from the TrackStateOnSurface object
    const Trk::MeasurementBase* measurement = (*trackStateOnSurface)->measurementOnTrack();

    if ( !measurement ){
      msg(MSG::WARNING) << "MeasurementBase object could not be extracted from a measurement... continuing" << endreq;
      continue;
    }

    // Clone the MeasurementBase object
    measurement = measurement->clone();

    /* Extrapolate the current multi-component state to the next measurement surface. For the smoother the direction of propagation
       is opposite to the direction of momentum */
    
    const Trk::TrackParameters* combinedState = 0;
    const AmgSymMatrix(5)* measuredCov = 0;
    if (m_outputlevel<=0){
      combinedState = m_combiner->combine( *updatedState );

      measuredCov = combinedState->covariance();

      double varQoverP(0.);

      if ( measuredCov ){
        varQoverP = (*measuredCov)(Trk::qOverP,Trk::qOverP);
      }

      ATH_MSG_DEBUG("Starting extrapolation parameters:\t" 
                    << combinedState->parameters()[Trk::phi] << "\t"
                    << combinedState->parameters()[Trk::theta] << "\t" 
                    << combinedState->parameters()[Trk::qOverP] << "\t"
                    << varQoverP );
        
      delete combinedState;
      combinedState = 0;
    }


    const Trk::MultiComponentState* extrapolatedState = m_extrapolator->extrapolate( *updatedState, 
                                                                                     measurement->associatedSurface(), 
                                                                                     Trk::oppositeMomentum, 
                                                                                     false, 
                                                                                     particleHypothesis );

    // Free memory from the updated state. Only delete if the smoothed state is combined with forwards fit.
    if ( m_combineWithFitter && updatedState != extrapolatedState )
      delete updatedState;

    updatedState = 0;

    if (!extrapolatedState) {
      ATH_MSG_DEBUG( "Extrapolation to measurement surface failed... rejecting track!");
      delete measurement;
      delete smoothedTrajectory;
      if ( !m_combineWithFitter ) delete firstUpdatedState;
      return 0;
    }
    
    

    if (m_outputlevel<=0){
      combinedState = m_combiner->combine( *extrapolatedState );

      measuredCov = combinedState->covariance();

      double varQoverP( 0. );

      if ( measuredCov ){
        varQoverP = (*measuredCov)(Trk::qOverP,Trk::qOverP);
      }

      msg(MSG::DEBUG) << "Finishing extrapolation parameters:\t" 
            << combinedState->parameters()[Trk::phi] << "\t"
            << combinedState->parameters()[Trk::theta] << "\t" 
            << combinedState->parameters()[Trk::qOverP] << "\t"
            << varQoverP << endreq;
    
      delete combinedState;
      combinedState = 0;
    }
    
      // Original measurement was flagged as  an outlier
    if( !(*trackStateOnSurface)->type(TrackStateOnSurface::Measurement) ){
      updatedState = extrapolatedState;
      
      fitQuality = new FitQuality(1,1);
      std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type(0);
      type.set( TrackStateOnSurface::Outlier );  
       
      const Trk::MultiComponentStateOnSurface* updatedStateOnSurface  =0;
      
      if(m_combineWithFitter){
        updatedStateOnSurface = new Trk::MultiComponentStateOnSurface( measurement, updatedState->clone(), fitQuality, 0, type);
      } else {
        updatedStateOnSurface = new Trk::MultiComponentStateOnSurface( measurement, updatedState, fitQuality, 0, type);      
      }
    
      
      smoothedTrajectory->push_back( updatedStateOnSurface );
      continue;
    }
  
    

    // Update newly extrapolated state with MeasurementBase measurement
    fitQuality = 0;
    updatedState = m_updator->update( *extrapolatedState, *measurement, fitQuality );

    if (!updatedState) {
      msg(MSG::WARNING) << "Could not update the multi-component state... rejecting track!" << endreq;
      delete smoothedTrajectory;
      if (fitQuality) delete fitQuality;
      return 0;
    }

    if (m_outputlevel<=0){
          
      combinedState = m_combiner->combine( *updatedState );

      measuredCov = combinedState->covariance();

      double varQoverP( 0. );

      if ( measuredCov ){
        varQoverP = (*measuredCov)(Trk::qOverP,Trk::qOverP);
      }

      msg(MSG::DEBUG) << "Update finished parameters:\t\t" 
            << combinedState->parameters()[Trk::phi] << "\t"
            << combinedState->parameters()[Trk::theta] << "\t" 
            << combinedState->parameters()[Trk::qOverP] << "\t"
            << varQoverP << endreq;
      msg(MSG::DEBUG) << "-----------------------------------------------------------------------------" << endreq;
    
      delete combinedState;
      combinedState = 0;
    }
    
    // Free memory if the extrapolated state
    if (updatedState != extrapolatedState)
      delete extrapolatedState;
    
    extrapolatedState = 0;

    /* =============================================================
       Optional combine smoother state with fitter state
       ============================================================= */

    if (m_combineWithFitter){
    
      if( fitQuality )  delete fitQuality;
      fitQuality = 0;
      
      
      const Trk::MultiComponentState* forwardsMultiState = 0;
      
      const Trk::MultiComponentStateOnSurface* forwardsMultiStateOnSurface = dynamic_cast<const Trk::MultiComponentStateOnSurface*>(*trackStateOnSurface);
      
      if (!forwardsMultiStateOnSurface) {

        // Create new multiComponentState from single state
        Trk::ComponentParameters componentParameters( (*trackStateOnSurface)->trackParameters(), 1. );
        forwardsMultiState = new Trk::MultiComponentState( componentParameters );
      }
      
      else
        forwardsMultiState = forwardsMultiStateOnSurface->components();
      
      const Trk::MultiComponentState* combinedState2 = this->combine( *forwardsMultiState, *updatedState );
      
      // Free memory in the case of a new multi-component state being created
      if (!forwardsMultiStateOnSurface)
        delete forwardsMultiState;
      
      if (!combinedState2) {
        msg(MSG::WARNING) << "Could not combine state from forward fit with smoother state... rejecting track!" << endreq;
        delete updatedState;
        delete measurement;
        delete extrapolatedState;
        delete smoothedTrajectory;
        return 0;
      }
            
      const Trk::FitQualityOnSurface* combinedFitQuality = m_updator->fitQuality( *combinedState2, *measurement );

      // In the case of combination with forwards state - push back the combined state
      const Trk::MultiComponentStateOnSurface* combinedStateOnSurface = new MultiComponentStateOnSurface( measurement, combinedState2, combinedFitQuality );
      smoothedTrajectory->push_back( combinedStateOnSurface );
      
    } else {
        
      // If combination with forwards state is not done - push back updated state
      const Trk::MultiComponentStateOnSurface* updatedStateOnSurface = 0;
      
      if ( trackStateOnSurface == lasttrackStateOnSurface){
        const Trk::TrackParameters* combinedLastState = m_combiner->combine(*updatedState,true);

        if (combinedLastState)
          updatedStateOnSurface = new Trk::MultiComponentStateOnSurface(measurement, combinedLastState, updatedState, fitQuality);
        else 
          updatedStateOnSurface = new Trk::MultiComponentStateOnSurface(measurement, updatedState, fitQuality);
        
      } else {
       
        updatedStateOnSurface = new Trk::MultiComponentStateOnSurface( measurement, updatedState, fitQuality );

      }
      
      smoothedTrajectory->push_back( updatedStateOnSurface );




      /* =============================================================
         Add measurement from calo if  it is present
         ============================================================= */
      
      if ( ccot && trackStateOnSurface == secondLastTrackStateOnSurface){
      
        const Trk::MultiComponentState* ccotState = addCCOT( updatedStateOnSurface, ccot, smoothedTrajectory);
        if(ccotState){
          updatedState = ccotState;
        }
        
      }
      
    }

  } // End for loop over all components

  // There is one updated state with new memory allocated at the end of the iterative for loop... This must be deleted
  if ( m_combineWithFitter )
    delete updatedState;

  // Clean up the memory from the first updated state - it has already been deleted if combination with fitter
  if ( !m_combineWithFitter ) delete firstUpdatedState;

  return smoothedTrajectory;

}

const Trk::MultiComponentState* Trk::GsfSmoother::combine (const Trk::MultiComponentState& forwardsMultiState, const Trk::MultiComponentState& smootherMultiState) const
{

  Trk::MultiComponentState* combinedMultiState = new MultiComponentState();
  
  /* ================================================
     Loop over all components in forwards multi-state
     ================================================ */

  Trk::MultiComponentState::const_iterator forwardsComponent = forwardsMultiState.begin();

  for ( ; forwardsComponent != forwardsMultiState.end(); ++forwardsComponent ) {

    // Need to check that all components have associated weight matricies
    const AmgSymMatrix(5)* forwardMeasuredCov = forwardsComponent->first->covariance();

    if ( !forwardMeasuredCov )
      msg(MSG::DEBUG) << "No measurement associated with forwards component... continuing for now" << endreq;

    /* ====================================================
       Loop over all components in the smoother multi-state
       ==================================================== */

    Trk::MultiComponentState::const_iterator smootherComponent = smootherMultiState.begin();
    
    for ( ; smootherComponent != smootherMultiState.end(); ++smootherComponent ) {

      // Need to check that all components have associated weight matricies
      const AmgSymMatrix(5)* smootherMeasuredCov = smootherComponent->first->covariance();

      if ( !smootherMeasuredCov && !forwardMeasuredCov ){
        msg(MSG::WARNING) << "Cannot combine two components both without associated errors... returning 0" << endreq;
        return 0;
      }

      if ( !forwardMeasuredCov ){
        if (m_outputlevel<=0) 
          msg(MSG::DEBUG) << "Forwards state without error matrix... using smoother state only" << endreq;
        Trk::ComponentParameters smootherComponentOnly( smootherComponent->first->clone(), smootherComponent->second );
        combinedMultiState->push_back( smootherComponentOnly );
        continue;
      }

      if ( !smootherMeasuredCov ){
        if (m_outputlevel<=0) 
          msg(MSG::DEBUG) << "Smoother state withour error matrix... using forwards state only" << endreq;
        Trk::ComponentParameters forwardComponentOnly( forwardsComponent->first->clone(), forwardsComponent->second );
        combinedMultiState->push_back( forwardComponentOnly );
        continue;
      }

      const AmgSymMatrix(5) summedCovariance = *forwardMeasuredCov + *smootherMeasuredCov;
      
      const AmgSymMatrix(5) K = *forwardMeasuredCov * summedCovariance.inverse();

      //if (matrixInversionError) {
      //  msg(MSG::WARNING) << "Matrix inversion failed... Exiting!" << endreq;
      //  return 0;
      //}

      const Amg::VectorX newParameters = forwardsComponent->first->parameters() + K * ( smootherComponent->first->parameters() - forwardsComponent->first->parameters() );

      const Amg::VectorX parametersDiff = forwardsComponent->first->parameters() - smootherComponent->first->parameters();

      AmgSymMatrix(5)* covarianceOfNewParameters = new AmgSymMatrix(5)(K * *smootherMeasuredCov);

      const Trk::TrackParameters* combinedTrackParameters = (forwardsComponent->first)->associatedSurface().createTrackParameters( newParameters[Trk::loc1],newParameters[Trk::loc2],newParameters[Trk::phi],newParameters[Trk::theta],newParameters[Trk::qOverP], covarianceOfNewParameters );

      // Covariance matrix object now owned by TrackParameters object. Reset pointer to prevent delete
      covarianceOfNewParameters = 0;

      // Determine weighting exponential term for the baysian smoother
      //matrixInversionError = 0;

      const AmgSymMatrix(5) invertedSummedCovariance = summedCovariance.inverse();

      //if ( matrixInversionError ){
      //  msg(MSG::WARNING) << "Matrix inversion failed... exiting" << endreq;
      //  return 0;
      //}

      // Determine the scaling factor for the new weighting. Determined from the PDF of the many-dimensional gaussian
      double exponent = parametersDiff.transpose() * invertedSummedCovariance * parametersDiff;

      double weightScalingFactor = exp( -0.5 * exponent );

      double combinedWeight = smootherComponent->second * forwardsComponent->second * weightScalingFactor;

      const Trk::ComponentParameters combinedComponent(combinedTrackParameters, combinedWeight);

      combinedMultiState->push_back(combinedComponent);

    }

  }
  
  // Component reduction on the combined state
  const Trk::MultiComponentState* mergedState = m_merger->merge(*combinedMultiState);

  if ( mergedState != combinedMultiState )
    delete combinedMultiState;

  // Before return the weights of the states need to be renormalised to one.
  const Trk::MultiComponentState* renormalisedMergedState = mergedState->clonedRenormalisedState();

  if ( renormalisedMergedState != mergedState )
    delete mergedState;

  if (m_outputlevel<0) 
    msg(MSG::VERBOSE) << "Size of combined state from smoother: " << renormalisedMergedState->size() << endreq;

  return renormalisedMergedState;

}




const Trk::MultiComponentState* Trk::GsfSmoother::addCCOT( const Trk::TrackStateOnSurface* currentState, 
                                                           const Trk::CaloCluster_OnTrack* ccot,  
                                                           Trk::SmoothedTrajectory* smoothedTrajectory) const
{


  const Trk::MultiComponentStateOnSurface* currentMultiStateOS = dynamic_cast<const Trk::MultiComponentStateOnSurface*>(currentState);

  if (!currentMultiStateOS || !ccot) return 0;

  const Trk::MultiComponentState*  currentMultiComponentState  = currentMultiStateOS->components();
  
  const Trk::MeasurementBase* measurement = currentState->measurementOnTrack();

  const Trk::Surface* currentSurface(0);
  if(measurement) currentSurface = &(measurement->associatedSurface());

  const Trk::MultiComponentState* extrapolatedState(0);
  // Extrapolate to the Calo
  if(currentSurface){
    extrapolatedState = m_extrapolator->extrapolateDirectly( *currentMultiComponentState, 
                                                              ccot->associatedSurface(), 
                                                              Trk::alongMomentum, 
                                                              false, 
                                                              Trk::nonInteracting );
  }
  

  // Extrapolation Failed continue
  if( !extrapolatedState || extrapolatedState == currentMultiComponentState ){
    ATH_MSG_DEBUG("Extrapolation to the CCOT failed .. now not being taken in account");
    return 0;
  } 

  // Update newly extrapolated state with MeasurementBase measurement
  Trk::FitQualityOnSurface* fitQuality=0;
  const Trk::MultiComponentState* updatedState  =  m_updator->update( *extrapolatedState, *ccot, fitQuality);
      

  if( !updatedState || updatedState == extrapolatedState){
    ATH_MSG_DEBUG("Update of extrapolated state with CCOT failed .. now not being taken in account"); 
    delete extrapolatedState;
    if(fitQuality) delete fitQuality;
    return 0;
  } 

  //std::cout << "\n After  Extrapolarion update -- Weight "<<(extrapolatedState->front().second) << "\n" << *(extrapolatedState->front().first) <<std::endl; 

  //std::cout << "\n After  Measurement update -- Weight "<<(updatedState->front().second) << "\n" << *(updatedState->front().first) <<std::endl; 

    
  //Measurement update using CCOT was successful  
  
  delete extrapolatedState;
  extrapolatedState = 0;
  
  //std::cout  << " \n CCOT \n" <<  *ccot <<std::endl;
  
  //Build TSOS with CCOT at the surface of the surface of calo 
  const Trk::MultiComponentStateOnSurface* updatedMCSOS 
              = new Trk::MultiComponentStateOnSurface( ccot->clone(), updatedState, fitQuality );

  
  // Extrapolate back to the surface nearest the origin
  extrapolatedState = m_extrapolator->extrapolateDirectly( *updatedState, 
                                                           *currentSurface, 
                                                           Trk::oppositeMomentum, 
                                                           false, 
                                                           Trk::nonInteracting );
                                      
  if (!extrapolatedState || extrapolatedState == updatedState){
    ATH_MSG_DEBUG("Extrapolation from CCOT to 1st measurement failed .. now not being taken in account");
    delete updatedMCSOS;
    return 0;
  }
  
  //std::cout << "\nBefore Addition -- Weight "<<(currentMultiComponentState->front().second) << "\n" << *(currentMultiComponentState->front().first) <<std::endl; 

  //std::cout << "\nAfter Addition -- Weight "<<(extrapolatedState->front().second) << "\n" << *(extrapolatedState->front().first) <<std::endl; 


  // Now build a dummy measurement ....  we dont want to a double count the measurement but
  // we need to extrapolate back to origin to allow for the perigee parameters to be estimated
  // Note this only important if the track is refit otherwise it has no influence.
  AmgSymMatrix(5)* covMatrix = new AmgSymMatrix(5);
  covMatrix->setZero();
  (*covMatrix)(0,0) = 1e6;

  Trk::LocalParameters* locpars(0);
  Trk::DefinedParameter locX( 0 ,  Trk::locX  ) ;
  std::vector<Trk::DefinedParameter> defPar ;
  defPar.push_back( locX ) ;
  locpars = new Trk::LocalParameters( defPar ) ;

  Trk::PseudoMeasurementOnTrack* pseudoMeasurement
                                  = new PseudoMeasurementOnTrack( *locpars,
                                                                  *covMatrix,
                                                                 *currentSurface);

  //  Combine the state using and find the mode of the distribution
  const Trk::TrackParameters* combinedState = m_combiner->combine(*extrapolatedState,true);
  
  const Trk::FitQualityOnSurface* combinedFitQuality = m_updator->fitQuality( *extrapolatedState, *ccot );     
  
  // Build a TSOS using the dummy measurement and combined state
  Trk::MultiComponentStateOnSurface* final = new Trk::MultiComponentStateOnSurface(pseudoMeasurement, combinedState, extrapolatedState, combinedFitQuality);
  smoothedTrajectory->push_back(updatedMCSOS);
  smoothedTrajectory->push_back(final);
  ATH_MSG_DEBUG("Successfully added CCOT ");

  return  extrapolatedState;
}
