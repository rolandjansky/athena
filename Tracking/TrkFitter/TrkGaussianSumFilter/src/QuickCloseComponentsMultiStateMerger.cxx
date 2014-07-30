/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      QuickCloseComponentsMultiStateMerger.cxx  -  description
      ---------------------------------------------------
begin                : Wednesday 3rd September 2008
author               : amorley
email                : Anthony.Morley@cern.ch
decription           : Implementation code for QuickCloseComponentsMultiStateMerger
                       class
*********************************************************************************/

#include "TrkGaussianSumFilter/QuickCloseComponentsMultiStateMerger.h"
#include "TrkGaussianSumFilter/IComponentSeparationDistance.h"
#include "TrkGaussianSumFilter/IMultiComponentStateAssembler.h"
#include "TrkGaussianSumFilter/IMultiComponentStateCombiner.h"

#include "TrkParameters/TrackParameters.h"

#include "GaudiKernel/Chrono.h"

#include <map>

Trk::QuickCloseComponentsMultiStateMerger::QuickCloseComponentsMultiStateMerger(
  const std::string& type, const std::string& name, const IInterface* parent)
  :
  AthAlgTool(type, name, parent),
  m_maximumNumberOfComponents(5),
  m_distance("Trk::KullbackLeiblerComponentDistance/KullbackLeiblerComponentDistance"),
  m_stateCombiner("Trk::MultiComponentStateCombiner/CloseComponentsCombiner"),
  m_stateAssembler("Trk::MultiComponentStateAssembler/CloseComponentsStateAssembler"),
  m_chronoSvc("ChronoStatSvc", name)
{

  declareInterface<IMultiComponentStateMerger>(this);

  declareProperty("CombinerTool",m_stateCombiner);
  declareProperty("MaximumNumberOfComponents", m_maximumNumberOfComponents);
  declareProperty("DistanceType", m_distance);

}

Trk::QuickCloseComponentsMultiStateMerger::~QuickCloseComponentsMultiStateMerger()
{}

StatusCode Trk::QuickCloseComponentsMultiStateMerger::initialize()
{
    if (msgLvl(MSG::VERBOSE)) msg() << "Initialising Close Components Merger" << endreq;

  // Request the Chrono Service
  if ( m_chronoSvc.retrieve().isFailure() ) {
   msg(MSG::FATAL) << "Failed to retrieve service " << m_chronoSvc << endreq;
   return StatusCode::FAILURE;
  } else 
   msg(MSG::INFO) << "Retrieved service " << m_chronoSvc << endreq;

  // Retrieve the distance tool
  if ( m_distance.retrieve().isFailure() ){
    msg(MSG::FATAL)
        << "Could not retrieve component distance AlgTool ... Exiting!" << endreq;
    return StatusCode::FAILURE;
  }

  // Request an instance of the MultiComponentStateCombiner
  if ( m_stateCombiner.retrieve().isFailure() ){
    msg(MSG::FATAL) << "Could not retrieve an instance of the multi-component state combiner... Exiting!" << endreq;
    return StatusCode::FAILURE;
  }
  
  // Request an instance of the MultiComponentStateAssembler
  if ( m_stateAssembler.retrieve().isFailure() ){
    msg(MSG::FATAL) << "Could not retrieve an instance of the mutli-component state assembler... Exiting!" << endreq;
    return StatusCode::FAILURE;
  }

  if ( m_maximumNumberOfComponents <= 0 ){
    msg(MSG::FATAL) << "Attempting to merge multi-state into zero components... stop being silly!" << endreq;
    return 0;
  }

  msg(MSG::INFO) << "Initialisation of " << type() << " under instance " << name() << " was successful" << endreq;

  return StatusCode::SUCCESS;

}

StatusCode Trk::QuickCloseComponentsMultiStateMerger::finalize()
{

  msg(MSG::INFO) << "Finalisation of " << type() << " under instance " << name() << " was successful" << endreq;

  return StatusCode::SUCCESS;

}

