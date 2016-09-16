/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      GsfExtrapolator.cxx  -  description
      -----------------------------------
begin                : Tuesday 25th January 2005
author               : amorley
email                : amorley@cern.ch
decription           : Implementation code for GsfExtrapolator class
*********************************************************************************/

#include "TrkGaussianSumFilter/GsfExtrapolator.h"

#include "TrkGaussianSumFilter/IMaterialMixtureConvolution.h"
#include "TrkGaussianSumFilter/IMultiComponentStateCombiner.h"
#include "TrkGaussianSumFilter/IMultiComponentStateMerger.h"

#include "TrkExInterfaces/IPropagator.h"
#include "TrkExInterfaces/INavigator.h"

#include "TrkExInterfaces/IMultipleScatteringUpdator.h"
#include "TrkExInterfaces/IEnergyLossUpdator.h"

#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/Layer.h"

#include "TrkSurfaces/Surface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkExUtils/MaterialUpdateMode.h"

#include "TrkGeometry/MagneticFieldProperties.h"

#include "GaudiKernel/Chrono.h"

bool useBoundaryMaterialUpdate(true);

Trk::GsfExtrapolator::GsfExtrapolator(const std::string& type, const std::string& name, const IInterface* parent)
  :
  AthAlgTool(type, name, parent),
  m_outputlevel(0),
  m_propagators(),
  m_propagatorStickyConfiguration(true),
  m_propagatorConfigurationLevel(10),
  m_propagatorSearchLevel(10),
  m_navigator("Trk::Navigator/Navigator"),
  m_materialUpdator("Trk::GsfMaterialMixtureConvolution/GsfMaterialMixtureConvolution"),
  m_merger("Trk::CloseComponentsMultiStateMerger/CloseComponentsMultiStateMerger"),
  m_stateCombiner("Trk::MultiComponentStateCombiner/GsfExtrapolatorCombiner"),
  m_msupdators("Trk::MultipleScatteringUpdator/AtlasMultipleScatteringUpdator"),
  m_elossupdators("Trk::EnergyLossUpdator/AtlasEnergyLossUpdator"),
  m_surfaceBasedMaterialEffects(false),
  m_recall(false),
  m_recallSurface(0),
  m_recallLayer(0),
  m_recallTrackingVolume(0),
  m_extrapolateCalls(0),
  m_extrapolateDirectlyCalls(0),
  m_extrapolateDirectlyFallbacks(0),
  m_navigationDistanceIncreaseBreaks(0),
  m_oscillationBreaks(0),
  m_missedVolumeBoundary(0),
  m_chronoSvc("ChronoStatSvc", name),
  m_matstates(0),
  m_fastField(false)
{

  declareInterface<IMultiStateExtrapolator>(this);

  declareProperty("Propagators",                  m_propagators               );
  
  declareProperty("SearchLevelClosestParameters", m_propagatorSearchLevel         );

  declareProperty("StickyConfiguration",          m_propagatorStickyConfiguration );

  declareProperty("Navigator",                    m_navigator                 );

  declareProperty("GsfMaterialConvolution",       m_materialUpdator           );

  declareProperty("ComponentMerger",              m_merger                    );

  declareProperty("SurfaceBasedMaterialEffects",  m_surfaceBasedMaterialEffects   );

  declareProperty("MagneticFieldProperties",      m_fastField);

}

Trk::GsfExtrapolator::~GsfExtrapolator()
{}

/* =========================================================================================================================================
   B
   =========================================================================================================================================
   Initialisation and finalisation
   =========================================================================================================================================
   ========================================================================================================================================= */

