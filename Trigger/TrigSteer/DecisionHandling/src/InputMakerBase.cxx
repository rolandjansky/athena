/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "DecisionHandling/InputMakerBase.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

using namespace TrigCompositeUtils;


InputMakerBase::InputMakerBase( const std::string& name, ISvcLocator* pSvcLocator )
: ::AthReentrantAlgorithm( name, pSvcLocator ) {}

InputMakerBase::~InputMakerBase() {}

const SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer>& InputMakerBase::decisionInputs() const{
  return m_inputs;
}

const SG::WriteHandleKeyArray<TrigCompositeUtils::DecisionContainer>& InputMakerBase::decisionOutputs() const{
  return m_outputs;
}

StatusCode InputMakerBase::sysInitialize() {
  CHECK( AthReentrantAlgorithm::sysInitialize() ); // initialise base class
  ATH_MSG_DEBUG("mergeOutputs is "<<m_mergeOutputs);
  CHECK( m_inputs.initialize() );
  renounceArray(m_inputs); // make inputs implicit, i.e. not required by scheduler
  ATH_MSG_DEBUG("Will consume implicit decisions:" );
  for (auto& input: m_inputs){  
    ATH_MSG_DEBUG( " "<<input.key() );
  }
  CHECK( m_outputs.initialize() );
  ATH_MSG_DEBUG(" and produce decisions: ");
  for (auto& output: m_outputs){  
    ATH_MSG_DEBUG( " "<<output.key() );
  }
  return StatusCode::SUCCESS;
}


// For each input Decision in the input container, create an output Decision in the corresponding output container and link them.
// If the input is invalid or empty, the output is not created, resulting as invalid
StatusCode InputMakerBase::decisionInputToOutput(const EventContext& context, std::vector< SG::WriteHandle<TrigCompositeUtils::DecisionContainer> > & outputHandles) const{

  if (!m_mergeOutputs)   ATH_MSG_DEBUG("Creating one output per input");
  else                   ATH_MSG_DEBUG("Creating one merged output per RoI");

  outputHandles = decisionOutputs().makeHandles(context);
  //size_t tot_inputs = countInputHandles( context );
  size_t outputIndex = 0;
  for ( auto inputKey: decisionInputs() ) {
    auto inputHandle = SG::makeHandle( inputKey, context );

    if( not inputHandle.isValid() ) {
      ATH_MSG_DEBUG( "Got no decisions from input "<< inputKey.key() << " because handle not valid");
      outputIndex++;
      continue;
    }
    if( inputHandle->size() == 0){ // input filtered out
      ATH_MSG_WARNING( "Got no decisions from input "<< inputKey.key()<<": handle is valid but container is empty. Is this expected?");
      outputIndex++;
      continue;
    }
    ATH_MSG_DEBUG( "Running on input "<< inputKey.key()<<" with " << inputHandle->size() << " elements" );
    
    // We have an input container with >= 1 Decision objects. Create an output container with the same index. 
    TrigCompositeUtils::createAndStore(outputHandles[outputIndex]);


    auto outDecisions = outputHandles[outputIndex].ptr();

    // If using m_mergeOutputs, then collate all RoIs that are stored in this input container
    ElementLinkVector<TrigRoiDescriptorCollection> RoIsFromDecision;

    // loop over decisions retrieved from this input
    size_t input_counter =0;
    size_t output_counter =0;
    for (const TrigCompositeUtils::Decision* inputDecision : *inputHandle){
      ATH_MSG_DEBUG( "Input Decision "<<input_counter <<" has " <<TrigCompositeUtils::getLinkToPrevious(inputDecision).size()<<" previous links");
      TrigCompositeUtils::Decision* newDec = nullptr;
      bool addDecision = false;
      ElementLink<TrigRoiDescriptorCollection> roiEL = ElementLink<TrigRoiDescriptorCollection>();
      
      if (m_mergeOutputs){
        // Get RoI from this input decision 
        const auto roiELInfo = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( inputDecision,  m_roisLink.value());
        CHECK( roiELInfo.isValid() );      
        roiEL = roiELInfo.link;
        const auto roiIt = std::find(RoIsFromDecision.begin(), RoIsFromDecision.end(), roiEL);
        addDecision = (roiIt == RoIsFromDecision.end());
        // addDecision is positive here if we have *not* before encountered this RoI within this input collection.
        if (addDecision) {
          RoIsFromDecision.push_back(roiEL); // To keep track of which we have used
          ATH_MSG_DEBUG( "Found RoI:" << **roiEL <<" FS="<< (*roiEL)->isFullscan());
        }
      } else { // Not merging, keep a 1-to-1 mapping of Decisions in Input & Output containers
        addDecision=true;
      }

      if ( addDecision ){
        // Create a new Decision in the Output container to mirror one in the Input container
        newDec = TrigCompositeUtils::newDecisionIn( outDecisions );
        output_counter++;
      } else{
        // addDecision can only be false if m_mergeOutputs was true and roiEL was found within RoIsFromDecision
        // Re-use a Decision already added to the Output container 
        const auto roiIt = std::find(RoIsFromDecision.begin(), RoIsFromDecision.end(), roiEL);
        if (roiIt == RoIsFromDecision.end()) {
          ATH_MSG_ERROR("Logic error in decisionInputToOutput. Cannot find existing RoI.");
          return StatusCode::FAILURE;
        }
        const size_t roiCounter = std::distance( RoIsFromDecision.begin(), roiIt );
        newDec = outDecisions[outputIndex][roiCounter];
      }

      TrigCompositeUtils::linkToPrevious( newDec, inputDecision ); // Link inputDecision object as the 'seed' of newDec
      TrigCompositeUtils::insertDecisionIDs( inputDecision, newDec ); // Copy decision IDs from inputDecision into newDec
      ATH_MSG_DEBUG("New decision has "<< (TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( newDec,  m_roisLink.value())).isValid() <<" valid "<<m_roisLink.value() <<" and "<< TrigCompositeUtils::getLinkToPrevious(newDec).size() <<" previous decisions");     
      input_counter++;  
    } // loop over input decisions

    ATH_MSG_DEBUG( "Filled output key " <<  decisionOutputs()[ outputIndex ].key() <<" of size "<<outDecisions->size()  <<" at index "<< outputIndex);
    for (auto i : *outDecisions) msg() << i << " ";
    msg() << endmsg;
    outputIndex++;         
  } // end of first loop over input keys

  return StatusCode::SUCCESS;
}



