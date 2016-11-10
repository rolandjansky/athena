/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      CloseComponentsMultiStateMerger.cxx  -  description
      ---------------------------------------------------
begin                : Wednesday 23rd February 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Implementation code for CloseComponentsMultiStateMerger
                       class
*********************************************************************************/

#include "TrkGaussianSumFilter/CloseComponentsMultiStateMerger.h"
#include "TrkGaussianSumFilter/IComponentSeparationDistance.h"
#include "TrkGaussianSumFilter/IMultiComponentStateAssembler.h"
#include "TrkGaussianSumFilter/IMultiComponentStateCombiner.h"

#include "TrkParameters/TrackParameters.h"

Trk::CloseComponentsMultiStateMerger::CloseComponentsMultiStateMerger(
  const std::string& type, const std::string& name, const IInterface* parent)
  :
  AthAlgTool(type, name, parent),
  m_maximumNumberOfComponents(5),
  m_distance("Trk::KullbackLeiblerComponentDistance/KullbackLeiblerComponentDistance"),
  m_stateCombiner("Trk::MultiComponentStateCombiner/CloseComponentsCombiner"),
  m_stateAssembler("Trk::MultiComponentStateAssembler/CloseComponentsStateAssembler"),
  m_chronoSvc( "ChronoStatSvc", name )
{
  
  declareInterface<IMultiComponentStateMerger>(this);

  declareProperty("MaximumNumberOfComponents", m_maximumNumberOfComponents);
  declareProperty("DistanceType", m_distance);
  
}

Trk::CloseComponentsMultiStateMerger::~CloseComponentsMultiStateMerger()
{}

StatusCode Trk::CloseComponentsMultiStateMerger::initialize()
{

  msg(MSG::VERBOSE) << "Initialising Close Components Merger" << endmsg;

  // Request the Chrono Service
  if ( m_chronoSvc.retrieve().isFailure() ) {
   msg(MSG::FATAL) << "Failed to retrieve service " << m_chronoSvc << endmsg;
   return StatusCode::FAILURE;
	} else 
   msg(MSG::INFO) << "Retrieved service " << m_chronoSvc << endmsg;

  // Retrieve the distance tool
  if ( m_distance.retrieve().isFailure() ){
    msg(MSG::FATAL)
  			<< "Could not retrieve component distance AlgTool ... Exiting!" << endmsg;
    return StatusCode::FAILURE;
  }

  // Request an instance of the MultiComponentStateCombiner
  if ( m_stateCombiner.retrieve().isFailure() ){
    msg(MSG::FATAL) << "Could not retrieve an instance of the multi-component state combiner... Exiting!" << endmsg;
    return StatusCode::FAILURE;
  }
  
  m_stateCombiner->useMode(false);

  // Request an instance of the MultiComponentStateAssembler
  if ( m_stateAssembler.retrieve().isFailure() ){
    msg(MSG::FATAL) << "Could not retrieve an instance of the mutli-component state assembler... Exiting!" << endmsg;
    return StatusCode::FAILURE;
  }

  msg(MSG::INFO) << "Initialisation of " << type() << " under instance " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;

}

StatusCode Trk::CloseComponentsMultiStateMerger::finalize()
{

  msg(MSG::INFO) << "Finalisation of " << type() << " under instance " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;

}

