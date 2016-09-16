/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

Trk::MultiComponentStateAssembler::MultiComponentStateAssembler (const std::string& type, const std::string& name, const IInterface* parent)
  :
  AthAlgTool(type, name, parent),
  m_outputlevel(0),
  m_assemblyDone(false),
  m_minimumFractionalWeight(1.e-9),
  m_minimumValidFraction(0.01),
  m_validWeightSum(0.),
  m_invalidWeightSum(0.),
  m_multiComponentState( new Trk::MultiComponentState() )
{
  
  declareInterface<IMultiComponentStateAssembler>(this);
  
  declareProperty("minimumFractionalWeight", m_minimumFractionalWeight);
  declareProperty("minimumValidFraction",    m_minimumValidFraction);

}

Trk::MultiComponentStateAssembler::~MultiComponentStateAssembler () 
{}

StatusCode Trk::MultiComponentStateAssembler::initialize(){
  
  m_outputlevel = msg().level()-MSG::DEBUG;   // save the threshold for debug printout in private member
  
  if (m_outputlevel < 0) 
    msg(MSG::VERBOSE) << "Initialisation of " << name() << " successful" << endmsg;
  
  return StatusCode::SUCCESS;
}

StatusCode Trk::MultiComponentStateAssembler::finalize(){
 
  delete m_multiComponentState;
 
  if (m_outputlevel < 0) 
    msg(MSG::VERBOSE) << "Finalisation of " << name() << " successful" << endmsg;
  
  return StatusCode::SUCCESS;
  
}

bool Trk::MultiComponentStateAssembler::reset()
{

  if (m_outputlevel < 0) 
    msg(MSG::VERBOSE) << "Resetting the MultiComponentStateAssembler: " << name() << endmsg;

  m_assemblyDone = false;

  if ( !m_multiComponentState->empty() ){
    
    Trk::MultiComponentState::const_iterator component = m_multiComponentState->begin();

    for ( ; component != m_multiComponentState->end(); ++component )
      delete component->first;
    
    m_multiComponentState->clear();

  }

  m_validWeightSum = 0.;
  m_invalidWeightSum = 0.;
  
  return true;

}

void Trk::MultiComponentStateAssembler::status() const
{

  if (m_outputlevel <= 0) {
    std::cout << "**************** Current status of assembler: " << name() << " ****************" << std::endl;
    std::cout << "Number of components in cached state: " << m_multiComponentState->size() << std::endl;
    std::cout << "Assembly done? (Bool): " << m_assemblyDone << std::endl;
    std::cout << "Total valid weight:    " << m_validWeightSum << std::endl;
    std::cout << "Total invalid weight:  " << m_invalidWeightSum << std::endl;
    std::cout << "**************** End assembler status ****************" << std::endl;
  }

}

bool Trk::MultiComponentStateAssembler::addComponent (const ComponentParameters& componentParameters){
  
  if (m_outputlevel < 0) 
    msg(MSG::VERBOSE) << "Adding single component to mixture" << endmsg;

  if ( m_assemblyDone ){
    msg(MSG::WARNING) << "Trying to add state after assembly... returning false" << endmsg;
    return false;
  }
  
  const Trk::ComponentParameters* clonedComponentParameters = new Trk::ComponentParameters( (componentParameters.first)->clone(), componentParameters.second);

  if (m_outputlevel < 0) 
    msg(MSG::VERBOSE) << "Creating multiple component state from single component. Weight of state is: " << componentParameters.second << endmsg;

  Trk::MultiComponentState* singleComponentList = new Trk::MultiComponentState(*clonedComponentParameters);
  this->addComponentsList(singleComponentList);
  delete clonedComponentParameters;
  singleComponentList->clear();
  delete singleComponentList;

  return true;
}

