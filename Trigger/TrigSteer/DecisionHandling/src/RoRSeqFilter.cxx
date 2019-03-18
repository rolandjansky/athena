/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DecisionHandling includes
#include "RoRSeqFilter.h"

#include "GaudiKernel/Property.h"

using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::decisionIDs;
using TrigCompositeUtils::insertDecisionIDs;
using TrigCompositeUtils::createAndStore;
using TrigCompositeUtils::newDecisionIn;

RoRSeqFilter::RoRSeqFilter( const std::string& name, 
  ISvcLocator* pSvcLocator ) :
  ::AthAlgorithm( name, pSvcLocator )
{}

RoRSeqFilter::~RoRSeqFilter()
{}

StatusCode RoRSeqFilter::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." );
  CHECK( not m_inputKeys.empty() );
  CHECK( not m_outputKeys.empty() );

  if ( m_mergeInputs ) {
    CHECK( m_inputKeys.size() > 1 );
    CHECK( m_outputKeys.size() == 1 );    
  } else {
    CHECK( m_inputKeys.size() == m_outputKeys.size() );
  }

  CHECK( m_inputKeys.initialize() );
  CHECK( m_outputKeys.initialize() );

  renounceArray(m_inputKeys);
  // is this needed? renounceArray(m_outputKeys); // TimM - I don't think so

  ATH_MSG_DEBUG("Will consume implicit ReadDH:" );
  for (auto& input: m_inputKeys){  
    ATH_MSG_DEBUG(" - "<<input.key());
  }
  ATH_MSG_DEBUG("Will produce implicit WriteDH: ");
  for (auto& output: m_outputKeys){  
    ATH_MSG_DEBUG(" - "<<output.key());
  }

  CHECK( not m_chainsProperty.empty() );
  
  for ( const std::string& el: m_chainsProperty ) 
    m_chains.insert( HLT::Identifier( el ).numeric() );

  for ( const HLT::Identifier& id: m_chains )
    ATH_MSG_DEBUG( "Configured to require chain " << id );
  
  ATH_MSG_DEBUG( "mergeInputs is " << m_mergeInputs);

  return StatusCode::SUCCESS;
}

StatusCode RoRSeqFilter::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode RoRSeqFilter::execute() {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  auto inputHandles  = m_inputKeys.makeHandles();
  auto outputHandles = m_outputKeys.makeHandles();
  //std::vector<SG::ReadHandle<DecisionContainer>> inputHandles;
  //std::vector<SG::WriteHandle<DecisionContainer>> outputHandles;

  bool validInputs=false;
  for ( auto inputHandle: inputHandles ) {
    if( inputHandle.isValid() ) {// this is because input is implicit
      validInputs = true;
    }
  }
  if (!validInputs) {
    setFilterPassed(false);
    ATH_MSG_DEBUG ( "No valid inputs found, filter failed. Return...." );
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG( "Running on "<< inputHandles.size() <<" input keys");
  
  size_t passCounter = 0;
  size_t outputIndex = 0;
  if ( m_mergeInputs ) {

    ATH_MSG_DEBUG( "Recording " <<  m_outputKeys[ 0 ].key() ); 
    createAndStore(outputHandles[0]);
    DecisionContainer* output = outputHandles[0].ptr();
    for ( auto inputKey: m_inputKeys ) {
      auto inputHandle = SG::makeHandle( inputKey );
      if( inputHandle.isValid() )
        passCounter += copyPassing( *inputHandle,  *output );
    }
    outputIndex++;

  } else { // Not merging inputs

    for ( auto inputKey: m_inputKeys ) {
      // already made handles, so this code could be simplified to a loop over inputHandles.
      auto inputHandle = SG::makeHandle( inputKey );

      if( not inputHandle.isValid() ) {
        ATH_MSG_DEBUG( "InputHandle "<< inputKey.key() <<" not present" );
      } else if ( inputHandle->empty() ) {
        ATH_MSG_DEBUG( "InputHandle "<< inputKey.key() <<" contains all rejected decisions, skipping" );
      } else {
        ATH_MSG_DEBUG( "Checking inputHandle "<< inputKey.key() <<" with " << inputHandle->size() <<" elements");
        createAndStore(outputHandles[outputIndex]);
        DecisionContainer* output = outputHandles[outputIndex].ptr();
        passCounter += copyPassing( *inputHandle, *output );  
        ATH_MSG_DEBUG( "Recorded output key " <<  m_outputKeys[ outputIndex ].key() <<" of size "<<output->size()  <<" at index "<< outputIndex);
      }
      outputIndex++; // Keep the mapping of inputKey<->outputKey correct
    }

  }

  setFilterPassed( passCounter != 0 );
  ATH_MSG_DEBUG( "Filter " << ( filterPassed() ? "passed" : "rejected") <<"; creating "<< outputIndex<<" valid outDecisions DH:");
  for (auto output: outputHandles){
    if( output.isValid() ) ATH_MSG_DEBUG(" "<<output.key());
  }

  
  return StatusCode::SUCCESS;
}
  
size_t RoRSeqFilter::copyPassing( const DecisionContainer& input, 
                                  DecisionContainer& output ) const {
  size_t passCounter = 0;
  ATH_MSG_DEBUG( "Input size " << input.size() );
  for (size_t i = 0; i < input.size(); ++i) {
    const Decision* inputDecision = input.at(i);

    DecisionIDContainer objDecisions;      
    decisionIDs( inputDecision, objDecisions );

    ATH_MSG_DEBUG("Number of positive decisions for this input: " << objDecisions.size() );

    for ( DecisionID id : objDecisions ) {
      ATH_MSG_DEBUG( " -- Positive decision " << HLT::Identifier( id ) );
    }

    DecisionIDContainer intersection;
    std::set_intersection( m_chains.begin(), m_chains.end(),
         objDecisions.begin(), objDecisions.end(),
         std::inserter(intersection, intersection.end()) );

    if ( not intersection.empty() ) {      
      // This sets up the 'self' link & the 'seed' link (seeds from inputDecision)
      Decision* decisionCopy = newDecisionIn( &output, inputDecision, name() );

      // Copy accross only the DecisionIDs which have passed through this Filter for this Decision object. 
      // WARNING: Still need to 100% confirm if the correct set to propagate forward is objDecisions or intersection.
      // Tim M changing this from objDecisions (all IDs) -> intersection (only passed IDs) Feb 19
      insertDecisionIDs(intersection, decisionCopy);

      passCounter ++;
      ATH_MSG_DEBUG("Input satisfied at least one active chain");
    } else {
      ATH_MSG_DEBUG("No Input decisions requested by active chains");
    }
  }
  ATH_MSG_DEBUG( "Output size " << output.size() );
  return passCounter;  
}