const Trk::MultiComponentState* Trk::CloseComponentsMultiStateMerger::merge(const Trk::MultiComponentState& unmergedState) const
{

  // Start the timer
  m_chronoSvc->chronoStart("MS::scan");

 
  if (msgLvl(MSG::VERBOSE)) msg() << "Merging state with " << unmergedState.size() << " components" << endmsg;

  bool componentWithoutMeasurement = false;

  Trk::MultiComponentState::const_iterator component = unmergedState.begin();

  // Scan all components for covariance matricies. If one or more component is missing an error matrix, component reduction is impossible.
  for ( ; component != unmergedState.end(); ++component){

    const AmgSymMatrix(5)* measuredCov = component->first->covariance();

    if ( !measuredCov )
      componentWithoutMeasurement = true;
  }

  if ( componentWithoutMeasurement ){
    if (msgLvl(MSG::DEBUG)) msg() << "A track parameters object is without measurement... reducing state to single component" << endmsg;

    const Trk::TrackParameters* combinedState = m_stateCombiner->combine( unmergedState );

    const Trk::ComponentParameters reducedState( combinedState, 1. );

    return ( new Trk::MultiComponentState( reducedState ) );


  }

  unsigned int numberOfComponents = unmergedState.size();

  // Check that the assember is reset
  bool isAssemblerReset = m_stateAssembler->reset();

  if ( !isAssemblerReset ){
    msg(MSG::ERROR) << "Could not reset the state assembler... returning 0" << endmsg;
    return 0;
  }

  if ( unmergedState.empty() ){
    msg(MSG::ERROR) << "Attempting to merge multi-state with zero components" << endmsg;
    return 0;
  }

  if ( m_maximumNumberOfComponents <= 0 ){
    msg(MSG::ERROR) << "Attempting to merge multi-state into zero components... stop being silly!" << endmsg;
    return 0;
  }

  if (unmergedState.size() <= m_maximumNumberOfComponents){
    if (msgLvl(MSG::VERBOSE)) msg() << "State is already sufficiently small... no component reduction required" << endmsg;
    return unmergedState.clone();
  }

  SortByLargerWeight comparator;
  MultiComponentStateMap unmergedComponentsMap(comparator);
  MultiComponentStateMap mergedComponentsMap(comparator);

  // Clone unmerged state as STL methods deletes components

  Trk::MultiComponentState* clonedUnmergedState = const_cast<Trk::MultiComponentState*>( unmergedState.clone() );

  component = clonedUnmergedState->begin();

  for ( ; component != clonedUnmergedState->end(); ++component ){

    // Map components of multi-component state list into multi-map
    unmergedComponentsMap.insert( std::make_pair(component->second, *component) );
  }

  while (numberOfComponents > m_maximumNumberOfComponents){

    // Reset the merged components multi-map for next iteration
    
    if ( !mergedComponentsMap.empty() )
      mergedComponentsMap.clear();

    while ( numberOfComponents > m_maximumNumberOfComponents && !unmergedComponentsMap.empty() ){

      if (unmergedComponentsMap.size() > 1){

  const std::pair<const Trk::ComponentParameters, MultiComponentStateMap::iterator>* minimumDistancePair = pairWithMinimumDistance(unmergedComponentsMap);

  if( minimumDistancePair ){

    Trk::MultiComponentState* componentsToBeMerged = new Trk::MultiComponentState();

    // Push back the two components closest in distance
    componentsToBeMerged->push_back(unmergedComponentsMap.begin()->second);
    componentsToBeMerged->push_back(minimumDistancePair->first);

    // Combine the closest distance components
    const Trk::ComponentParameters* combinedComponents = m_stateCombiner->combineWithWeight( *componentsToBeMerged );
  
    // Erase these components from the unmerged components map. These need to be deleted also as new memory is assigned in the combiner
    delete componentsToBeMerged;

    unmergedComponentsMap.erase( unmergedComponentsMap.begin() );
    unmergedComponentsMap.erase( minimumDistancePair->second );

    // Insert this merged component into the merged components multi-map
    mergedComponentsMap.insert( std::make_pair(combinedComponents->second, *combinedComponents) );
    
    // Try deleting the minimum distance pair
    delete minimumDistancePair;

    // Delete memory allocated to component parameters pair. This does not delete the inserted copy
    delete combinedComponents;

  }

  else{

    if (msgLvl(MSG::DEBUG)) msg() << "Component will be removed from state because it could not be reduced" << endmsg;

    delete (unmergedComponentsMap.begin()->second).first;

    unmergedComponentsMap.erase( unmergedComponentsMap.begin() );

  }

  // Decrement the number of components
    --numberOfComponents;
    if (msgLvl(MSG::VERBOSE)) msg() << "Number of components: " << numberOfComponents << endmsg;

      } // end if number of unmerged components > 1 clause

      else {

  // Case if only one component exists in the unmerged state

  // Copy single component in the unmerged state into the merged state.
  mergedComponentsMap.insert( std::make_pair(unmergedComponentsMap.begin()->first, unmergedComponentsMap.begin()->second) );

  // Delete the entry in the unmerged component map. Do not delete as no new memory is assigned
  unmergedComponentsMap.erase( unmergedComponentsMap.begin() );

      }

    } // end while loop requiring excess of components and unmerged component map not empty

    /* Once all of the components have been merged once, ie there are no components left in the unmerged container
       if further mergers are required because the maximum number of components allowed is still exceeded, a second
       iteration of component mergers are initiated by setting the merged component map to the new unmerged map */

    if (unmergedComponentsMap.empty() && numberOfComponents > m_maximumNumberOfComponents)
      unmergedComponentsMap = mergedComponentsMap;

  } // end while loop over maximum number of components

  // Build final state containing both merged and unmerged components

  MultiComponentStateMap::const_iterator mapComponent = unmergedComponentsMap.begin();

  for ( ; mapComponent != unmergedComponentsMap.end(); ++mapComponent){

    // Add component to state being prepared for assembly and check that it is valid
    bool componentAdded = m_stateAssembler->addComponent(mapComponent->second);

    if ( !componentAdded )
      msg(MSG::WARNING) << "Component could not be added to the state in the assembler" << endmsg;

    // Free up memory from track parameters in unmerged components map
    delete mapComponent->second.first;

  }

  mapComponent = mergedComponentsMap.begin();

  for ( ; mapComponent != mergedComponentsMap.end(); ++mapComponent){

    // Add component to state being prepared for assembly and check that it is valid
    bool componentAdded = m_stateAssembler->addComponent(mapComponent->second);

    if ( !componentAdded )
      msg(MSG::WARNING) << "Component could not be added to the state in the assembler" << endmsg;

    // Free up memory from track parameters in unmerged components map
    delete mapComponent->second.first;

  }

  const Trk::MultiComponentState* mergedState = m_stateAssembler->assembledState();

  if (msgLvl(MSG::VERBOSE)) msg() << "Number of components in merged state: " << mergedState->size() << endmsg;

  // Memory clean up
  clonedUnmergedState->clear();
  delete clonedUnmergedState;

  m_chronoSvc->chronoStop("MS::scan");
  return mergedState;

}

