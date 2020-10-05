/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "DecisionHandling/InputMakerBase.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

using namespace TrigCompositeUtils;


InputMakerBase::InputMakerBase( const std::string& name, ISvcLocator* pSvcLocator )
: ::AthReentrantAlgorithm( name, pSvcLocator ) {}

InputMakerBase::~InputMakerBase() {}

const SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer>& InputMakerBase::decisionInputs() const{
  return m_inputs;
}

const SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer>& InputMakerBase::decisionOutputs() const{
  return m_outputs;
}

StatusCode InputMakerBase::sysInitialize() {
  CHECK( AthReentrantAlgorithm::sysInitialize() ); // initialise base class
  CHECK( m_inputs.initialize() );
  renounceArray(m_inputs); // make inputs implicit, i.e. not required by scheduler
  ATH_MSG_DEBUG("Will consume implicit decisions:" );
  for (auto& input: m_inputs){  
    ATH_MSG_DEBUG( " "<<input.key() );
  }
  CHECK( m_outputs.initialize() );
  ATH_MSG_DEBUG(" and produce decisions: " << m_outputs.key());
  return StatusCode::SUCCESS; 
}

// Create one output container and merge all inputs into this, using heuristics to identify equal Decision objects.
// If the input is invalid or empty, the output is not created, resulting as invalid
StatusCode InputMakerBase::decisionInputToOutput(const EventContext& context, SG::WriteHandle<TrigCompositeUtils::DecisionContainer>& outputHandle) const{

  ATH_MSG_DEBUG("Creating one merged output per Decision object");
  ATH_CHECK( outputHandle.isValid() );
  TrigCompositeUtils::DecisionContainer* outDecisions  = outputHandle.ptr();

  for ( auto inputKey: decisionInputs() ) {
    auto inputHandle = SG::makeHandle( inputKey, context );

    if( not inputHandle.isValid() ) {
      ATH_MSG_DEBUG( "Got no decisions from input "<< inputKey.key() << " because implicit handle not valid");
      continue;
    }
    if( inputHandle->size() == 0){
      ATH_MSG_DEBUG( "Got no decisions from input "<< inputKey.key()<<": implicit handle is valid but container is empty.");
      continue;
    }
    ATH_MSG_DEBUG( "Running on input "<< inputKey.key()<<" with " << inputHandle->size() << " elements" );
    

    size_t input_counter = 0;
    for (const TrigCompositeUtils::Decision* inputDecision : *inputHandle){
      ATH_MSG_DEBUG( " -- Input Decision " << input_counter <<": has " <<TrigCompositeUtils::getLinkToPrevious(inputDecision).size()<<" previous links");

      size_t alreadyAddedIndex = std::numeric_limits<std::size_t>::max();
      const bool alreadyAdded = matchInCollection(outDecisions, inputDecision, alreadyAddedIndex);

      TrigCompositeUtils::Decision* newDec = nullptr;
      if (alreadyAdded) { // Already added, at alreadyAddedIndex
        newDec = outDecisions->at( alreadyAddedIndex );
        ATH_MSG_DEBUG( "  -- Matched to existing, " << inputKey.key() << " index " << input_counter << " is merged into existing output index " << alreadyAddedIndex);
      } else { // Not already added, make new
        newDec = TrigCompositeUtils::newDecisionIn( outDecisions );
        newDec->setName("IM");
        ATH_MSG_DEBUG( "  -- Did not match to existing, " << inputKey.key() << " index " << input_counter << " creates output index " << outDecisions->size()-1);
      }

      // Note: We will call linkToPrevious and insertDecisionIDs N times on a single "newDec", where N is the number of inputDecision which checkExisting determines are the same object.
      TrigCompositeUtils::linkToPrevious( newDec, inputDecision, context ); // Link inputDecision object as the 'seed' of newDec
      TrigCompositeUtils::insertDecisionIDs( inputDecision, newDec ); // Copy decision IDs from inputDecision into newDec
      ATH_MSG_DEBUG("      -- This output decision now has " << TrigCompositeUtils::getLinkToPrevious(newDec).size() << " seeds and "<< newDec->decisions().size() << " decisionIds");           
      input_counter++;  
    } // loop over input decisions

  } // end of: for ( auto inputKey: decisionInputs() )

  // Print some debug messages summarising the content of the outputHandles.
  if (msgLvl(MSG::DEBUG)) {
    debugPrintOut(context, outputHandle);
  }

  return StatusCode::SUCCESS;
}


bool InputMakerBase::matchInCollection(const DecisionContainer* toMatchAgainst, const Decision* toMatch, size_t& matchIndex) const {
  if ( m_mergeUsingFeature ) {
    matchIndex = matchDecision<xAOD::IParticleContainer>(toMatchAgainst, toMatch, featureString());
  } else {
    matchIndex = matchDecision<TrigRoiDescriptorCollection>(toMatchAgainst, toMatch, m_roisLink.value());
  }
  return (matchIndex != std::numeric_limits<std::size_t>::max());
}


void InputMakerBase::debugPrintOut(const EventContext& context, SG::WriteHandle<TrigCompositeUtils::DecisionContainer>& outputHandle) const{
  size_t validInput=0;
  for ( auto inputKey: decisionInputs() ) {
    auto inputHandle = SG::makeHandle( inputKey, context );
    ATH_MSG_DEBUG(" " << inputKey.key() << " " << (inputHandle.isValid()? "valid": "not valid" ) );
    if (inputHandle.isValid()) {
      if (inputHandle->size() > 0) {
        validInput++;
      } else {
        ATH_MSG_DEBUG("   " << inputKey.key() << " actually NOT valid due to size() == 0");
      }
    }
  }
  ATH_MSG_DEBUG( "Number of implicit ReadHandles for input decisions is " << decisionInputs().size() << ", " << validInput << " are valid/not-empty" );
  
  ATH_MSG_DEBUG("Output " << outputHandle.key() <<" with "<< outputHandle->size() <<" decisions:");
  for (const auto outdecision : *outputHandle){
    TrigCompositeUtils::DecisionIDContainer objDecisions;      
    TrigCompositeUtils::decisionIDs( outdecision, objDecisions );    
    ATH_MSG_DEBUG("Number of positive decisions for this output: " << objDecisions.size() );
    for ( TrigCompositeUtils::DecisionID id : objDecisions ) {
      ATH_MSG_DEBUG( " ---  decision " << HLT::Identifier( id ) );
    }  
  }
}