bool Trk::MultiComponentStateAssembler::addMultiState (const MultiComponentState& multiComponentState){
  
  if (m_outputlevel < 0) 
    msg(MSG::VERBOSE) << "Adding multiple component state to mixture" << endmsg;

  if ( m_assemblyDone ){
    msg(MSG::WARNING) << "Trying to add state after assembly... returning false" << endmsg;
    return false;
  }
  
  Trk::MultiComponentState* clonedMultiComponentState = const_cast<Trk::MultiComponentState*>( multiComponentState.clone() );

  this->addComponentsList(clonedMultiComponentState);
  clonedMultiComponentState->clear();
  delete clonedMultiComponentState;
  return true;
}

bool Trk::MultiComponentStateAssembler::addInvalidComponentWeight (const double& invalidComponentWeight){
  
  if (m_outputlevel < 0) 
    msg(MSG::VERBOSE) << "Adding the weight of an invalid state to the mixture" << endmsg;

  m_invalidWeightSum += invalidComponentWeight;

  return true;

}

void Trk::MultiComponentStateAssembler::addComponentsList (const MultiComponentState* multiComponentState){
  
  if (m_outputlevel < 0) 
    msg(MSG::VERBOSE) << "Add multiple component state to exisiting mixture" << endmsg;

  if ( m_assemblyDone ){
    msg(MSG::WARNING) << "Trying to add state after assembly" << endmsg;
    return;
  }
  
  double sumW(0.);
  Trk::MultiComponentState::const_iterator component = multiComponentState->begin();

  for ( ; component != multiComponentState->end(); ++ component)
    sumW += (*component).second;
  

  m_multiComponentState->insert( m_multiComponentState->end(), multiComponentState->begin(), multiComponentState->end() );

  m_validWeightSum += sumW;

  if (m_outputlevel < 0) 
    msg(MSG::VERBOSE) << "Successfully inserted state" << endmsg;

}

bool Trk::MultiComponentStateAssembler::prepareStateForAssembly (){
  
  if (m_outputlevel < 0) 
    msg(MSG::VERBOSE) << "Preparing state for assembly" << endmsg;

  // Protect against empty state
  if ( !isStateValid () ){
    if (m_outputlevel <= 0) 
      msg(MSG::DEBUG) << "State is not valid... returning false" << endmsg;
    return false;
  }
  
  // Check for minimum fraction of valid states
  double validWeightFraction = m_validWeightSum / ( m_validWeightSum + m_invalidWeightSum );

  if (m_invalidWeightSum > 0. && validWeightFraction < m_minimumValidFraction){
    if (m_outputlevel <= 0) 
      msg(MSG::DEBUG) << "Insufficient valid states in the state... returning false" << endmsg;
    return false;
  }

  // Check to see assembly has not already been done
  if ( m_assemblyDone ){
    if (m_outputlevel < 0) 
      msg(MSG::VERBOSE) << "Assembly of state already complete... returning true" << endmsg;
    return true;
  }

  // Remove components with negligable weights
  removeSmallWeights ();
  
  // Now recheck to make sure the state is now still valid
  if ( !isStateValid () ){
    if (m_outputlevel <= 0) 
      msg(MSG::DEBUG) << "After removal of small weights, state is invalid... returning false" << endmsg;
    return false;
  }
  
  // Sort Multi-Component State by weights
  m_multiComponentState->sort( SortByLargerComponentWeight() );
  
  // Set assembly flag
  m_assemblyDone = true;

  if (m_outputlevel < 0) 
    msg(MSG::VERBOSE) << "State is prepared for assembly... returning true" << endmsg;

  return true;
}

