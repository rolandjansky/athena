/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "TrkGaussianSumFilter/IMultiComponentStateAssembler.h"
#include "TrkGaussianSumFilter/IPosteriorWeightsCalculator.h"

#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkEventPrimitives/FitQuality.h"

#include "TrkEventPrimitives/LocalParameters.h"
#include "CxxUtils/make_unique.h"

#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/Chrono.h"


Trk::GsfMeasurementUpdator::GsfMeasurementUpdator(const std::string type, const std::string name, const IInterface* parent)
  :
  AthAlgTool(type, name, parent),
  m_outputlevel(0),
  m_updator("Trk::KalmanUpdator/KalmanUpdator"),
  m_posteriorWeightsCalculator("Trk::PosteriorWeightsCalculator/PosteriorWeightsCalculator"),
  m_stateAssembler("Trk::MultiComponentStateAssembler/GsfMeasurementStateAssembler"),
  m_chronoSvc("ChronoStatSvc", name)
{
  declareInterface<IMultiStateMeasurementUpdator>(this);
  declareProperty("Updator", m_updator); //Linear updator
}


StatusCode Trk::GsfMeasurementUpdator::initialize()
{

  m_outputlevel = msg().level()-MSG::DEBUG;   // save the threshold for debug printout in private member

  // Request the Chrono Service
  if ( m_chronoSvc.retrieve().isFailure() ) {
   ATH_MSG_FATAL("Failed to retrieve service " << m_chronoSvc);
   return StatusCode::FAILURE;
  } else 
   ATH_MSG_INFO("Retrieved service " << m_chronoSvc);

  // Retrieve the updator tool
  if ( m_updator.retrieve().isFailure() ){
    ATH_MSG_FATAL("Could not retrieve measurement updator AlgTool ... Exiting!");
    return StatusCode::FAILURE;
  }

  // Retrieve the Posterior Weights Calculator
  if ( m_posteriorWeightsCalculator.retrieve().isFailure() ){
    ATH_MSG_FATAL("Could not find the Posterior Weights Calculator Service... Exiting!");
    return StatusCode::FAILURE;
  }

  // Request an instance of the MultiComponentStateAssembler
  if ( m_stateAssembler.retrieve().isFailure() ){
    ATH_MSG_ERROR("Could not access the MultiComponentStateAssembler Service");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Initialisation of " << name() << " was successful");
  return StatusCode::SUCCESS;

}

StatusCode Trk::GsfMeasurementUpdator::finalize()
{

  ATH_MSG_INFO("Finalisation of " << name() << " was successful");
  return StatusCode::SUCCESS;

}

const Trk::MultiComponentState* Trk::GsfMeasurementUpdator::update (const Trk::MultiComponentState& stateBeforeUpdate, const Trk::MeasurementBase& measurement) const
{

  ATH_MSG_VERBOSE( "Updating using GsfMeasurementUpdator");

  const Trk::MultiComponentState* updatedState = 0;

  // Point to the correct member function of the linear fitter measurement updator for fitting in the direction of momentum
  Updator updator = &Trk::IUpdator::addToState;

  // Check all components have associated error matricies
  Trk::MultiComponentState::const_iterator component = stateBeforeUpdate.begin();

  bool rebuildStateWithErrors = false;

  // Perform initial check of state awaiting update. If all states have associated error matricies then no need to perform the rebuild
  for ( ; component != stateBeforeUpdate.end(); ++component ) {
    rebuildStateWithErrors = rebuildStateWithErrors || invalidComponent( component->first ) ;
  }

  if ( rebuildStateWithErrors ){

    ATH_MSG_VERBOSE( "Rebuilding state with errors");

    Trk::MultiComponentState* stateWithInsertedErrors = rebuildState( stateBeforeUpdate );

    // Perform the measurement update with the modified state
    updatedState = calculateFilterStep(*stateWithInsertedErrors, measurement, updator);
    
    delete stateWithInsertedErrors;

    if ( !updatedState ) {
      ATH_MSG_DEBUG("Updated state could not be calculated... Returning 0" );
      return 0;
    }

    return updatedState;

  }

  // Perform the measurement update
  updatedState = calculateFilterStep(stateBeforeUpdate, measurement, updator);

  if ( !updatedState ) {
    ATH_MSG_DEBUG("Updated state could not be calculated... Returning 0" );
    return 0;
  }

  return updatedState;

}

const Trk::MultiComponentState* 
Trk::GsfMeasurementUpdator::getUnbiasedTrackParameters (const Trk::MultiComponentState& stateBeforeUpdate, const MeasurementBase& measurement) const
{

  // Point to the correct member function of the linear fitter measurement updator for fitting in the direction opposite to momentum (smoothing)
  Updator updator = &Trk::IUpdator::removeFromState;

  // Calculate the weight of each state after the measurement
  const Trk::MultiComponentState* updatedState = calculateFilterStep(stateBeforeUpdate, measurement, updator);

  return updatedState;

}

const Trk::FitQualityOnSurface* 
Trk::GsfMeasurementUpdator::fitQuality (const MultiComponentState& updatedState, const MeasurementBase& measurement) const
{

  // Fit quality assumes that a state that has been updated by the measurement updator has been supplied to it

  if ( updatedState.empty() ){
    ATH_MSG_WARNING( "Attempting to calculate chi2 of a hit with respect to an empty multiple-component state" );
    return 0;
  }

  double chi2 = 0;
  int degreesOfFreedom = 0;
  Trk::MultiComponentState::const_iterator component = updatedState.begin();

  for ( ; component != updatedState.end(); ++component){
    const Trk::TrackParameters* trackParameters = component->first;

    // IUpdator interface change (27/09/2005) to allow for fit quality calculations depending on if the track parameters incorporate the information 
    // contained in the measurement. I ALWAYS do this - hence the fullStateFitQuality method is used
    const Trk::FitQualityOnSurface* componentFitQuality = m_updator->fullStateFitQuality( *trackParameters, measurement.localParameters(), measurement.localCovariance() );

    double componentChi2 = componentFitQuality->chiSquared();

    chi2 += component->second * componentChi2;
    
    // The same measurement is included in each update so it is the same for each component
    if ( component == updatedState.begin() )
      degreesOfFreedom = componentFitQuality->numberDoF();

    delete componentFitQuality;

  }

  if ( std::isnan( chi2 ) || chi2 <= 0. ){

    return 0;
  }

  const Trk::FitQualityOnSurface* fitQualityOnSurface = new FitQualityOnSurface(chi2, degreesOfFreedom);

  return fitQualityOnSurface;
}

const Trk::MultiComponentState* 
Trk::GsfMeasurementUpdator::calculateFilterStep( const Trk::MultiComponentState& stateBeforeUpdate, 
             const Trk::MeasurementBase& measurement, 
             const Updator updator) const
{

  ATH_MSG_VERBOSE( "Calculate Filter Step");

  // Start the timer
  //Chrono chrono( &(*m_chronoSvc), "GsfMeasurementUpdate" );

  // Check that the assember is reset
  bool isAssemblerReset = m_stateAssembler->reset();

  if ( !isAssemblerReset ){
    ATH_MSG_DEBUG("Could not reset the state assembler... returning 0");
    return 0;
  }

  if ( stateBeforeUpdate.empty() ){
    ATH_MSG_WARNING("Cannot update multi-state with no components!");
    return 0;
  }

  // Calculate the weight of each component after the measurement
  const Trk::MultiComponentState* stateWithNewWeights = 0;

  stateWithNewWeights = m_posteriorWeightsCalculator->weights(stateBeforeUpdate, measurement);

  if ( !stateWithNewWeights ) {
    ATH_MSG_DEBUG( "Cacluation of state posterior weights failed... Exiting!");
    return 0;
  }
  else
    ATH_MSG_VERBOSE( "Calculation of state posterior weights successful");

  // Update each component using the specified updator
  Trk::MultiComponentState::const_iterator component = stateWithNewWeights->begin();

  for ( ; component != stateWithNewWeights->end(); ++component){

    const Trk::TrackParameters* updatedTrackParameters = 0;

    ATH_MSG_VERBOSE( "Performing update of predicted component state with measurement...");

    Trk::FitQualityOnSurface* fitQuality = 0;

    // Track update alternates between update and getUnbiasedTrackParams
    Trk::IUpdator* updatorPointer = &(*m_updator);
    updatedTrackParameters = (updatorPointer->*updator) ( *(*component).first, measurement.localParameters(), measurement.localCovariance(), fitQuality );

    //updatedTrackParameters = m_updator->addToState( *(*component).first, measurement.localParameters(), measurement.localCovariance(), fitQuality );

    if ( !updatedTrackParameters ) {
      ATH_MSG_DEBUG( "Update of state with Measurement has failed 1... Exiting!");
      if ( fitQuality )  delete fitQuality;
      continue;
    }
    
    if ( fitQuality && fitQuality->chiSquared() <= 0. ){
      ATH_MSG_DEBUG( "Fit quality of update failed... Exiting!");
      delete updatedTrackParameters;      
      delete fitQuality;
      continue;
    }

    // Clean up memory
    delete fitQuality;

    ATH_MSG_VERBOSE( "Successful measurement update with Measurement");

    // Updator does not change the weighting
    Trk::ComponentParameters updatedComponentParameters(updatedTrackParameters, component->second);

    // Add component to state being prepared for assembly and check that it is valid
    bool componentAdded = m_stateAssembler->addComponent(updatedComponentParameters);

    if ( !componentAdded )
      ATH_MSG_DEBUG( "Component could not be added to the state in the assembler");

    delete updatedTrackParameters;
  
  }
  
  delete stateWithNewWeights;

  const Trk::MultiComponentState* assembledUpdatedState = m_stateAssembler->assembledState();


 
  if(!assembledUpdatedState)
    return 0;
      
  // Renormalise state
  const Trk::MultiComponentState* renormalisedUpdatedState = assembledUpdatedState->clonedRenormalisedState();
  
  // Clean up memory
  delete assembledUpdatedState;

  
  ATH_MSG_VERBOSE( "Successful calculation of filter step"); 

  return renormalisedUpdatedState;

}


const Trk::MultiComponentState*
Trk::GsfMeasurementUpdator::update (const Trk::MultiComponentState& stateBeforeUpdate, const Trk::MeasurementBase& measurement,
                                    std::unique_ptr<FitQualityOnSurface>&   fitQoS ) const
{

  ATH_MSG_VERBOSE( "Updating using GsfMeasurementUpdator");

  const Trk::MultiComponentState* updatedState = 0;

  // Check all components have associated error matricies
  Trk::MultiComponentState::const_iterator component = stateBeforeUpdate.begin();

  bool rebuildStateWithErrors = false;

  // Perform initial check of state awaiting update. If all states have associated error matricies then no need to perform the rebuild
  for ( ; component != stateBeforeUpdate.end(); ++component ) {
    rebuildStateWithErrors = rebuildStateWithErrors || invalidComponent( component->first ) ;
  }

  if ( rebuildStateWithErrors ){

    ATH_MSG_VERBOSE( "Rebuilding state with errors");

    Trk::MultiComponentState* stateWithInsertedErrors = rebuildState( stateBeforeUpdate );
 
    // Perform the measurement update with the modified state
    updatedState = calculateFilterStep(*stateWithInsertedErrors, measurement, fitQoS);
    
    delete stateWithInsertedErrors;

    if ( !updatedState ) {
      ATH_MSG_DEBUG( "Updated state could not be calculated... Returning 0");
      fitQoS.reset();
      return 0;
    }

    return updatedState;

  }

  // Perform the measurement update
  updatedState = calculateFilterStep(stateBeforeUpdate, measurement, fitQoS);

  if ( !updatedState ) {
    ATH_MSG_DEBUG( "Updated state could not be calculated... Returning 0");
    fitQoS.reset();
    return 0;
  }

  return updatedState;

}


const Trk::MultiComponentState* 
Trk::GsfMeasurementUpdator::calculateFilterStep( const Trk::MultiComponentState& stateBeforeUpdate, 
             const Trk::MeasurementBase& measurement, 
             std::unique_ptr<FitQualityOnSurface>& fitQoS) const
{

  ATH_MSG_VERBOSE( "Calculate Filter Step");

  // Start the timer
  //Chrono chrono( &(*m_chronoSvc), "GsfMeasurementUpdate" );

  // Check that the assember is reset
  bool isAssemblerReset = m_stateAssembler->reset();

  if ( !isAssemblerReset ){
    ATH_MSG_ERROR("Could not reset the state assembler... returning 0");
    return 0;
  }

  if ( stateBeforeUpdate.empty() ){
    ATH_MSG_WARNING( "Cannot update multi-state with no components!");
    return 0;
  }

  // Calculate the weight of each component after the measurement
  const Trk::MultiComponentState* stateWithNewWeights = 0;

  stateWithNewWeights = m_posteriorWeightsCalculator->weights(stateBeforeUpdate, measurement);

  if ( !stateWithNewWeights ) {
    ATH_MSG_DEBUG( "Cacluation of state posterior weights failed... Exiting!");
    return 0;
  }
  else
    ATH_MSG_VERBOSE( "Calculation of state posterior weights successful");

  // Update each component using the specified updator
  Trk::MultiComponentState::const_iterator component = stateWithNewWeights->begin();


  double chiSquared = 0;
  int degreesOfFreedom = 0;

  for ( ; component != stateWithNewWeights->end(); ++component){

    const Trk::TrackParameters* updatedTrackParameters = 0;

    ATH_MSG_VERBOSE( "Performing update of predicted component state with measurement...");

    if (fabs((*component).first->parameters()[Trk::qOverP])>0.033333) { //GC: protection against low momentum tracks getting lost
                                                                       // cutoff is 30MeV
      ATH_MSG_DEBUG( "About to update component with p<30MeV...skipping component! (2)");
      continue;
    }

    Trk::FitQualityOnSurface* componentFitQuality = 0;

    // Track update alternates between update and getUnbiasedTrackParams
    updatedTrackParameters = m_updator->addToState( *(*component).first, measurement.localParameters(), measurement.localCovariance(), componentFitQuality );

    if ( !updatedTrackParameters ) {
      ATH_MSG_DEBUG( "Update of state with Measurement has failed 2... Exiting!");
      if ( componentFitQuality ) delete componentFitQuality;
      continue;
    }
    
    if( invalidComponent(updatedTrackParameters)  ){
      ATH_MSG_DEBUG( "Invalid cov matrix after update... Exiting!");
      ATH_MSG_VERBOSE("Original TP \n" <<  *(*component).first ); 
      if((*component).first->covariance())
        ATH_MSG_VERBOSE("Original has a COV\n " << *(*component).first->covariance() );
      ATH_MSG_VERBOSE("Measurement  \n" <<  measurement ); 
      ATH_MSG_VERBOSE("Result  \n" <<  *updatedTrackParameters ); 
      if(updatedTrackParameters->covariance())
        ATH_MSG_VERBOSE("Result has a COV\n" << *updatedTrackParameters->covariance() );
      
      delete updatedTrackParameters;      
      delete componentFitQuality;
      continue;    
    }

    if ( !componentFitQuality || componentFitQuality->chiSquared() <= 0. ){
      ATH_MSG_DEBUG( "Fit quality of update failed... Exiting!");
      delete updatedTrackParameters;      
      delete componentFitQuality;
      continue;
    }

    double componentChi2 = componentFitQuality->chiSquared();

    chiSquared += component->second * componentChi2;

    // The same measurement is included in each update so it is the same for each component
    if ( component == stateWithNewWeights->begin() )
      degreesOfFreedom = componentFitQuality->numberDoF();

    // Clean up memory
    delete componentFitQuality;

    ATH_MSG_VERBOSE( "Successful measurement update with Measurement");

    // Updator does not change the weighting
    Trk::ComponentParameters updatedComponentParameters(updatedTrackParameters, component->second);

    // Add component to state being prepared for assembly and check that it is valid
    bool componentAdded = m_stateAssembler->addComponent(updatedComponentParameters);

    if ( !componentAdded )
      ATH_MSG_DEBUG( "Component could not be added to the state in the assembler");

    delete updatedTrackParameters;

  }
  
  delete stateWithNewWeights;

  const Trk::MultiComponentState* assembledUpdatedState = m_stateAssembler->assembledState();
  
  if(!assembledUpdatedState)
    return 0;
  
  ATH_MSG_VERBOSE( "AssembledUpdatedState size: " << assembledUpdatedState->size() );

  
  fitQoS = CxxUtils::make_unique<FitQualityOnSurface>(chiSquared, degreesOfFreedom);

      
  // Renormalise state
  const Trk::MultiComponentState* renormalisedUpdatedState = assembledUpdatedState->clonedRenormalisedState();
  
  // Clean up memory
  delete assembledUpdatedState;

  
  ATH_MSG_VERBOSE( "Successful calculation of filter step: " << renormalisedUpdatedState->size()); 

  return renormalisedUpdatedState;

}

bool Trk::GsfMeasurementUpdator::invalidComponent(const Trk::TrackParameters* trackParameters ) const
{
  auto measuredCov = trackParameters->covariance();
  bool rebuildCov = false; 
  if (!measuredCov){
    rebuildCov = true;
  } else {
    for (int i(0); i<5; ++i){
      if( (*measuredCov)(i,i)  <= 0.)
        rebuildCov = true;
    }
  }
  
  return rebuildCov;
}

Trk::MultiComponentState*  Trk::GsfMeasurementUpdator::rebuildState(const Trk::MultiComponentState& stateBeforeUpdate) const
{
  Trk::MultiComponentState*   stateWithInsertedErrors = new Trk::MultiComponentState();
  const Trk::TrackParameters* trackParametersWithError = 0;

  auto component = stateBeforeUpdate.begin();

  for ( ; component != stateBeforeUpdate.end(); ++component ){

    auto trackParameters = component->first;
    auto weight = component->second;

    bool rebuildCov = invalidComponent(trackParameters);

    if ( rebuildCov ){

      if (m_outputlevel <= 0) 
        ATH_MSG_DEBUG( "No measurement associated with track parameters, creating a big one");
      AmgSymMatrix(5)* bigNewCovarianceMatrix = new AmgSymMatrix(5);
      bigNewCovarianceMatrix->setZero();
      double covarianceScaler = 1.;
      (*bigNewCovarianceMatrix)(0,0) = 250. * covarianceScaler;
      (*bigNewCovarianceMatrix)(1,1) = 250. * covarianceScaler;
      (*bigNewCovarianceMatrix)(2,2) = 0.25;
      (*bigNewCovarianceMatrix)(3,3) = 0.25;
      (*bigNewCovarianceMatrix)(4,4) = 0.001 * 0.001;
  
      AmgVector(5) par = trackParameters->parameters();
      trackParametersWithError = trackParameters->associatedSurface().createTrackParameters(par[Trk::loc1],par[Trk::loc2],par[Trk::phi],par[Trk::theta],par[Trk::qOverP], bigNewCovarianceMatrix );
      Trk::ComponentParameters componentParametersWithError( trackParametersWithError, weight );
      stateWithInsertedErrors->push_back( componentParametersWithError );

    }

    else
      stateWithInsertedErrors->push_back( *component );

  }

  return stateWithInsertedErrors;
}