const std::pair<const Trk::ComponentParameters, Trk::CloseComponentsMultiStateMerger::MultiComponentStateMap::iterator>*
Trk::CloseComponentsMultiStateMerger::pairWithMinimumDistance(Trk::CloseComponentsMultiStateMerger::MultiComponentStateMap& unmergedComponentsMap) const
{

  // msg(MSG::VERBOSE) << "Calculating pair with minimum distance from state of size: " << unmergedComponentsMap.size() << endmsg;

  double minimumDistance = 10e10;

  bool minimumDistanceSet = false;
  MultiComponentStateMap::iterator minimumDistanceMarker(0);
  MultiComponentStateMap::iterator component = unmergedComponentsMap.begin();

  for ( ; component != unmergedComponentsMap.end(); ++component){

    // First component of the unmerged components map is the reference and distances of all other components are determined with respect to this component
    if ( component != unmergedComponentsMap.begin() ){

      double distance = (*m_distance)(unmergedComponentsMap.begin()->second, component->second);

      if (distance < minimumDistance){
        // msg(MSG::VERBOSE) << "New minimum distance set" << endmsg;
        minimumDistance = distance;
        minimumDistanceMarker = component;
        minimumDistanceSet = true;
      }
    }
  }

  if ( !minimumDistanceSet ){
    if (msgLvl(MSG::DEBUG)) msg() << "Minimum distance cannot be found" << endmsg;
    return 0;
  }

  const Trk::ComponentParameters minimumDistanceComponent = minimumDistanceMarker->second;

  std::pair<const Trk::ComponentParameters, MultiComponentStateMap::iterator>* returnPair
    = new std::pair<const Trk::ComponentParameters, MultiComponentStateMap::iterator>(minimumDistanceComponent, minimumDistanceMarker);

  return returnPair;

}