const Trk::MultiComponentState*
Trk::MultiComponentStateAssembler::assembledState () {
  
  if (m_outputlevel < 0) 
    msg(MSG::VERBOSE) << "Finalising assembly... no specified reweighting" << endmsg;

  if ( !prepareStateForAssembly() ) {
    if (m_outputlevel <= 0) 
      msg(MSG::DEBUG) << "Unable to prepare state for assembly... returning 0" << endmsg;
    return 0;
  }

  if (m_outputlevel < 0) 
    msg(MSG::VERBOSE) << "Successful preparation for assembly" << endmsg;
  
  if ( m_invalidWeightSum > 0. || m_validWeightSum <= 0.) {
    if (m_outputlevel < 0) 
      msg(MSG::VERBOSE) << "Assembling state with invalid weight components" << endmsg;
    double totalWeight = m_validWeightSum + m_invalidWeightSum;
    const Trk::MultiComponentState* stateAssembly = doStateAssembly(totalWeight);
    return stateAssembly;
  }
  
  const Trk::MultiComponentState* multiComponentState = m_multiComponentState->clone();

  // Reset state cache before leaving
  this->reset();

  return multiComponentState;
}

const Trk::MultiComponentState*
Trk::MultiComponentStateAssembler::assembledState (const double& newWeight) {
  
  if (m_outputlevel < 0) 
    msg(MSG::VERBOSE) << "Finalising assembly with reweighting of components" << endmsg;

  if ( !prepareStateForAssembly() ) {
    if (m_outputlevel <= 0) 
      msg(MSG::DEBUG) << "Unable to prepare state for assembly... returing 0" << endmsg;
    return 0;
  }
  
  const Trk::MultiComponentState* stateAssembly = doStateAssembly(newWeight);

  return stateAssembly;	
}

const Trk::MultiComponentState*
Trk::MultiComponentStateAssembler::doStateAssembly (const double& newWeight) {
  
  if (m_outputlevel < 0) 
    msg(MSG::VERBOSE) << "Do state assembly" << endmsg;
  
  if ( !isStateValid() ) {
    if (m_outputlevel < 0) 
      msg(MSG::VERBOSE) << "Cached state is empty... returning 0" << endmsg;
    return 0;
  }
  
  
  if (m_validWeightSum <= 0.) {
    if (!m_multiComponentState->empty()) {
      double fixedWeights = 1. / (double) m_multiComponentState->size();
      Trk::MultiComponentState::iterator component = m_multiComponentState->begin();
      for ( ; component != m_multiComponentState->end() ; ++component ){
        component->second = fixedWeights;
      } 
    }
    const Trk::MultiComponentState* assembledState = m_multiComponentState->clone();
    // Reset the cache before leaving
    this->reset();
    return assembledState;
  }

  double scalingFactor = m_validWeightSum > 0. ? newWeight / m_validWeightSum : 1. ;
  const Trk::MultiComponentState* assembledState = m_multiComponentState->cloneWithWeightScaling( scalingFactor );

  // Reset the cashe before leaving
  this->reset();

  return assembledState;
}

void Trk::MultiComponentStateAssembler::removeSmallWeights () {
  
  if (m_outputlevel < 0) 
    msg(MSG::VERBOSE) << "Removing small weights" << endmsg;

  double totalWeight( m_validWeightSum + m_invalidWeightSum );
  
  if ( totalWeight == 0. ) {
    if (m_outputlevel <= 0) 
      msg(MSG::DEBUG) << "Total weight of state is zero... exiting" << endmsg;
    return;
  }
  
  // Loop over states and remove those with insiginificant weights fractions
  bool continueToRemoveComponents;
  
  do {
    continueToRemoveComponents = false;
    MultiComponentState::iterator component;
    for ( component  = m_multiComponentState->begin() ;
	  component != m_multiComponentState->end()   ;
	  ++component ) {
      if ( (*component).second / totalWeight < m_minimumFractionalWeight ) {
        delete component->first;
        m_multiComponentState->erase(component);
        if (m_outputlevel <= 0) 
          msg(MSG::DEBUG) << "State with weight " << (*component).second << " has been removed from mixture" << endmsg;
        continueToRemoveComponents = true;
        break;
      } // end if
    } // end for
  } while ( continueToRemoveComponents );

}
