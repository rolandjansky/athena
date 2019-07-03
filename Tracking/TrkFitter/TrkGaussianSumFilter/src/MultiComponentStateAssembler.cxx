/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
			MultiComponentStateAssembler.cxx  -  description
			----------------------------------------------------
begin                : Monday 20th December 2004
author               : atkinson
email                : Tom.Atkinson@cern.ch
description          : Implementation code for MultiComponentStateAssembler
			class.
*********************************************************************************/

#include "TrkGaussianSumFilter/MultiComponentStateAssembler.h"
#include "TrkGaussianSumFilter/SortingClasses.h"
#include "TrkParameters/TrackParameters.h"

Trk::MultiComponentStateAssembler::MultiComponentStateAssembler (const std::string& type, 
                                                                 const std::string& name, 
                                                                 const IInterface* parent)
  : AthAlgTool(type, name, parent)
{ 
  declareInterface<IMultiComponentStateAssembler>(this);
}

Trk::MultiComponentStateAssembler::~MultiComponentStateAssembler () 
{}

StatusCode Trk::MultiComponentStateAssembler::initialize(){
  
  ATH_MSG_DEBUG("Initialisation of " << name() << " successful \n"); 
  return StatusCode::SUCCESS;
}

StatusCode Trk::MultiComponentStateAssembler::finalize(){
 
  ATH_MSG_DEBUG ("Finalisation of " << name() << " successful \n");  
  return StatusCode::SUCCESS;
}

bool Trk::MultiComponentStateAssembler::reset(Cache& cache) const
{

  ATH_MSG_VERBOSE("Resetting the MultiComponentStateAssembler: " << name()<<"\n");
  cache.assemblyDone = false;
  if (cache.multiComponentState.get()!=nullptr && !cache.multiComponentState->empty() ){
    Trk::MultiComponentState::const_iterator component = cache.multiComponentState->begin();
    for ( ; component != cache.multiComponentState->end(); ++component ){
      delete component->first;
    }
    cache.multiComponentState->clear();
  }
  cache.validWeightSum = 0.;
  cache.invalidWeightSum = 0.; 
  return true;
}

void Trk::MultiComponentStateAssembler::status(const Cache& cache) const
{
    ATH_MSG_DEBUG( "**************** Current status of assembler: " << name() << " **************** \n "
                   << "Number of components in cached state: " << cache.multiComponentState->size() << "\n"
                   << "Assembly done? (Bool): " << cache.assemblyDone << "\n"
                   << "Total valid weight:    " << cache.validWeightSum << "\n"
                   << "Total invalid weight:  " << cache.invalidWeightSum << "\n"
                   << "**************** End assembler status **************** \n");
}

bool Trk::MultiComponentStateAssembler::addComponent (Cache&  cache, 
                                                      const ComponentParameters& componentParameters) const{
  
  if ( cache.assemblyDone ){
    ATH_MSG_WARNING("Trying to add state after assembly... returning false \n");
    return false;
  } 
  const Trk::ComponentParameters* clonedComponentParameters = 
    new Trk::ComponentParameters( (componentParameters.first)->clone(), componentParameters.second);

  Trk::MultiComponentState* singleComponentList = new Trk::MultiComponentState(*clonedComponentParameters);
  this->addComponentsList(cache,singleComponentList);
  delete clonedComponentParameters;
  singleComponentList->clear();
  delete singleComponentList;
  return true;
}

bool Trk::MultiComponentStateAssembler::addMultiState (Cache& cache, 
                                                       const MultiComponentState& multiComponentState) const{
  
  if (cache.assemblyDone ){
    ATH_MSG_WARNING("Trying to add state after assembly... returning false \n");
    return false;
  }

  Trk::MultiComponentState* clonedMultiComponentState = multiComponentState.clone();

  this->addComponentsList(cache,clonedMultiComponentState);
  clonedMultiComponentState->clear();
  delete clonedMultiComponentState;
  return true;
}

bool Trk::MultiComponentStateAssembler::addInvalidComponentWeight (Cache& cache, 
                                                                   const double invalidComponentWeight) const {
  
  cache.invalidWeightSum += invalidComponentWeight;
  return true;
}

void Trk::MultiComponentStateAssembler::addComponentsList (Cache& cache, 
                                                           const MultiComponentState* multiComponentState) const{
  
  if ( cache.assemblyDone ){
    ATH_MSG_WARNING( "Trying to add state after assembly \n");
    return;
  }
  double sumW(0.);
  Trk::MultiComponentState::const_iterator component = multiComponentState->begin();
  for ( ; component != multiComponentState->end(); ++ component)
    sumW += (*component).second;
  

  cache.multiComponentState->insert(cache.multiComponentState->end(), 
                                    multiComponentState->begin(), 
                                    multiComponentState->end() );

  cache.validWeightSum += sumW;
}

