/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************************
			LargestWeightsMultiStateMerger.cxx  -  description
			--------------------------------------------------
begin                : Wednesday 23rd February 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Implementation code for LargestWeightsMultiStateMerger class
***********************************************************************************/

#include "TrkGaussianSumFilter/LargestWeightsMultiStateMerger.h"
#include "TrkGaussianSumFilter/IMultiComponentStateAssembler.h"
#include "TrkGaussianSumFilter/IMultiComponentStateCombiner.h"
#include "TrkGaussianSumFilter/SortingClasses.h"

Trk::LargestWeightsMultiStateMerger::LargestWeightsMultiStateMerger(const std::string& type, const std::string& name, const IInterface* parent)
  :
  AthAlgTool(type, name, parent),
  m_outputlevel(0),
  m_maximumNumberOfComponents(5),
  m_doSmallComponentMerging(true),
  m_stateCombiner("Trk::MultiComponentStateCombiner/LargestWeightsStateCombiner"),
  m_stateAssembler("Trk::MultiComponentStateAssembler/LargestWeightsStateAssembler")
{
  declareInterface<IMultiComponentStateMerger>(this);
  declareProperty("MaximumNumberOfComponents", m_maximumNumberOfComponents);
}

Trk::LargestWeightsMultiStateMerger::~LargestWeightsMultiStateMerger()
{}

StatusCode Trk::LargestWeightsMultiStateMerger::initialize()
{

  m_outputlevel = msg().level()-MSG::DEBUG;   // save the threshold for debug printout in private member

  // Retrieve an instance of the multi-component state combiner

  if ( m_stateCombiner.retrieve().isFailure() ){
    msg(MSG::FATAL) << "Could not retrieve an instance of the multi-component state combiner... Exiting!" << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve an instance of the multi-component state assembler

  if ( m_stateAssembler.retrieve().isFailure() ){
    msg(MSG::FATAL) << "Could not retrieve an instance of the multi-component state assembler... Exiting!" << endmsg;
    return StatusCode::FAILURE;
  }

  msg(MSG::INFO) << "Initialisation of " << type() << " under instance " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;

}

StatusCode Trk::LargestWeightsMultiStateMerger::finalize()
{

  msg(MSG::INFO) << "Finalisation of " << type() << " under instance " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;

}

const Trk::MultiComponentState* Trk::LargestWeightsMultiStateMerger::merge(const Trk::MultiComponentState& multiComponentState) const
{

  if (m_outputlevel < 0) 
    msg(MSG::VERBOSE) << "Merging state with " << multiComponentState.size() << " components" << endmsg;

  // Check that the assember is reset
  bool isAssemblerReset = m_stateAssembler->reset();

  if ( !isAssemblerReset ){
    msg(MSG::ERROR) << "Could not reset the state assembler... returning 0" << endmsg;
    return 0;
  }

  if ( multiComponentState.empty() ){
    msg(MSG::ERROR) << "Attempting to merge multi-state with zero components" << endmsg;
    return 0;
  }

  if ( m_maximumNumberOfComponents <= 0 ){
    msg(MSG::ERROR) << "Attempting to merge multi-state into zero components... stop being silly!" << endmsg;
    return 0;
  }

  if ( multiComponentState.size() <= m_maximumNumberOfComponents){
    if (m_outputlevel < 0) 
      msg(MSG::VERBOSE) << "State is already sufficiently small... no component reduction required" << endmsg;
    return multiComponentState.clone();
  }

  Trk::MultiComponentState* unmergedState = const_cast<Trk::MultiComponentState*>( multiComponentState.clone() );

  std::vector<double> weights;
  Trk::MultiComponentState* componentsForCollapse = new Trk::MultiComponentState;

  // Sort all components of unmerged state according to weight
  unmergedState->sort( SortByLargerComponentWeight() );

  unsigned int numberOfComponents = 0;
  Trk::MultiComponentState::const_iterator component = unmergedState->begin();

  for ( ; component != unmergedState->end(); ++component, ++numberOfComponents )
    {
      
      if (numberOfComponents < m_maximumNumberOfComponents){

        // Add component to state being prepared for assembly and check that it is valid
        bool componentAdded = m_stateAssembler->addComponent(*component);
      
        if ( !componentAdded )
          msg(MSG::WARNING) << "Component could not be added to the state in the assembler" << endmsg;

      }

      else{
        Trk::ComponentParameters clonedComponent( component->first->clone(), component->second );
        componentsForCollapse->push_back( clonedComponent );
        if (!m_doSmallComponentMerging) break;
      }
    }

  // New memory assigned here which should be deleted
   const Trk::ComponentParameters* collapsedComponent = m_stateCombiner->combineWithWeight(*componentsForCollapse);

   // Memory clean-up
   delete componentsForCollapse;

   // Add component to state being prepared for assembly and check that it is valid
   bool componentAdded = m_stateAssembler->addComponent(*collapsedComponent);

   // Memory clean-up
   delete collapsedComponent->first;
   delete collapsedComponent;

   if ( !componentAdded )
	  msg(MSG::WARNING) << "Component could not be added to the state in the assembler" << endmsg;

   const Trk::MultiComponentState* assembledState = m_stateAssembler->assembledState(1.);

   delete unmergedState;

   return assembledState;

}