StatusCode InputMakerBase::debugPrintOut(const EventContext& context, const std::vector< SG::WriteHandle<TrigCompositeUtils::DecisionContainer> >& outputHandles) const{
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
  ATH_MSG_DEBUG( "number of implicit ReadHandles for input decisions is " << decisionInputs().size() << ", " << validInput << " are valid/notempty" );
  
  size_t validOutput=0;
  for ( auto outHandle: outputHandles ) {
    if( not outHandle.isValid() ) continue;
    validOutput++;
  }
  ATH_MSG_DEBUG("Produced " << validOutput << " valid/notempty output decisions containers");
  if(validInput != validOutput) {
    ATH_MSG_ERROR("Found valid/notempty: " << validInput << " inputs and " << validOutput << " outputs");
  }
  
  for ( auto outHandle: outputHandles ) {
    if( not outHandle.isValid() ) continue;
    ATH_MSG_DEBUG(outHandle.key() <<" with "<< outHandle->size() <<" decisions:");
    for (auto outdecision:  *outHandle){
      TrigCompositeUtils::DecisionIDContainer objDecisions;      
      TrigCompositeUtils::decisionIDs( outdecision, objDecisions );    
      ATH_MSG_DEBUG("Number of positive decisions for this output: " << objDecisions.size() );
      for ( TrigCompositeUtils::DecisionID id : objDecisions ) {
        ATH_MSG_DEBUG( " ---  decision " << HLT::Identifier( id ) );
      }  
    }
  }
  return StatusCode::SUCCESS;
}



size_t InputMakerBase::countInputHandles( const EventContext& context ) const {
  size_t validInputCount=0;
  for ( auto &inputKey: decisionInputs() ) {
    auto inputHandle = SG::makeHandle( inputKey, context );
    ATH_MSG_DEBUG(" "<<inputKey.key()<<(inputHandle.isValid()? " is valid": " is not valid" ) );
    if (inputHandle.isValid()) validInputCount++;
  }
  ATH_MSG_DEBUG( "number of implicit ReadHandles is " << decisionInputs().size() <<", "<< validInputCount<<" are valid" );
  return validInputCount;
}