bool Trk::MultiComponentStateAssembler::prepareStateForAssembly (Cache& cache) const{
  
  // Protect against empty state
  if ( !isStateValid (cache) ){
    ATH_MSG_DEBUG("State is not valid... returning false \n");
    return false;
  }
  
  // Check for minimum fraction of valid states
  double validWeightFraction = cache.validWeightSum / ( cache.validWeightSum + cache.invalidWeightSum );

  if (cache.invalidWeightSum > 0. && validWeightFraction < m_minimumValidFraction){
    ATH_MSG_DEBUG( "Insufficient valid states in the state... returning false \n");
    return false;
  }
  // Check to see assembly has not already been done
  if ( cache.assemblyDone ){
     ATH_MSG_DEBUG("Assembly of state already complete... returning true \n");
    return true;
  }

  // Remove components with negligable weights
  removeSmallWeights (cache);
  
  // Now recheck to make sure the state is now still valid
  if ( !isStateValid (cache) ){
     ATH_MSG_DEBUG("After removal of small weights, state is invalid... returning false \n");
    return false;
  }
  
  // Sort Multi-Component State by weights
  std::sort(cache.multiComponentState->begin(),
            cache.multiComponentState->end(), 
            SortByLargerComponentWeight() );
  
  // Set assembly flag
  cache.assemblyDone = true;

  return true;
}

const Trk::MultiComponentState*
Trk::MultiComponentStateAssembler::assembledState (Cache& cache)  const{
  
  ATH_MSG_VERBOSE( "Finalising assembly... no specified reweighting \n");

  if ( !prepareStateForAssembly(cache) ) {
    ATH_MSG_DEBUG ("Unable to prepare state for assembly... returning 0 \n");
    return 0;
  }

  ATH_MSG_VERBOSE("Successful preparation for assembly \n");
  
  if ( cache.invalidWeightSum > 0. || cache.validWeightSum <= 0.) {
    ATH_MSG_VERBOSE("Assembling state with invalid weight components \n");
    double totalWeight = cache.validWeightSum + cache.invalidWeightSum;
    const Trk::MultiComponentState* stateAssembly = doStateAssembly(cache,totalWeight);
    return stateAssembly;
  }
  
  const Trk::MultiComponentState* multiComponentState = cache.multiComponentState->clone();
  // Reset state cache before leaving
  this->reset(cache);
 
  return multiComponentState;
}

const Trk::MultiComponentState*
Trk::MultiComponentStateAssembler::assembledState (Cache& cache, const double newWeight) const{
  
  ATH_MSG_VERBOSE( "Finalising assembly with reweighting of components \n");

  if ( !prepareStateForAssembly(cache) ) {
     ATH_MSG_DEBUG("Unable to prepare state for assembly... returing nullptr \n");
    return nullptr;
  }
  const Trk::MultiComponentState* stateAssembly = doStateAssembly(cache,newWeight);
  return stateAssembly;	
}

const Trk::MultiComponentState*
Trk::MultiComponentStateAssembler::doStateAssembly (Cache& cache, const double newWeight) const{
  
  if ( !isStateValid(cache) ) {
    ATH_MSG_DEBUG( "Cached state is empty... returning 0 \n");
    return nullptr;
  }
   
  if (cache.validWeightSum <= 0.) {
    if (!cache.multiComponentState->empty()) {
      double fixedWeights = 1. / (double) cache.multiComponentState->size();
      Trk::MultiComponentState::iterator component = cache.multiComponentState->begin();
      for ( ; component != cache.multiComponentState->end() ; ++component ){
        component->second = fixedWeights;
      } 
    }
    const Trk::MultiComponentState* assembledState = cache.multiComponentState->clone();
    // Reset the cache before leaving
    this->reset(cache);
    return assembledState;
  }

  double scalingFactor = cache.validWeightSum > 0. ? newWeight / cache.validWeightSum : 1. ;
  const Trk::MultiComponentState* assembledState = cache.multiComponentState->cloneWithWeightScaling( scalingFactor );
  // Reset the cashe before leaving
  this->reset(cache);

  return assembledState;
}

void Trk::MultiComponentStateAssembler::removeSmallWeights (Cache& cache) const{
  
  double totalWeight( cache.validWeightSum + cache.invalidWeightSum );
  
  if ( totalWeight == 0. ) {
    ATH_MSG_DEBUG("Total weight of state is zero... exiting \n");
    return;
  }

  //Loop over states and remove those with insiginificant weights fractions
    bool continueToRemoveComponents;
     do {
       continueToRemoveComponents = false;
       MultiComponentState::iterator component;
       for ( component  = cache.multiComponentState->begin() ;
        component != cache.multiComponentState->end();/*Nothing*/ ) {
         if ( (*component).second / totalWeight < m_minimumFractionalWeight ) {
           delete component->first;
           /* Get the iterator position after erase*/
           component=cache.multiComponentState->erase(component);
           ATH_MSG_DEBUG("State with weight " << (*component).second << " has been removed from mixture");
           continueToRemoveComponents = true;
           break;
           } // end if
           /* increment the iterator normally*/
           ++component;
       } // end for
     } while ( continueToRemoveComponents );
 }