const Trk::MultiComponentState* Trk::QuickCloseComponentsMultiStateMerger::merge(const Trk::MultiComponentState& unmergedState) const
{

  // Start the timer
  Chrono chrono( &(*m_chronoSvc), "CloseComponentMerger" );

  if (msgLvl(MSG::VERBOSE)) msg() << "Merging state with " << unmergedState.size() << " components" << endreq;

  unsigned int numberOfComponents = unmergedState.size();

  // Check that the assember is reset
  bool isAssemblerReset = m_stateAssembler->reset();

  if (unmergedState.size() <= m_maximumNumberOfComponents){
    if (msgLvl(MSG::VERBOSE)) msg() << "State is already sufficiently small... no component reduction required" << endreq;
    return unmergedState.clone();
  }

  if ( !isAssemblerReset ){
    msg(MSG::ERROR) << "Could not reset the state assembler... returning 0" << endreq;
    return 0;
  }

  if ( unmergedState.empty() ){
    msg(MSG::ERROR) << "Attempting to merge multi-state with zero components" << endreq;
    return 0;
  }

  bool componentWithoutMeasurement = false;

  Trk::MultiComponentState::const_iterator component = unmergedState.begin();

  // Scan all components for covariance matricies. If one or more component is missing an error matrix, component reduction is impossible.
  for ( ; component != unmergedState.end(); ++component){

    const AmgSymMatrix(5)* measuredCov = component->first->covariance();

    if ( !measuredCov )
      componentWithoutMeasurement = true;
  }


  
  
  const Trk::TrackParameters* combinedState = unmergedState.begin()->first->clone(); 
 
  //m_stateCombiner->useMode(true);
  //const Trk::TrackParameters* combinedState = m_stateCombiner->combine( unmergedState );
  //m_stateCombiner->useMode(false);


  const Trk::ComponentParameters reducedState( combinedState, 1. );


  if ( componentWithoutMeasurement ){
    if (msgLvl(MSG::DEBUG)) msg() << "A track parameters object is without measurement... reducing state to single component" << endreq;

    return ( new Trk::MultiComponentState( reducedState ) );
  }


  // These should be sorted by key....
  SortBySmallerWeight comparator;
  MultiComponentStateMap unmergedComponentsMap(comparator);
  
  // Clone unmerged state as STL methods deletes components
  Trk::MultiComponentState* clonedUnmergedState = const_cast<Trk::MultiComponentState*>( unmergedState.clone() );

  component = clonedUnmergedState->begin();

  for ( ; component != clonedUnmergedState->end(); ++component ){

    // Map components of multi-component state list into multi-map
    double  distance  = calculateDistance( reducedState, *component );
   
    if (distance == 0)      
      if (msgLvl(MSG::DEBUG)) msg() << "Distance  == 0 hmmm interesting" << endreq;

    unmergedComponentsMap.insert( std::make_pair(distance, *component) );
  }


  if (msgLvl(MSG::VERBOSE)) msg() << "  Start Reducing Components   " << endreq;


  while (numberOfComponents > m_maximumNumberOfComponents){

    // Reset the merged components multi-map for next iteration
      
    double minDistance(9.9e12);
    double key1(-99999999),key2(-99999999);
    MultiComponentStateMap::iterator mapComponent = unmergedComponentsMap.begin();
    MultiComponentStateMap::iterator mapComponentEnd = unmergedComponentsMap.end();
    --mapComponentEnd;
    MultiComponentStateMap::iterator tempIter;
   
    MultiComponentStateMap::iterator iterKey1,iterKey2;
    
    for ( ; mapComponent != mapComponentEnd; ++mapComponent){
      tempIter = mapComponent;
      ++tempIter;
    
      double distance  =  tempIter->first - mapComponent->first ;
      //std::cout << "1st " << mapComponent->first <<" 2nd " << tempIter->first <<" = "<< distance << std::endl;
   
      if ( distance < minDistance )
      {
        key1 = mapComponent->first;
        key2 = tempIter->first;
        iterKey1 = mapComponent;
        iterKey2 = tempIter;
        minDistance = distance;
      }
    }
    
    if ( key1 != -99999999 && key2 != -99999999 && iterKey1 != iterKey2 ){
    
      Trk::MultiComponentState* componentsToBeMerged = new Trk::MultiComponentState();
   
      if (msgLvl(MSG::VERBOSE)) msg() << "key1 " << key1 << " key2 " << key2 << endreq;
   
      componentsToBeMerged->push_back( iterKey1->second );
      componentsToBeMerged->push_back( iterKey2->second );
      
      // Combine the closest distance components
      const Trk::ComponentParameters* combinedComponents = m_stateCombiner->combineWithWeight( *componentsToBeMerged );
      
      if (msgLvl(MSG::VERBOSE)) msg() << "Weight of new component"<< combinedComponents->second << endreq;
            
      // Erase these components from the unmerged components map. These need to be deleted also as new memory is assigned in the combiner
      delete componentsToBeMerged;
      
      unmergedComponentsMap.erase( iterKey1 );
      unmergedComponentsMap.erase( iterKey2 );
   
      double  distance  = calculateDistance( reducedState, *combinedComponents );
    
      if (distance == 0)      
        if (msgLvl(MSG::DEBUG)) msg() << "Distance  == 0 hmmm interesting" << endreq;

      unmergedComponentsMap.insert( std::make_pair(distance, *combinedComponents) );
    
      // Delete memory allocated to component parameters pair. This does not delete the inserted copy
      delete combinedComponents;
  
      // Decrement the number of components
      --numberOfComponents;
  
    } else {
      
      if (msgLvl(MSG::DEBUG)) msg() << " Keys are either equal or totally nonsensical " << endreq;
   
      // Something has gone amiss...... what shall i do
      clonedUnmergedState->clear();
      delete clonedUnmergedState;
      
      return ( new Trk::MultiComponentState( reducedState ) );     
      
    }
    
    if (msgLvl(MSG::VERBOSE)) msg() << "Number of components: " << numberOfComponents << endreq;
  
  }

  // std::cout << "   Finished Reducing Components   " <<std::endl;
  delete combinedState;

  // Build final state containing both merged and unmerged components

  MultiComponentStateMap::const_iterator mapComponent = unmergedComponentsMap.begin();

  for ( ; mapComponent != unmergedComponentsMap.end(); ++mapComponent){

    // Add component to state being prepared for assembly and check that it is valid
    bool componentAdded = m_stateAssembler->addComponent(mapComponent->second);

    if ( !componentAdded )
      msg(MSG::WARNING) << "Component could not be added to the state in the assembler" << endreq;

    // Free up memory from track parameters in unmerged components map
    delete mapComponent->second.first;

  }
  
  const Trk::MultiComponentState* mergedState = m_stateAssembler->assembledState();

  if (msgLvl(MSG::VERBOSE)) msg() << "Number of components in merged state: " << mergedState->size() << endreq;

  // Memory clean up
  clonedUnmergedState->clear();
  delete clonedUnmergedState;

  return mergedState;

}


double Trk::QuickCloseComponentsMultiStateMerger::calculateDistance(const Trk::ComponentParameters referenceParameters , const Trk::ComponentParameters variableParameters  ) const
{
  
  double distance(0);
  
  distance = (*m_distance)( referenceParameters, variableParameters);
   
  return distance; 
}