StatusCode Trk::GsfExtrapolator::initialize()
{

  m_outputlevel = msg().level()-MSG::DEBUG;   // save the threshold for debug printout in private member


  // Request the Chrono Service
  if ( m_chronoSvc.retrieve().isFailure() ) {
   msg(MSG::FATAL) << "Failed to retrieve service " << m_chronoSvc << endmsg;
   return StatusCode::FAILURE;
  } else 
   msg(MSG::INFO) << "Retrieved service " << m_chronoSvc << endmsg;

  
  // Request the Propagator AlgTools
  if ( m_propagators.size() ){

      msg(MSG::INFO) << "Attempting to retrieve propagator tool.... " << endmsg;

      if ( m_propagators.retrieve().isFailure() ){
        msg(MSG::WARNING) << "Could not find propagator..... " << m_propagators << endmsg;
        return StatusCode::FAILURE;

      } else {
        msg(MSG::INFO) << "Retrieved tools " << m_propagators << endmsg;
      }


    // Set the configuration level for the retrieved propagators
    unsigned int retrievedPropagators = m_propagators.size();

    if (!retrievedPropagators){
      msg(MSG::WARNING) << "None of the specified propagators could be retrieved! " << "Extrapolators will operate in unconfigured mode." << endmsg;
      m_propagatorConfigurationLevel = 10;
    }

    else{
      m_propagatorConfigurationLevel = m_propagators.size() - 1;
      if (m_outputlevel < 0) 
        msg(MSG::VERBOSE) << "Propagator configuration level: " << m_propagatorConfigurationLevel << endmsg;
    }
  }


  else
    msg(MSG::WARNING) << "List of propagators to retrieve is empty. Extrapolators will work in unconfigured mode" << endmsg;

  // Request the Navigation AlgTool
  if ( m_navigator.retrieve().isFailure() ){
    msg(MSG::FATAL) << "Could not retrieve the Navigator  ... Exiting!" << endmsg;
    return StatusCode::FAILURE;
  }

  // Request the Material Effects Updator AlgTool
  if ( m_materialUpdator.retrieve().isFailure() ){
    msg(MSG::FATAL) << "Could not retrieve MaterialEffectsUpdator ... Exiting!" << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve an instance of the component merger
  if ( m_merger.retrieve().isFailure() ){
    msg(MSG::FATAL) << "Could not retrieve the component merger tool... Exiting!" << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve an instance of the multi-component state combiner tool
  if ( m_stateCombiner.retrieve().isFailure() ){
    msg(MSG::FATAL) << "Could not retrieve an instance of the multi-component state combiner... Exiting!" << endmsg;
    return StatusCode::FAILURE;
  }


  if ( m_elossupdators.retrieve().isFailure() ){
    msg(MSG::FATAL) << "Could not retrieve an instance of the multi-component state combiner... Exiting!" << endmsg;
    return StatusCode::FAILURE;
  }


  if ( m_msupdators.retrieve().isFailure() ){
    msg(MSG::FATAL) << "Could not retrieve an instance of the multi-component state combiner... Exiting!" << endmsg;
    return StatusCode::FAILURE;
  }

  m_fieldProperties = m_fastField ? Trk::MagneticFieldProperties(Trk::FastField) : Trk::MagneticFieldProperties(Trk::FullField);


  msg(MSG::INFO) << "Initialisation of " << name() << " was successful" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode Trk::GsfExtrapolator::finalize()
{

  msg(MSG::INFO) << "*** Extrapolator " << name() << " performance statistics ***********" << std::endl;
  msg() << " * - Number of extrapolate() calls:                " << m_extrapolateCalls << std::endl;
  msg() << " * - Number of extrapolateDirectly() calls:        " << m_extrapolateDirectlyCalls << std::endl;
  msg() << " * - Number of extrapolateDirectly() fallbacks:    " << m_extrapolateDirectlyFallbacks << std::endl;
  msg() << " * - Number of navigation distance check breaks:   " << m_navigationDistanceIncreaseBreaks << std::endl;
  msg() << " * - Number of volume boundary search failures:    " << m_missedVolumeBoundary << std::endl;
  msg() << " * - Number of tracking volume oscillation breaks: " << m_oscillationBreaks << std::endl;
  msg() << "*****************************************************************************************************************" << endmsg;

  msg(MSG::INFO) << "Finalisation of " << name() << " was successful" << endmsg;
  return StatusCode::SUCCESS;
}

/* =========================================================================================================================================
   =========================================================================================================================================
   Extrapolation methods without configured propagator
   =========================================================================================================================================
   ========================================================================================================================================= */

const Trk::MultiComponentState* Trk::GsfExtrapolator::extrapolate( const Trk::IPropagator& propagator,
                   const Trk::MultiComponentState& multiComponentState,
                   const Trk::Surface& surface,
                   Trk::PropDirection direction,
                   Trk::BoundaryCheck boundaryCheck,
                   Trk::ParticleHypothesis particleHypothesis ) const
{

  if (m_outputlevel < 0) msg(MSG::VERBOSE) << "GSF extrapolate() in non-configured mode: " << multiComponentState.size() << endmsg;

  // If the extrapolation is to be without material effects simply revert to the extrapolateDirectly method
  if ( particleHypothesis == Trk::nonInteracting )
    return extrapolateDirectly( propagator, multiComponentState, surface, direction, boundaryCheck, particleHypothesis );

  // Surface based material effects (assumes all material is on active sensor elements)
  if ( m_surfaceBasedMaterialEffects )
    return extrapolateSurfaceBasedMaterialEffects( propagator, multiComponentState, surface, direction, boundaryCheck, particleHypothesis );

  // statistics
  ++m_extrapolateCalls;

  // Start the timer
  //Chrono chrono( &(*m_chronoSvc), "extrapolate()" );

  const Trk::Layer*           associatedLayer     = 0;
  const Trk::TrackingVolume*  startVolume         = 0;
  const Trk::TrackingVolume*  destinationVolume   = 0;
  const Trk::TrackParameters* referenceParameters = 0;

  initialiseNavigation( propagator, multiComponentState, surface, associatedLayer, startVolume, destinationVolume, referenceParameters, direction );

  // Bail to direct extrapolation if the direction cannot be determined
  if ( direction == Trk::anyDirection )
    return extrapolateDirectly( propagator, multiComponentState, surface, direction, boundaryCheck, particleHypothesis );

  const Trk::TrackParameters* combinedState = multiComponentState.begin()->first;

  const Trk::MultiComponentState* currentState = &multiComponentState;

  /* Define the initial distance between destination and current position. Destination should be determined from either
      - reference parameters (prefered if they exist) or
      - destination surface
  */

  Amg::Vector3D globalSeparation = referenceParameters ? referenceParameters->position() - combinedState->position() : surface.globalReferencePoint() - combinedState->position();
  double initialDistance = globalSeparation.mag();
  if (referenceParameters) printState("Intial Ref Parameters at next surface ", *referenceParameters);
  else ATH_MSG_DEBUG("No reference parameters?? Gobal sep (r,z):  (" <<globalSeparation.perp() << ",\t" <<globalSeparation.z() <<")" ); 

  
  // ===============
  // Debug print-out
  // ===============
  if (m_outputlevel < 0){
    msg(MSG::VERBOSE) << "extrapolate() with configuration ++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    msg() << " -> Combined state:      " << *combinedState << std::endl;
    msg() << " --------------------------------------------------------------------------------------------" << std::endl;
    msg() << " -> Destination surface: " << surface << std::endl;
    msg() << " --------------------------------------------------------------------------------------------" << std::endl;
    msg() << " -> Starting volume from navigator:    " << startVolume->volumeName() << std::endl;
    msg() << " -> Destination volume from navigator: " << destinationVolume->volumeName() << std::endl;
    msg() << " --------------------------------------------------------------------------------------------" << std::endl;
    msg() << " -> Configuration (direction/boundaryCheck/particleHypothesis): " << direction << " / " << boundaryCheck << " / " << particleHypothesis << std::endl;
    msg() << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endmsg;
  }
  
  if (m_outputlevel<=0)
    msg(MSG::DEBUG) << "Running extrapolation direction " << direction << " from: "
          << startVolume->volumeName() << " to " << destinationVolume->volumeName() << " and surface centre ("
          << surface.center().x() << ", " << surface.center().y() << ", " << surface.center().z() << ")" << endmsg;

  this->printState( "Extrapolation starting with", *combinedState );

  // Clean up memory from combiner. It is no longer needed
  combinedState = 0;

  /* There are two parts to the extrapolation:
     - Extrapolate from start point to volume boundary
     - Extrapolate from volume boundary to destination surface
  */

  const Trk::IPropagator* currentPropagator = 0;

  /* =================================================================
     Extrapolation to destination volume boundary
     ================================================================= */

  bool foundFinalBoundary(true);

  int fallbackOscillationCounter(0);

  const Trk::TrackingVolume* currentVolume  = startVolume;
  const Trk::TrackingVolume* previousVolume = 0;

  while ( currentVolume && currentVolume != destinationVolume && currentState ){

    // Configure propagator based on the current tracking volume
    currentPropagator = m_propagatorStickyConfiguration ? &propagator : &(*m_propagators[ this->propagatorType( *currentVolume ) ]);

    if (referenceParameters) printState("Old Ref Parameters at next surface ", *referenceParameters);

    // Extrapolate to volume boundary
    extrapolateToVolumeBoundary( *currentPropagator,
                                 *currentState,
                                 associatedLayer,
                                 *currentVolume,
                                 direction,
                                 particleHypothesis );

    // New current state is the state extrapolated to the tracking volume boundary.
    currentState = m_stateAtBoundarySurface.stateAtBoundary;

    // New reference parameters are the navigation parameters at the boundary surface
    referenceParameters = m_stateAtBoundarySurface.navigationParameters;
    if (referenceParameters) printState("New Ref Parameters at next surface ", *referenceParameters);

    // The volume that the extrapolation is about to enter into is called the nextVolume
    const Trk::TrackingVolume* nextVolume = m_stateAtBoundarySurface.trackingVolume;

    // Break the loop if the next tracking volume is the same as the current one
    if ( !nextVolume || nextVolume == currentVolume ){
      if (m_outputlevel<=0) msg(MSG::DEBUG) << "Navigation break: No next volume found" << endmsg;
      ++m_missedVolumeBoundary;
      foundFinalBoundary = false;
      break;
    }

    // Break the lop if an oscillation is detected
    if ( previousVolume == nextVolume )
      ++fallbackOscillationCounter;

    if ( fallbackOscillationCounter > 10 ){
      if (m_outputlevel<=0) msg(MSG::DEBUG) << "Navigation break: Oscillation" << endmsg;
      ++m_oscillationBreaks;
      foundFinalBoundary = false;
      break;
    }

    // Break the loop if the distance between the surface and the track parameters has increased
    combinedState = currentState->begin()->first;

    const TrackParameters* parametersAtDestination =  currentPropagator->propagateParameters( *combinedState, surface, direction, false, m_fieldProperties, Trk::electron );
    Amg::Vector3D newDestination; 
    if (parametersAtDestination){
      printState("Parameters at next surface ", *parametersAtDestination);
      newDestination = parametersAtDestination->position();
      delete parametersAtDestination;
    } else {
      ATH_MSG_DEBUG( "Distance check propagation Failed.  Using surface center" ); 
      newDestination = surface.center();
    }
    
    double revisedDistance = ( referenceParameters->position() - newDestination ).mag();

    double distanceChange = fabs( revisedDistance - initialDistance );

    if ( revisedDistance > initialDistance && distanceChange > 0.01 ){
      ATH_MSG_DEBUG( "Navigation break. Initial separation: " << initialDistance << " Current Sepatation: " << revisedDistance );
      ATH_MSG_DEBUG( ".... Volume  " << nextVolume->volumeName()<< " from " <<currentVolume->volumeName() ); 
      foundFinalBoundary = false;
      ++m_navigationDistanceIncreaseBreaks;
      break;
    }

    combinedState = 0;

    ATH_MSG_DEBUG ("Switching tracking volume look for Material: " << nextVolume->volumeName() );

    // Initialise the oscillation checker
    previousVolume = currentVolume;

    // As the extrapolation is moving into the next volume, the next volume -> current volume
    currentVolume = nextVolume;
    
    // Associated layer now needs to be reset
    //if(!entryLayerFound) 
    associatedLayer = 0;
    
  } // end while loop

  // Look to catch failures now
  if ( !currentState ){
    if (m_outputlevel<=0) msg(MSG::DEBUG) << "No current state at boundary... Falling back to original state" << endmsg;
    currentState = &multiComponentState;
    foundFinalBoundary = false;
  }
  
  if ( currentVolume != destinationVolume ){
    if (m_outputlevel<=0) msg(MSG::DEBUG) << "Trk::GsfExtrapolator::extrapolate failed to reach destination volume... " << endmsg;
    currentState = &multiComponentState;
    foundFinalBoundary = false;
  }

  if ( !foundFinalBoundary ){
    if (m_outputlevel<=0) msg(MSG::DEBUG) << "Could not find final boundary switch to direct EXTRAPOLATION" << endmsg;

/*    const Trk::MultiComponentState* bailOutState = extrapolateDirectly( propagator,
                                                                       *currentState,
                                                                       surface,
                                                                       direction,
                                                                       boundaryCheck,
                                                                       particleHypothesis );
*/

    const Trk::MultiComponentState* bailOutState = multiStatePropagate( propagator,
                                                                        *currentState,
                                                                        surface,
                                                                        Trk::anyDirection,
                                                                        boundaryCheck,
                                                                        particleHypothesis );

    if ( !bailOutState ){
      if (m_outputlevel<=0) msg(MSG::DEBUG) << "Fallback to propagation failed... Returning 0" << endmsg;
      return 0;
    }

    emptyGarbageBins();

    return bailOutState;

  }

  // ===============
  // Debug print-out
  // =============== 

  if (m_outputlevel<=0){
    combinedState = m_stateCombiner->combine( *currentState );

    if ( !combinedState ){
      msg(MSG::ERROR) << "State combination failed... exiting" << endmsg;
      return 0;
    }

    this->printState( "Reached final volume boundary " + currentVolume->volumeName() + " with", *combinedState );

    delete combinedState;
    combinedState = 0;
  }

  /* =================================================================
     Extrapolation from volume boundary to surface
     ================================================================= */

  // Configure propagator based on the current tracking volume
  currentPropagator = m_propagatorStickyConfiguration ? &propagator : &(*m_propagators[ this->propagatorType( *currentVolume ) ]);
  
  // extrapolate inside destination volume
  const Trk::MultiComponentState* destinationState = extrapolateInsideVolume( *currentPropagator,
                                                                              *currentState,
                                                                              surface,
                                                                              associatedLayer,
                                                                              *currentVolume,
                                                                              direction,
                                                                              boundaryCheck,
                                                                              particleHypothesis );

  // FALLBACK POINT: Crisis if extrapolation fails here... As per extrapolation to volume boundary, in emergency revert to extrapolateDirectly
  if ( destinationState == currentState ){
    destinationState = 0;
  }


  if ( destinationState &&  &((*(destinationState->begin())).first->associatedSurface()) != &surface ){
    ATH_MSG_DEBUG ( "Failed to reach destination surface  ... reached some other surface");
    if( destinationState != currentState)
      delete destinationState;
    destinationState = 0;
  }

  if ( !destinationState ){

    if (m_outputlevel<=0) msg(MSG::DEBUG) << "extrapolateInsideVolume() failed... falling back to direct propagation" << endmsg;

    destinationState = multiStatePropagate( propagator,
              *currentState,
              surface,
              Trk::anyDirection,
              boundaryCheck,
              particleHypothesis );

    // statistics
    ++m_extrapolateDirectlyFallbacks;

  }

  emptyGarbageBins();

  if ( !destinationState ){
    if (m_outputlevel<=0) msg(MSG::DEBUG) << "Extrapolation inside volume failed... returning 0" << endmsg;
    return 0;
  }

  // ===============
  // Debug print-out
  // ===============
  if (m_outputlevel<=0){

    combinedState = m_stateCombiner->combine( *destinationState );

    if ( !combinedState ){
      msg(MSG::ERROR) << "State combination failed... exiting" << endmsg;
      return 0;
    }

    this->printState( "Extrapolation completed with", *combinedState );
    msg(MSG::DEBUG) << "With "<< destinationState->size() <<  " components" << endmsg;
    msg(MSG::DEBUG) << "-----------------------------------------------------------------------------------------------------------" << endmsg;

    delete combinedState;
    combinedState = 0;
  }
  // ===============================================
  // After successful extrapolation return the state
  // ===============================================

  return destinationState;

}

const Trk::MultiComponentState* Trk::GsfExtrapolator::extrapolateDirectly( const Trk::IPropagator& propagator,
                     const Trk::MultiComponentState& multiComponentState,
                     const Trk::Surface& surface,
                     Trk::PropDirection direction,
                     Trk::BoundaryCheck boundaryCheck,
                     Trk::ParticleHypothesis particleHypothesis ) const
{

  if (m_outputlevel < 0) msg(MSG::VERBOSE) << "GSF extrapolateDirectly() in non-configured mode: " <<multiComponentState.size() <<  endmsg;

  // statistics
  ++m_extrapolateDirectlyCalls;

  resetRecallInformation();

  const Trk::TrackingVolume* currentVolume = m_navigator->highestVolume();

  if ( !currentVolume ){
    msg(MSG::WARNING) << "Current tracking volume could not be determined... returning 0" << endmsg;
    return 0;
  }

  // Propagate all components
  const Trk::MultiComponentState* returnState = multiStatePropagate( propagator,
                                                                     multiComponentState,
                                                                     surface,
                                                                     direction,
                                                                     boundaryCheck,
                                                                     particleHypothesis );

  if ( !returnState ){
    if (m_outputlevel<=0) msg(MSG::DEBUG) << "Trk::GsfExtrapolator::extrapolateDirectly() failed... returning 0" << endmsg;
    return 0;
  }

  return returnState;

}

/* =========================================================================================================================================
   =========================================================================================================================================
   Configured AlgTool methods
   =========================================================================================================================================
   ========================================================================================================================================= */

const Trk::MultiComponentState* Trk::GsfExtrapolator::extrapolate( const Trk::MultiComponentState& multiComponentState,
                                                                   const Trk::Surface& surface,
                                                                   Trk::PropDirection direction,
                                                                   Trk::BoundaryCheck boundaryCheck,
                                                                   Trk::ParticleHypothesis particleHypothesis ) const
{

  if (m_outputlevel < 0) msg(MSG::VERBOSE) << "GSF extrapolate() in configured mode" << endmsg;

  if ( multiComponentState.empty() ){
    if (m_outputlevel<=0) msg(MSG::DEBUG) << "MultiComponentState is empty... returning 0" << endmsg;
    return 0;
  }

  if (m_propagatorConfigurationLevel < 10){

    // Set the propagator to that one corresponding to the configuration level
    const Trk::IPropagator* currentPropagator = &(*m_propagators[m_propagatorConfigurationLevel]);

    const Trk::MultiComponentState* extrapolatedState = this->extrapolate( *currentPropagator,
                     multiComponentState,
                     surface,
                     direction,
                     boundaryCheck,
                     particleHypothesis );

    return extrapolatedState;

  }
  
  msg(MSG::ERROR) << "No default propagator is selected. Check job options!" << endmsg;
  return 0;

}

const Trk::MultiComponentState* Trk::GsfExtrapolator::extrapolateDirectly( const Trk::MultiComponentState& multiComponentState,
                     const Trk::Surface& surface,
                     Trk::PropDirection direction,
                     Trk::BoundaryCheck boundaryCheck,
                     Trk::ParticleHypothesis particleHypothesis ) const
{
  
  if (m_outputlevel < 0) msg(MSG::VERBOSE) << "GSF extrapolateDirectly() in configured mode" << endmsg;

  if (m_propagatorConfigurationLevel < 10){
    
    // Set the propagator to that one corresponding to the configuration level
    const Trk::IPropagator* currentPropagator = &(*m_propagators[m_propagatorConfigurationLevel]);

    if ( !currentPropagator) {
      msg(MSG::ERROR) << "No current propagator is defined... Exiting" << endmsg;
      return 0;
    }

    return this->extrapolateDirectly( *currentPropagator,
              multiComponentState,
              surface,
              direction,
              boundaryCheck,
              particleHypothesis );
  }
  
  msg(MSG::ERROR) << "No default propagator is selected. Check job options!" << endmsg;
  
  return 0;

}

/* =========================================================================================================================================
   =========================================================================================================================================

   Extrapolate to Volume Boundary!

   =========================================================================================================================================
   ========================================================================================================================================= */

void Trk::GsfExtrapolator::extrapolateToVolumeBoundary ( const Trk::IPropagator& propagator,
               const Trk::MultiComponentState& multiComponentState,
               const Trk::Layer* layer,
               const Trk::TrackingVolume& trackingVolume,
               Trk::PropDirection direction,
               Trk::ParticleHypothesis particleHypothesis ) const
{
    
  ATH_MSG_DEBUG("GSF extrapolateToVolumeBoundary() to tracking volume: " << trackingVolume.volumeName());

  // Start the timer
  //Chrono chrono( &(*m_chronoSvc), "extrapolateToVolumeBoundary()" );

  // MultiComponentState propagation and material effects
  const Trk::MultiComponentState* currentState  = &multiComponentState;

  // Determine the current layer - the state combiner is required
  
  if(!currentState){
    ATH_MSG_WARNING( "extrapolateToVolumeBoundary: Trying to extrapolate nothing?? -  return" );
    return;
  }
  
  const Trk::TrackParameters* combinedState = currentState->begin()->first; //m_stateCombiner->combine( multiComponentState );
  
  const Trk::Layer* associatedLayer = layer;

  if ( !associatedLayer )
  {
    ATH_MSG_DEBUG( "No assoicated layer passed with volume.... lets get one" );
    // Get entry layer but do not use it as  it should have already be hit if it was desired
    associatedLayer = trackingVolume.associatedLayer( combinedState->position() );
    associatedLayer = associatedLayer ? associatedLayer : trackingVolume.nextLayer( combinedState->position(), direction * combinedState->momentum().unit(), associatedLayer );

    if (associatedLayer)
      ATH_MSG_DEBUG( "Found layer in Volume " << layerRZoutput(*associatedLayer) );

  }
  // Only loop over layers if they can be found within the tracking volume
  else if ( trackingVolume.confinedLayers() && associatedLayer->layerMaterialProperties() )
  {
    //const TrackParameters* paranetersAtDestination =  currentPropagator->propagateParameters( *combinedState, associatedLayer->surfaceRepresentation(), direction, false, trackingVolume );
    //if(paranetersAtDestination)
      
    const Trk::MultiComponentState* updatedState = m_materialUpdator->postUpdate( *currentState, 
                      *layer, 
                      direction, 
                      particleHypothesis );

    // Memory clean-up
    if ( updatedState && updatedState != currentState && currentState != &multiComponentState )
      delete currentState;
      
    // Refresh updated state pointer
    updatedState = updatedState ? updatedState : currentState;
    
    if (updatedState != currentState )
      addMaterialtoVector( layer, currentState->begin()->first );
      
    //----------------------------------------
    //   Component reduction
    //---------------------------------------- 

    const Trk::MultiComponentState* reducedState = m_merger->merge(*updatedState);
     
    // Memory clean-up
    if ( reducedState && reducedState != updatedState && updatedState != &multiComponentState )
      delete updatedState;
       
    // Refresh current state pointer
    currentState = reducedState ? reducedState : updatedState;

    
  }

  // Clean up memory used by the combiner
  combinedState = 0;
    
  const Trk::MultiComponentState* nextState = 0;
    
  // If an associated surface can be found, extrapolation within the tracking volume is mandatory
  // This will take extrapolate to the last layer in the volume
  if ( associatedLayer ){

    nextState = extrapolateFromLayerToLayer( propagator, 
               *currentState, 
               trackingVolume, 
               associatedLayer, 
               0, 
               direction, 
               particleHypothesis );
    
    // Make sure reduced state is added to the list of garbage to be collected
    if ( nextState && nextState != currentState && currentState != &multiComponentState )
      delete currentState;
    
    // Refresh the current state pointer
    currentState = nextState ? nextState : currentState;
    
  }

  


  /* =============================================
     Find the boundary surface using the navigator
     ============================================= */

  Trk::NavigationCell nextNavigationCell(0,0);

  combinedState = currentState->begin()->first->clone(); //m_stateCombiner->combine( *currentState );

  const Trk::TrackingVolume*  nextVolume           = 0;
  const Trk::TrackParameters* navigationParameters = m_stateAtBoundarySurface.navigationParameters ? m_stateAtBoundarySurface.navigationParameters : combinedState;

  unsigned int navigationPropagatorIndex = 0;

  if ( m_propagatorConfigurationLevel < 10 ) {
    
    // Not the default
    while ( navigationPropagatorIndex <= m_propagatorConfigurationLevel ) {

      const Trk::IPropagator* navigationPropagator = &(*m_propagators[navigationPropagatorIndex]);

      if ( !navigationPropagator ){
        msg(MSG::WARNING) << "Navigation propagator cannot be retrieved... Continuing" << endmsg;
        continue;
      }

      nextNavigationCell   = m_navigator->nextTrackingVolume( *navigationPropagator, *navigationParameters, direction, trackingVolume );
      nextVolume           = nextNavigationCell.nextVolume;
      if (navigationPropagatorIndex >= 1)
        delete navigationParameters;
      navigationParameters = nextNavigationCell.parametersOnBoundary;

      ++navigationPropagatorIndex;

      // If the next tracking volume is found then no need to continue looping
      if ( nextVolume ) break;

    }
    
  }

  else {
    
    // The Default
    nextNavigationCell   = m_navigator->nextTrackingVolume( propagator, *navigationParameters, direction, trackingVolume );
    nextVolume           = nextNavigationCell.nextVolume;
    navigationParameters = nextNavigationCell.parametersOnBoundary;
    
  }
  
  // Clean up memory allocated by the combiner
  if ( navigationParameters != combinedState )
    delete combinedState;
  
  if ( !nextVolume ){
    if (m_outputlevel<=0) msg(MSG::DEBUG) << "Cannot find next TrackingVolume from initial tracking volume: " << trackingVolume.volumeName() << endmsg;
    
    // Reset the layer recall
    resetRecallInformation();
    
  }
  
  // !< TODO check the material on the boundary
  if (useBoundaryMaterialUpdate) {
   
    //Check for two things:
    // 1. If the next volume was found
    // 2. If there is material associated with the boundary layer.
    // If so, apply material effects update.
  
    //Get layer associated with boundary surface.
    const Trk::TrackParameters     *paramsAtBoundary     = nextNavigationCell.parametersOnBoundary;
    const Trk::Layer               *layerAtBoundary      = (paramsAtBoundary) ? (paramsAtBoundary->associatedSurface()).materialLayer() : 0;
    const Trk::TrackParameters     *matUpdatedParameters = 0;
    const Trk::MultiComponentState *matUpdatedState      = 0;

    if (nextVolume && layerAtBoundary) {

      if (layerAtBoundary->layerMaterialProperties()) {
	ATH_MSG_DEBUG("Boundary surface has material - updating properties");
        assert (currentState);
        matUpdatedState = m_materialUpdator->postUpdate(*currentState, 
                                                        *layerAtBoundary,
                                                        direction, 
                                                        particleHypothesis );
      }
    }

    //If state has changed due to boundary material, modify state, parameters accordingly.
    if ( matUpdatedState && matUpdatedState != currentState ) {

      ATH_MSG_DEBUG("Performing state update");
      
      //Clean out memory, update state.
      delete currentState;
      currentState = matUpdatedState;

      //Update navigation parameters (?).
      matUpdatedParameters = currentState->begin()->first->clone();
      if (matUpdatedParameters != navigationParameters) {
	delete navigationParameters;
	navigationParameters = matUpdatedParameters;
      }

      //Add to material vector.
      addMaterialtoVector(layerAtBoundary, currentState->begin()->first );

    }
  }
  
  // Update the boundary information      
  m_stateAtBoundarySurface.updateBoundaryInformation( currentState, navigationParameters, nextVolume );
  
  // Make sure navigation parameters and current state are added to the list of garbage to be collected
  throwIntoGarbageBin( navigationParameters );
  if ( currentState != &multiComponentState ) throwIntoGarbageBin( currentState );

  if (m_outputlevel<=0){
    combinedState = m_stateCombiner->combine( *currentState );

    printState( "State at boundary surface", *combinedState );

    delete combinedState;
  }

  return;

}

/* =========================================================================================================================================
   =========================================================================================================================================

   Extrapolate inside volume to destination surface!

   =========================================================================================================================================
   ========================================================================================================================================= */

const Trk::MultiComponentState* Trk::GsfExtrapolator::extrapolateInsideVolume ( const Trk::IPropagator& propagator,
                    const Trk::MultiComponentState& multiComponentState,
                    const Trk::Surface& surface,
                    const Trk::Layer* layer,
                    const Trk::TrackingVolume& trackingVolume,
                    Trk::PropDirection direction,
                    Trk::BoundaryCheck boundaryCheck,
                    Trk::ParticleHypothesis particleHypothesis ) const
{

  ATH_MSG_DEBUG("GSF extrapolateInsideVolume() in tracking volume: " << trackingVolume.volumeName() );

  // Start the timer
  //Chrono chrono( &(*m_chronoSvc), "extrapolateInsideVolume()" );
  
  const Trk::MultiComponentState* currentState = &multiComponentState;

  /* ==================================================
     Retrieve the destination layer
     ================================================== */
    
  // 1. Association
  const Trk::Layer* destinationLayer = surface.associatedLayer();
  
  // 2. Recall and Global Search
  if ( !destinationLayer ){
    ATH_MSG_DEBUG("No associated later to surface");
    destinationLayer = ( &surface == m_recallSurface ) ? m_recallLayer : trackingVolume.associatedLayer( surface.globalReferencePoint() );
  }
  // If there is still no destination layer, flag a warning
  if ( !destinationLayer )
    ATH_MSG_DEBUG ( "No destination layer could be found inside tracking volume: " << trackingVolume.volumeName() );


  if (layer)            ATH_MSG_DEBUG("Current layer     " << layerRZoutput(*layer) );
  if (destinationLayer) ATH_MSG_DEBUG("Destination layer " << layerRZoutput(*destinationLayer) );
 

  /* ==================================================
     Retrieve the current layer
     ================================================== */

  // Produce a combined state
  const Trk::TrackParameters* combinedState = currentState->begin()->first;

  const Trk::Layer* associatedLayer = layer;

  if ( !associatedLayer ){
  
  
    ATH_MSG_DEBUG( "No assoicated layer passed with volume.... lets get one" );
    // Get entry layer but do not use it as  it should have already be hit if it was desired
    associatedLayer = trackingVolume.associatedLayer( combinedState->position() );
    associatedLayer = associatedLayer ? associatedLayer : trackingVolume.nextLayer( combinedState->position(), direction * combinedState->momentum().unit(), associatedLayer );

    if (associatedLayer)
      ATH_MSG_DEBUG( "Found layer in Volume " << layerRZoutput(*associatedLayer) );

  }

  else if ( associatedLayer != destinationLayer && trackingVolume.confinedLayers() &&  associatedLayer->layerMaterialProperties() ){

    const Trk::MultiComponentState* updatedState = m_materialUpdator->postUpdate( *currentState, 
										  *associatedLayer, 
										  direction,
										  particleHypothesis );
    
    // Memory clean-up
    if ( updatedState && updatedState != currentState && currentState != &multiComponentState )
      delete currentState;

    // Refresh the updated state pointer
    updatedState = updatedState ? updatedState : currentState;
  
      
    if(updatedState != currentState )
      addMaterialtoVector( associatedLayer, currentState->begin()->first );

    /* ----------------------------------------
       Component reduction
       ---------------------------------------- */
    
    const Trk::MultiComponentState* reducedState = m_merger->merge(*updatedState );
    
    // Memory clean-up
    if ( reducedState && reducedState != updatedState && updatedState != &multiComponentState )
      delete updatedState;

    // Refresh the current state pointer
    currentState = reducedState ? reducedState : updatedState;

  }
  
  // Memory clean-up
  combinedState = 0;
  
  if ( destinationLayer ){
    
    // If there are intermediate layers then additional extrapolations need to be done  
    if ( associatedLayer && associatedLayer != destinationLayer ){
      
      const Trk::MultiComponentState* nextState = extrapolateFromLayerToLayer( propagator, 
                         *currentState, 
                         trackingVolume, 
                         associatedLayer, 
                         destinationLayer,
                         direction,
                         particleHypothesis );

      // Memory clean-up
      if ( nextState && nextState != currentState && currentState != &multiComponentState )
        delete currentState;

      // Refresh the current state pointer
      currentState = nextState ? nextState : currentState;

    }

    // Final extrapolation to destination surface
    const Trk::MultiComponentState* returnState = extrapolateToDestinationLayer( propagator, 
                     *currentState, 
                     surface, 
                     *destinationLayer, 
                     //trackingVolume,
                     associatedLayer,
                     direction, 
                     boundaryCheck, 
                     particleHypothesis );

    // Memory clean-up
    if ( returnState && returnState != currentState && currentState != &multiComponentState )
      delete currentState;
    
    returnState = returnState ? returnState : currentState;
    
    // Set the information for the current layer, surface, tracking volume
    setRecallInformation( surface, *destinationLayer, trackingVolume );
    
    if (m_outputlevel < 0) msg(MSG::VERBOSE) << "Successfully extrapolated inside volume  " << returnState->size() <<endmsg;
    
    return returnState;
    
  }

  // FALLBACK POINT: If no destination layer is found fall-back and extrapolate directly
  ATH_MSG_DEBUG("extrapolateInsideVolume() could not find the destination layer... propagating directly to surface");

  const Trk::MultiComponentState* returnState = multiStatePropagate( propagator,
                     *currentState,
                     surface,
                     direction,
                     boundaryCheck,
                     particleHypothesis );

  // Memory clean-up
  if ( returnState && returnState != currentState && currentState != &multiComponentState )
    delete currentState;

  // No destination layer exists so layer recall method cannot be used and should be reset
  resetRecallInformation();

  return returnState;

}

/* =========================================================================================================================================
   =========================================================================================================================================

   Extrapolate from Layer to Layer

   =========================================================================================================================================
   ========================================================================================================================================= */

const Trk::MultiComponentState* Trk::GsfExtrapolator::extrapolateFromLayerToLayer( const IPropagator& propagator,
                       const MultiComponentState& multiComponentState,
                       const TrackingVolume& trackingVolume,
                       const Layer* startLayer,
                       const Layer* destinationLayer,
                       PropDirection direction,
                       ParticleHypothesis particleHypothesis ) const
{

  ATH_MSG_DEBUG("Starting extrapolateFromLayerToLayer()" );

  const Trk::Layer*               currentLayer = startLayer;
  const Trk::MultiComponentState* currentState = &multiComponentState;

  // Find the combined state to find the next layer 
  if(!currentState){
    ATH_MSG_WARNING( "extrapolateFromLayerToLayer: Trying to extrapolate nothing?? -  return" );
    return 0;
  }
  const Trk::TrackParameters* combinedState = currentState->begin()->first;

  // No need to extrapolate to start layer, find the next one
  const Trk::Layer* nextLayer = currentLayer->nextLayer( combinedState->position(), direction * combinedState->momentum().unit() );

  if (startLayer)       ATH_MSG_DEBUG("Current layer     " << layerRZoutput(*startLayer) );
  if (destinationLayer) ATH_MSG_DEBUG("Destination layer " << layerRZoutput(*destinationLayer) );
  if (nextLayer)        ATH_MSG_DEBUG("Next layer        " << layerRZoutput(*nextLayer) );

  // Clean up memory
  combinedState =0;

  // Begin while loop over all intermediate layers
  while ( nextLayer && nextLayer != destinationLayer ){
    ATH_MSG_DEBUG("NextLayer, walking next step." );

    const Trk::MultiComponentState* nextState = 0;

    // Only extrapolate to an intermediate layer if it requires material update... otherwise step over it
    if ( nextLayer && nextLayer->layerMaterialProperties() ){
      ATH_MSG_DEBUG("NextLayer, has material extrapolate to it" );

      nextState = extrapolateToIntermediateLayer( propagator, 
              *currentState, 
              *nextLayer, 
              trackingVolume, 
              direction, 
              particleHypothesis );

      // Memory clean-up
      if ( nextState && nextState != currentState && currentState != &multiComponentState )
        delete currentState;

      // Refresh current state pointer
      currentState = nextState ? nextState : currentState;

    }

    // Find the next layer
    combinedState = currentState->begin()->first;  //m_stateCombiner->combine( *currentState );
    
    currentLayer = nextLayer;
    nextLayer = currentLayer->nextLayer( combinedState->position(), direction * combinedState->momentum().unit() );
  }
  
  if (destinationLayer && nextLayer != destinationLayer  && currentState != &multiComponentState){
    ATH_MSG_DEBUG("extrapolateFromLayerToLayer failed to reach destination layer..  return 0");
    if (currentLayer)     ATH_MSG_DEBUG("Current layer     " << layerRZoutput(*currentLayer) );
    if (nextLayer)        ATH_MSG_DEBUG("NextLayer layer   " << layerRZoutput(*nextLayer) );
    if (destinationLayer) ATH_MSG_DEBUG("Destination layer " << layerRZoutput(*destinationLayer) );
    delete currentState;
    currentState = 0;
  }

  return currentState;
  
}

/* =========================================================================================================================================
   =========================================================================================================================================

   Extrapolate to Intermediate Layer

   =========================================================================================================================================
   ========================================================================================================================================= */

const Trk::MultiComponentState* Trk::GsfExtrapolator::extrapolateToIntermediateLayer( const Trk::IPropagator& propagator,
                          const Trk::MultiComponentState& multiComponentState,
                          const Trk::Layer& layer,
                          const Trk::TrackingVolume& trackingVolume,
                          Trk::PropDirection direction,
                          Trk::ParticleHypothesis particleHypothesis,
                          bool doPerpCheck ) const
{

  const Trk::TrackParameters* combinedState = 0;
  if (m_outputlevel<=0){
    combinedState = m_stateCombiner->combine( multiComponentState );

    msg(MSG::VERBOSE) << "GSF extrapolateToIntermediateLayer()" << endmsg;
    msg(MSG::VERBOSE) << "Starting track parameters (combined state): " << *combinedState << endmsg;
    msg(MSG::VERBOSE) << "Destination layer (surface representation): " << layer.surfaceRepresentation() << endmsg;

    msg(MSG::VERBOSE) << "-----------------------------------------------------------------------------------------------------------" << endmsg;

    delete combinedState;
  }

  printState( "extrapolateToIntermediateLayer().  Starting  with ", *(multiComponentState.begin()->first) );


  const Trk::Surface* startSurface = &(multiComponentState.begin()->first->associatedSurface());
  if (startSurface){
    const Trk::Layer*  startLayer = startSurface->associatedLayer();
    if (startLayer){
      ATH_MSG_DEBUG ("Starting Layer: " << layerRZoutput(*startLayer) );
    } else {
      ATH_MSG_DEBUG ("Starting Surface has no associated layer" );
    }
  } else {
    ATH_MSG_DEBUG ("Initial parameters have no surafce associated to them" );
  }
  ATH_MSG_DEBUG ("Destination Layer: " << layerRZoutput(layer) );


  const Trk::MultiComponentState* initialState = &multiComponentState;

  // Propagate over all components
  const Trk::MultiComponentState* destinationState = multiStatePropagate( propagator,
      *initialState,
      layer.surfaceRepresentation(),
      direction,
      true,
      particleHypothesis );

  if ( !destinationState ){
    ATH_MSG_DEBUG("Multi-state propagation failed... Returning 0!");
    ATH_MSG_VERBOSE("Failed to reach layer" << layer.surfaceRepresentation() );
    return 0;
  }

  // the layer has been intersected ------------------------------------------------------------------------
  // check for radial direction change ---------------------------------------------------------------------
  int rDirection    = radialDirection(multiComponentState,direction);
  int newrDirection = radialDirection(*destinationState,direction);
  if (newrDirection != rDirection && doPerpCheck){
    // it is unfortunate that the cancelling could invalidate the material collection 
    ATH_MSG_DEBUG("  [!] Perpendicular direction of track has changed -- checking");
    // reset the nextParameters if the radial change is not allowed 
    //  resetting is ok - since the parameters are in the garbage bin already
    if (!radialDirectionCheck(propagator, multiComponentState, *destinationState,trackingVolume,direction,particleHypothesis)){
      ATH_MSG_DEBUG("  [+] Perpendicular direction check cancelled this layer intersection.");
      delete destinationState;
      return 0;
    }        
  }


  if ( destinationState != initialState && initialState != &multiComponentState )
    delete initialState;

  if (m_outputlevel<=0){
    combinedState = m_stateCombiner->combine( *destinationState );

    this->printState( "Propagation to intermediate completed with", *combinedState );

    delete combinedState;
  } 
  /* -------------------------------------
     Material effects
     ------------------------------------- */

  const Trk::MultiComponentState* updatedState = 0;



  updatedState = m_materialUpdator->update( *destinationState, layer, direction, particleHypothesis );

  // Memory clean-up
  if ( updatedState && updatedState != destinationState && destinationState != &multiComponentState ) 
    delete destinationState;

  updatedState = updatedState ? updatedState : destinationState;

  if(updatedState!=destinationState)
    addMaterialtoVector( &layer, updatedState->begin()->first, direction, particleHypothesis );


  /* -------------------------------------
     Component reduction
     ------------------------------------- */

  const Trk::MultiComponentState* reducedState = m_merger->merge( *updatedState );

  if ( reducedState && reducedState != updatedState && updatedState != &multiComponentState )
    delete updatedState;

  reducedState = reducedState ? reducedState : updatedState;



  if (m_outputlevel<=0){

    combinedState = m_stateCombiner->combine( *reducedState );

    this->printState( "Material update at intermediate completed with", *combinedState );
    msg(MSG::DEBUG) << "-----------------------------------------------------------------------------------------------------------" << endmsg;

    delete combinedState;
  }

  return reducedState;

}

/* =========================================================================================================================================
   =========================================================================================================================================

   Extrapolate to Destination Layer

   =========================================================================================================================================
   ========================================================================================================================================= */

const Trk::MultiComponentState* Trk::GsfExtrapolator::extrapolateToDestinationLayer ( const Trk::IPropagator& propagator,
                          const Trk::MultiComponentState& multiComponentState,
                          const Trk::Surface& surface,
                          const Trk::Layer& layer,
                          //const Trk::TrackingVolume& trackingVolume,
                          const Trk::Layer* startLayer,
                          Trk::PropDirection direction,
                          Trk::BoundaryCheck boundaryCheck,
                          Trk::ParticleHypothesis particleHypothesis ) const
{

  if (m_outputlevel < 0) msg(MSG::VERBOSE) << "GSF extrapolateToDestinationLayer()" << endmsg;

  const Trk::MultiComponentState* initialState = &multiComponentState;

  const Trk::TrackParameters* combinedState = 0;

  if (m_outputlevel<=0){

    combinedState = m_stateCombiner->combine( *initialState );

    msg(MSG::DEBUG) << "-----------------------------------------------------------------------------------------------------------" << endmsg;
    this->printState( "Starting extrapolation to destination with", *combinedState );
  
    delete combinedState;
  }

  // Propagate over all components
  const Trk::MultiComponentState* destinationState = multiStatePropagate( propagator,
                    multiComponentState,
                    surface,
                    direction,
                    boundaryCheck,
                    particleHypothesis );
  
  // Require a fall-back if the initial state is close to the destination surface then a fall-back solution is required

  if ( !destinationState ){

    combinedState = initialState->begin()->first;

    if ( surface.isOnSurface( combinedState->position(), true, 0.5 * layer.thickness() ) ){

      if (m_outputlevel<=0) msg(MSG::DEBUG) << "Initiating fall-back from failed propagation" << endmsg;

      destinationState = multiStatePropagate( propagator,
                *initialState,
                surface,
                Trk::anyDirection,
                boundaryCheck,
                particleHypothesis );

    }

    // Memory clean-up
    combinedState = 0;

    if ( !destinationState ){
      if (m_outputlevel<=0) msg(MSG::DEBUG) << "Propagated state is empty... returning 0" << endmsg;
      return 0;
    }

  }
  
    ATH_MSG_VERBOSE(  "DestinationState state has  " << destinationState->size() << " components " );


  if (m_outputlevel<=0){

    combinedState = m_stateCombiner->combine( *destinationState );

    this->printState( "Propagation to destination completed with", *combinedState );

    delete combinedState;

  }

  /* ----------------------------------------
     Material effects
     ---------------------------------------- */
  
   //std::cout << "CALLING PREUPDATE      " <<std::endl;
  
  const Trk::MultiComponentState* updatedState = ( startLayer != &layer ) ? m_materialUpdator->preUpdate( *destinationState, 
                            layer, 
                            direction, 
                            particleHypothesis ) : destinationState;

  if ( updatedState && updatedState != destinationState && destinationState != &multiComponentState )
    delete destinationState;

  updatedState = updatedState ? updatedState : destinationState;

  ATH_MSG_VERBOSE(  "Updated state has  " << updatedState->size() << " components " );


  if(updatedState != destinationState)
    addMaterialtoVector( &layer, updatedState->begin()->first, direction, particleHypothesis );

  /* ----------------------------------------
     Component reduction
     ---------------------------------------- */

  const Trk::MultiComponentState* reducedState = m_merger->merge(*updatedState); 
 
  if ( reducedState && reducedState != updatedState && updatedState != &multiComponentState )
    delete updatedState;

  reducedState = reducedState ? reducedState : updatedState;

  ATH_MSG_VERBOSE(  "Reduced state has  " << reducedState->size() << " components " );



  if (m_outputlevel <= 0){
    msg(MSG::VERBOSE) << "Successfully completed extrapolation to destination: " << endmsg;

    combinedState = m_stateCombiner->combine( *reducedState );

    this->printState( "Material update at destination completed with", *combinedState );
    msg(MSG::DEBUG) << "-----------------------------------------------------------------------------------------------------------" << endmsg;

    delete combinedState;
  }

  return reducedState;

}

/* =========================================================================================================================================
   =========================================================================================================================================

   Extrapolate based on material on active surfaces

   =========================================================================================================================================
   ========================================================================================================================================= */

const Trk::MultiComponentState* Trk::GsfExtrapolator::extrapolateSurfaceBasedMaterialEffects ( const IPropagator& propagator,
                             const MultiComponentState& multiComponentState,
                             const Surface& surface,
                             PropDirection direction,
                             BoundaryCheck boundaryCheck,
                             ParticleHypothesis particleHypothesis ) const
{

  if (m_outputlevel < 0) msg(MSG::VERBOSE) << "GSF extrapolateSurfaceBasedMaterialEffects()" << endmsg;

  /* -------------------------------------
     Preliminary checks
     ------------------------------------- */
  
  // Check the multi-component state is populated
  if ( multiComponentState.empty() ){
    msg(MSG::WARNING) << "Multi component state passed to extrapolateInsideVolume is not populated... returning 0" << endmsg;
    return 0;
  }

  //const Trk::TrackingVolume* currentVolume = m_navigator->highestVolume();

  const Trk::MultiComponentState* lastState = &multiComponentState;
  const Trk::MultiComponentState* nextState = 0;

  // Propagate over all components
  nextState = multiStatePropagate( propagator,
           *lastState,
           surface,
           direction,
           boundaryCheck,
           particleHypothesis );
  
  
  // Clean up memory after the propagation. Reset pointers
  if ( lastState != &multiComponentState && lastState != nextState )
    delete lastState;

  if ( !nextState ){
    if (m_outputlevel<=0) msg(MSG::DEBUG) << "Propagated state is empty... returning 0" << endmsg;
    return 0;
  }

  /* ----------------------------------------
     Material effects
     ---------------------------------------- */
 
  lastState = nextState;
  nextState = 0;

  nextState = m_materialUpdator->simpliedMaterialUpdate( *lastState, direction, particleHypothesis );

  if ( nextState != lastState && lastState != &multiComponentState )
    delete lastState;

  /* ----------------------------------------
     Component reduction
     ---------------------------------------- */

    lastState = nextState;
    nextState = 0;
 
    nextState = m_merger->merge(*lastState);

    if ( lastState != nextState && lastState != &multiComponentState )
      delete lastState;

  return nextState;

}

/* =========================================================================================================================================
   =========================================================================================================================================

   Multi-component state propagate

   =========================================================================================================================================
   ========================================================================================================================================= */

const Trk::MultiComponentState* Trk::GsfExtrapolator::multiStatePropagate ( const IPropagator& propagator,
                      const Trk::MultiComponentState& multiComponentState,
                      const Surface& surface,
                      PropDirection direction,
                      BoundaryCheck boundaryCheck,
                      ParticleHypothesis particleHypothesis ) const
{

  ATH_MSG_DEBUG( "GSF multiStatePropagate() propagating " << multiComponentState.size() << " components\n" <<
                 "\t\t\t\t\t...Propagating to surface [r,z] [" <<  surface.center().perp() << ",\t" << surface.center().z() << ']' );
  const Trk::Layer* layer  = surface.associatedLayer();
  if (layer) ATH_MSG_DEBUG("...associated layer to surface " << layerRZoutput(*layer) );
 
  
  // Start the timer
  //Chrono chrono( &(*m_chronoSvc), "multiStatePropagate()" );

  Trk::MultiComponentState* propagatedState = new Trk::MultiComponentState();

  Trk::MultiComponentState::const_iterator component = multiComponentState.begin();

  double sumw(0); //HACK variable to avoid propagation errors
  for ( ; component != multiComponentState.end(); ++component ){

    const Trk::TrackParameters* currentParameters    = component->first;
    const Trk::TrackParameters* propagatedParameters = 0;

    if ( !currentParameters ){
      ATH_MSG_DEBUG( "Component parameters not defined... component will not be propagated... continuing" );
      continue;
    }

    propagatedParameters = propagator.propagate( *currentParameters, surface, direction, boundaryCheck, m_fieldProperties, particleHypothesis );

    if ( !propagatedParameters ){
      ATH_MSG_DEBUG( "Propagation of component failed... continuing" );
      continue;
    }

    sumw+=component->second;
    // Propagation does not affect the weightings of the states
    const Trk::ComponentParameters propagatedComponent( propagatedParameters, component->second );
    propagatedState->push_back( propagatedComponent );

  }

  ATH_MSG_DEBUG( "GSF multiStatePropagate() propagated  " << propagatedState->size() <<  "components" );

  // Protect against empty propagation
  if ( propagatedState->empty() || sumw<0.1 ){
    ATH_MSG_DEBUG( "multiStatePropagate failed... returning 0" );
    delete propagatedState;
    return 0;
  }

  return propagatedState;

}

/* =========================================================================================================================================
   =========================================================================================================================================

   PropagatorType

   =========================================================================================================================================
   ========================================================================================================================================= */

unsigned int Trk::GsfExtrapolator::propagatorType ( const Trk::TrackingVolume& trackingVolume ) const
{
  if (m_propagatorStickyConfiguration) {
    if (m_propagators.size()>m_propagatorConfigurationLevel) return m_propagatorConfigurationLevel;
    ATH_MSG_WARNING( "Misconfigured propagator type, set to "<<m_propagatorConfigurationLevel<<"->0" );
    return 0;
  }

  // Determine what sort of magnetic field is present
  unsigned int magneticFieldMode = m_fieldProperties.magneticFieldMode();

  // Chose between runge-kutta and step propagators depending on field magnetic field and material properties
  // ST : the following check may fail as the dEdX is often dummy for dense volumes - switch to rho or zOverAtimesRho ?
  unsigned int propagatorMode    = ( magneticFieldMode > 1 && fabs( trackingVolume.dEdX ) < 10e-2 ) ? 2 : 3;

  unsigned int returnType =  (propagatorMode > m_propagatorConfigurationLevel) ? m_propagatorConfigurationLevel : propagatorMode;

  if ( m_propagators.size()> returnType ) return returnType;
  ATH_MSG_WARNING( "Misconfigured propagator type, set to "<< returnType <<"->0" );
  return 0;     
}

/* =========================================================================================================================================
   =========================================================================================================================================

   Initialise Navigation

   =========================================================================================================================================
   ========================================================================================================================================= */

void Trk::GsfExtrapolator::initialiseNavigation ( const Trk::IPropagator& propagator, 
              const Trk::MultiComponentState& multiComponentState,
              const Trk::Surface& surface,
              const Trk::Layer*& currentLayer,
              const Trk::TrackingVolume*& currentVolume,
              const Trk::TrackingVolume*& destinationVolume,
              const Trk::TrackParameters*& referenceParameters,
              Trk::PropDirection direction ) const
{

  // Start the timer
  //Chrono chrono( &(*m_chronoSvc), "initialiseNavigation()" );
  ATH_MSG_DEBUG("initialiseNavigation !!!");
  // Empty the garbage bin
  emptyGarbageBins();
  ATH_MSG_DEBUG( "Destination to surface [r,z] [" <<  surface.center().perp() << ",\t" << surface.center().z() << ']' );


  const Trk::TrackParameters* combinedState  = multiComponentState.begin()->first; //m_stateCombiner->combine( multiComponentState );
    
  /* =============================================
     Look for current volume
     ============================================= */

  // 1. See if the current layer is associated with a tracking volume
  
  const Trk::Surface* associatedSurface = &(combinedState->associatedSurface());
  
  currentLayer  = associatedSurface ? associatedSurface->associatedLayer() : currentLayer;
  currentVolume = currentLayer      ? currentLayer->enclosingTrackingVolume() : currentVolume;

  // If the association method failed then try the recall method

  if ( !currentVolume && associatedSurface == m_recallSurface ){
    currentVolume = m_recallTrackingVolume;
    currentLayer  = m_recallLayer;
  }

  // Global search method if this fails

  else if ( !currentVolume ) {
    // If the recall method fails then the cashed information needs to be reset
    resetRecallInformation();
    currentVolume = m_navigator->volume( combinedState->position() );
    currentLayer  = currentVolume->associatedLayer( combinedState->position() );
  
    // ---------------------------------- ASSOCIATED STATIC VOLUME --------------------------------------
    // this is not necessary for ( association & recall )
//    const Trk::TrackingVolume* lowestStaticVol = 
//                  m_navigator->trackingGeometry()->lowestStaticTrackingVolume(combinedState->position()); 
  
//    if (lowestStaticVol && lowestStaticVol != currentVolume ) currentVolume = lowestStaticVol;
    // --------------------------------------------------------------------------------------------------
  
  }

  /* =============================================
     Determine the resolved direction
     ============================================= */
  if ( currentVolume && direction == Trk::anyDirection ){


  }
  
  
  if ( direction == Trk::anyDirection ){

    if (!currentVolume) ATH_MSG_DEBUG( "Initliasation Current No Volume WTF" );
    else ATH_MSG_DEBUG( "Any direction initliation WTF" );
    
    referenceParameters = currentVolume ? propagator.propagateParameters( *combinedState, surface, direction, false, m_fieldProperties ) : 0;

    // These parameters will need to be deleted later. Add to list of garbage to be collected
    throwIntoGarbageBin( referenceParameters );

    if ( referenceParameters ){
      Amg::Vector3D surfaceDirection( referenceParameters->position() - combinedState->position() );
      direction = ( surfaceDirection.dot( combinedState->momentum() ) > 0. ) ? Trk::alongMomentum : Trk::oppositeMomentum;
    }

  }

  /* =============================================
     Look for destination volume
     ============================================= */

  // 1. See if the destination layer is associated with a tracking volume
  destinationVolume = surface.associatedLayer() ? surface.associatedLayer()->enclosingTrackingVolume() : 0;
  if (!surface.associatedLayer())
    ATH_MSG_DEBUG( "No layer associated to the surface" );
  
  

  // 2. See if there is a cashed recall surface
  if ( !destinationVolume && &surface == m_recallSurface ){
    destinationVolume = m_recallTrackingVolume;
    // If no reference parameters are defined, then determine them
    if ( !referenceParameters ){

      referenceParameters = currentVolume ? propagator.propagateParameters( *combinedState, surface, direction, false, m_fieldProperties ) : 0;

      // These parameters will need to be deleted later. Add to list of garbage to be collected
      throwIntoGarbageBin( referenceParameters );
    }
    
  // 3. Global search
  } else {

    // If no reference parameters are defined, then determine them
    if ( !referenceParameters ){

      referenceParameters = currentVolume ? propagator.propagateParameters( *combinedState, surface, direction, false, m_fieldProperties) : 0;

      // These parameters will need to be deleted later. Add to list of garbage to be collected
      throwIntoGarbageBin( referenceParameters );
    }

    // Global search of tracking geometry to find the destination volume
    if ( referenceParameters ){
      destinationVolume = m_navigator->volume( referenceParameters->position() );
    }  
    // If destination volume is still not found then global search based on surface position
    else { 
       destinationVolume = m_navigator->volume( surface.globalReferencePoint() );
    }
  } 
  
  return;

}

/* =========================================================================================================================================
   =========================================================================================================================================

   PrintState

   =========================================================================================================================================
   ========================================================================================================================================= */

void Trk::GsfExtrapolator::printState( const std::string& description, const Trk::TrackParameters& trackParameters ) const
{

  std::string label = description + " (r,phi,z), (phi, theta, q/p):\t(";

  if (m_outputlevel<=0) msg(MSG::DEBUG) << label
    << trackParameters.position().perp() << ", " << trackParameters.position().phi() << ", " << trackParameters.position().z() << "), ("
    << trackParameters.parameters()[Trk::phi] << ", " << trackParameters.parameters()[Trk::theta] << ", "  << trackParameters.parameters()[Trk::qOverP] << ")" << endmsg;
    
  return;

}




/* =========================================================================================================================================
   =========================================================================================================================================

   Extrapolate M

   =========================================================================================================================================
   ========================================================================================================================================= */

const std::vector<const Trk::TrackStateOnSurface*>* Trk::GsfExtrapolator::extrapolateM( const Trk::MultiComponentState& mcsparameters,
                                                                                    const Surface& sf,
                                                                                    PropDirection dir,
                                                                                    BoundaryCheck bcheck,
                                                                                    ParticleHypothesis particle) const
{
  ATH_MSG_DEBUG( "C extrapolateM()" );
  // create a new vector for the material to be collected  



  //const Trk::TrackParameters* combinedState  = mcsparameters.begin()->first; //m_stateCombiner->combine( multiComponentState );
    
  /* =============================================
     Look for current volume
     ============================================= */


  //const Amg::Vector3D destinationPosition = combinedState->position();

  m_matstates = new std::vector<const Trk::TrackStateOnSurface*>;
  // collect the material
  const MultiComponentState* parameterAtDestination = extrapolate(mcsparameters,sf,dir,bcheck,particle);
  // there are no parameters 
  if (!parameterAtDestination ){
    ATH_MSG_VERBOSE( "  [!] Destination surface for extrapolateM has not been hit (required through configuration). Return 0");
    // loop over and clean up
    std::vector<const Trk::TrackStateOnSurface*>::iterator tsosIter    = m_matstates->begin();
    std::vector<const Trk::TrackStateOnSurface*>::iterator tsosIterEnd = m_matstates->end();
    for ( ; tsosIter != tsosIterEnd; ++tsosIter)  delete (*tsosIter);
    delete m_matstates; m_matstates = 0;
    // bail out
    emptyGarbageBins();
    return 0;
  } 
  else {
    ATH_MSG_VERBOSE( "  [+] Adding the destination surface to the TSOS vector in extrapolateM() ");
    m_matstates->push_back(new TrackStateOnSurface(0,parameterAtDestination->begin()->first->clone(),0,0));
    delete parameterAtDestination;
  }

    // assign the temporary states
  std::vector<const Trk::TrackStateOnSurface*> *tmpMatStates = m_matstates;
  m_matstates=0;
  emptyGarbageBins();
  // return the material states
  return tmpMatStates;  
} 

/* =========================================================================================================================================
   =========================================================================================================================================

   Extrapolte M

   =========================================================================================================================================
   ========================================================================================================================================= */
/*const std::vector<const Trk::TrackParameters*>* Trk::GsfExtrapolator::extrapolateM(const TrackParameters& ,
                                                                                const Surface& ,
                                                                                PropDirection ,
                                                                                BoundaryCheck ,
                                                                                std::vector<MaterialEffectsOnTrack>& ,
                                                                                std::vector<Trk::TransportJacobian*>& ,
                                                                                ParticleHypothesis ) const
{
   ATH_MSG_DEBUG( "C-[" << ++m_methodSequence << "] extrapolateM(..) with jacobian collection - Not implemented yet." );
   return 0;
}*/


/* =========================================================================================================================================
   =========================================================================================================================================

   addMaterialtoVector

   =========================================================================================================================================
   ========================================================================================================================================= */

void Trk::GsfExtrapolator::addMaterialtoVector(const Trk::Layer* nextLayer,
                                               const Trk::TrackParameters* nextPar,
                                               PropDirection dir,
                                               ParticleHypothesis particle) const
                                                 
                                                 
{
  ATH_MSG_DEBUG( "GSF inside addMaterialVector " );
  
  if (!m_matstates || !nextLayer || !nextPar) return;
 
 
     
  // Extract the material properties from the layer
  const Trk::MaterialProperties* materialProperties(0);
  double pathcorr(0.);


  // Get the surface associated with the parameters
  const Trk::Surface* surface = &(nextPar->associatedSurface());

  // Only utilise the reference material if an associated detector element exists
  if ( surface && surface->associatedDetectorElement() ){

    // Get the layer material properties
    const Trk::LayerMaterialProperties* layerMaterial = nextLayer->layerMaterialProperties();

    // Assign the material properties
    materialProperties = layerMaterial ? layerMaterial->fullMaterial( nextPar->position() ) : 0;

    // Determine the pathCorrection if the material properties exist
    pathcorr = materialProperties ? 1. / fabs( surface->normal().dot( nextPar->momentum().unit() ) ) : 0.;

  }


  // Check that the material properties have been defined - if not define them from the layer information
  materialProperties = materialProperties ? materialProperties : nextLayer->fullUpdateMaterialProperties( *nextPar );

    

  if(!materialProperties){
    ATH_MSG_DEBUG( "addMaterialVector:   layer has no MaterialProperties!! " );  
    return;
  }

    
  //std::cout << "0PL, PC, X0  :" << thick  << '\t' << pathcorr<< '\t' <<thick / lx0 <<'\t'<< materialProperties->thickness() <<std::endl;

  
  if (m_matstates) {
    pathcorr = pathcorr > 0. ? pathcorr : nextLayer->surfaceRepresentation().pathCorrection(nextPar->position(),nextPar->momentum());
    double thick = pathcorr * materialProperties->thickness();                 
    double dInX0 = thick / materialProperties->x0();
    double absP=1/fabs(nextPar->parameters()[Trk::qOverP]);
    double scatsigma=sqrt(m_msupdators->sigmaSquare(*materialProperties,absP,pathcorr,particle));
    Trk::ScatteringAngles *newsa=new Trk::ScatteringAngles(0,0,scatsigma/sin(nextPar->parameters()[Trk::theta]),scatsigma);
    //energy loss
    Trk::EnergyLoss* eloss = m_elossupdators->energyLoss(*materialProperties,absP,pathcorr,
                                                 dir,particle,Trk::addNoise);
    
    
    // use curvilinear TPs to simplify retrieval by fitters
    Trk::CurvilinearParameters* cvlTP = new Trk::CurvilinearParameters(nextPar->position(),nextPar->momentum(),nextPar->charge());
    Trk::MaterialEffectsOnTrack* mefot =  new Trk::MaterialEffectsOnTrack(dInX0,newsa,eloss,cvlTP->associatedSurface());
    m_matstates->push_back(new TrackStateOnSurface(0,cvlTP,0,mefot));
    ATH_MSG_DEBUG( "addMaterialVector: from layer:"<< layerRZoutput(*nextLayer) <<". Size is now " << m_matstates->size()  );  
    
  }
  
}  
 
 
std::string Trk::GsfExtrapolator::layerRZoutput(const Trk::Layer& lay) const
{
  std::stringstream outStream;
  outStream <<  "[r,z] = [ " << lay.surfaceRepresentation().bounds().r() 
            << ", " << lay.surfaceRepresentation().center().z() << " ] - Index ";
  outStream << lay.layerIndex().value();                
  return outStream.str();
}

std::string Trk::GsfExtrapolator::positionOutput(const Amg::Vector3D& pos) const
{
  std::stringstream outStream;
  outStream <<  "[r,phi,z] = [ " << pos.perp() << ", " << pos.phi() << ", " << pos.z() << " ]";
  return outStream.str();
}



int Trk::GsfExtrapolator::radialDirection(const Trk::MultiComponentState& pars, PropDirection dir) const
{ 
   // safe inbound/outbound estimation
  double prePositionR = pars.begin()->first->position().perp();
  return (prePositionR > (pars.begin()->first->position() + dir*0.5*prePositionR*pars.begin()->first->momentum().unit()).perp()) ? -1 : 1;
}

bool Trk::GsfExtrapolator::radialDirectionCheck(const IPropagator& prop,
                                             const MultiComponentState& startParm,
                                             const MultiComponentState& parsOnLayer,
                                             const TrackingVolume& tvol,
                                             PropDirection dir,
                                             ParticleHypothesis particle) const
{
   const Amg::Vector3D& startPosition = startParm.begin()->first->position();
  const Amg::Vector3D& onLayerPosition = parsOnLayer.begin()->first->position();
  
  // the 3D distance to the layer intersection
  double distToLayer = (startPosition-onLayerPosition).mag();
  // get the innermost contained surface for crosscheck
  const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > >& boundarySurfaces = tvol.boundarySurfaces();
  // only for tubes the crossing makes sense to check for validity
  if (boundarySurfaces.size()==4) {
    // propagate to the inside surface and compare the distance:
    // it can be either the next layer from the initial point, or the inner tube boundary surface
    const Trk::Surface& insideSurface = (boundarySurfaces[Trk::tubeInnerCover].getPtr())->surfaceRepresentation();                  
    const Trk::TrackParameters* parsOnInsideSurface = prop.propagateParameters(*(startParm.begin()->first),
                                                                                insideSurface,dir,true,m_fieldProperties,particle);
    double distToInsideSurface = parsOnInsideSurface ? (startPosition-(parsOnInsideSurface->position())).mag() : 10e10;
    
    ATH_MSG_DEBUG( "  Radial direction check start - at " << positionOutput(startPosition) );
    ATH_MSG_DEBUG( "  Radial direction check layer - at " << positionOutput(onLayerPosition) );
    if (parsOnInsideSurface)
      ATH_MSG_DEBUG( "  Radial direction check inner - at " << positionOutput(parsOnInsideSurface->position()) );
    
    // memory cleanup (no garbage bin, this is faster)
    delete parsOnInsideSurface;
    ATH_MSG_DEBUG( "  Check radial direction: distance layer / boundary = " << distToLayer << " / " << distToInsideSurface);            
    // the intersection with the original layer is valid if it is before the inside surface
    return (distToLayer < distToInsideSurface) ? true : false;
  }    
  return true;
}    

 
