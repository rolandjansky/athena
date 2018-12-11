/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "DecisionHandling/InputMakerBase.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

const std::vector<std::string> baseLinks = {"initialRoI", "roi", "view", "feature"};

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
StatusCode InputMakerBase::decisionInputToOutput(const EventContext& context, std::vector< SG::WriteHandle<TrigCompositeUtils::DecisionContainer> > & outputHandles) const{
  if (m_mergeOutputs) return decisionInputToMergedOutput(context, outputHandles);

  
  ATH_MSG_DEBUG("Creating one output per input");
  outputHandles = decisionOutputs().makeHandles(context);
  countInputHandles( context );
 
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
    ATH_MSG_DEBUG( "Got input "<< inputKey.key()<<" with " << inputHandle->size() << " elements" );
    // create the output container
    auto outDecisions = std::make_unique<TrigCompositeUtils::DecisionContainer>();
    auto outDecAux    = std::make_unique<TrigCompositeUtils::DecisionAuxContainer>();
    outDecisions->setStore( outDecAux.get() );
       
    // loop over decisions retrieved from this input
    size_t input_counter =0;
    for ( auto decision : *inputHandle){
      // create new decision for each input	
      TrigCompositeUtils::Decision*  newDec = TrigCompositeUtils::newDecisionIn( outDecisions.get() );
      TrigCompositeUtils::linkToPrevious( newDec, inputKey.key(), input_counter );
      TrigCompositeUtils::insertDecisionIDs( decision, newDec );

      //      copyBaseLinks( decision, newDec);
      ATH_MSG_DEBUG("New decision has "<< newDec->hasObjectLink(m_roisLink.value() ) <<" " <<m_roisLink.value() );     
      input_counter++;	
    } // loop over decisions

    ATH_MSG_DEBUG( "Recording output key " <<  decisionOutputs()[ outputIndex ].key() <<" of size "<<outDecisions->size()  <<" at index "<< outputIndex);
    CHECK( outputHandles[outputIndex].record( std::move( outDecisions ), std::move( outDecAux ) ) );
    outputIndex++;	       
  } // end of first loop over input keys

  return StatusCode::SUCCESS;
}



// For each input Decision in the input container that links to one RoI, create an output Decision per RoI (mergeing them into one) and linking to all previous
StatusCode InputMakerBase::decisionInputToMergedOutput(const EventContext& context, std::vector< SG::WriteHandle<TrigCompositeUtils::DecisionContainer> > & outputHandles) const{

  ATH_MSG_DEBUG("Creating one merged output per RoI");
  outputHandles = decisionOutputs().makeHandles(context);
  countInputHandles( context );


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
    ATH_MSG_DEBUG( "Got input "<< inputKey.key()<<" with " << inputHandle->size() << " elements" );
    // create the output container
    auto outDecisions = std::make_unique<TrigCompositeUtils::DecisionContainer>();
    auto outDecAux    = std::make_unique<TrigCompositeUtils::DecisionAuxContainer>();
    outDecisions->setStore( outDecAux.get() );

    //map all RoIs that are stored in this input container
    std::vector <ElementLink<TrigRoiDescriptorCollection> > RoIsFromDecision;
       
    // loop over decisions retrieved from this input
    size_t input_counter =0;
    size_t output_counter =0;
    for ( auto decision : *inputHandle){
      ATH_MSG_DEBUG( "Input Decision "<<input_counter <<" has " <<TrigCompositeUtils::getLinkToPrevious(decision).size()<<" previous links");
      auto roiELInfo = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( decision,  m_roisLink.value());
      CHECK( roiELInfo.isValid() );

      
      auto roiEL = roiELInfo.link;//decision->objectLink<TrigRoiDescriptorCollection>(m_roisLink.value() );
      auto roiIt=find(RoIsFromDecision.begin(), RoIsFromDecision.end(), roiEL);
      TrigCompositeUtils::Decision*  newDec;
      if ( roiIt == RoIsFromDecision.end() ){
	RoIsFromDecision.push_back(roiEL); // just to keep track of which we have used 
	const TrigRoiDescriptor* roi = *roiEL;
	ATH_MSG_DEBUG( "Found RoI:" <<*roi<<" FS="<<roi->isFullscan());
	ATH_MSG_DEBUG( "Making new decision" );
	newDec = TrigCompositeUtils::newDecisionIn( outDecisions.get() );
	output_counter++;
      }
      else{
	int iroi = roiIt-RoIsFromDecision.begin();
	newDec = outDecisions.get()[outputIndex][iroi];
      }
      
      TrigCompositeUtils::linkToPrevious( newDec, inputKey.key(), input_counter );
      TrigCompositeUtils::insertDecisionIDs( decision, newDec );     	
      //copyBaseLinks( decision, newDec);
      ATH_MSG_DEBUG("New decision has "<< (TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( newDec,  m_roisLink.value())).isValid() <<" valid "<<m_roisLink.value() <<" and "<< TrigCompositeUtils::getLinkToPrevious(newDec).size() <<" previous decisions");     
      input_counter++;	
    } // loop over decisions

    ATH_MSG_DEBUG( "Recording output key " <<  decisionOutputs()[ outputIndex ].key() <<" of size "<<outDecisions->size()  <<" at index "<< outputIndex);
    CHECK( outputHandles[outputIndex].record( std::move( outDecisions ), std::move( outDecAux ) ) );
    outputIndex++;	       
  } // end of first loop over input keys

  return StatusCode::SUCCESS;
}



//StatusCode InputMakerBase::copyBaseLinks(const TrigCompositeUtils::Decision* src, TrigCompositeUtils::Decision* dest) const  {
  // do we need to filter the links to be copied? if not, we can copy all of them
  // for (auto link: baseLinks){
  //   if ( src->hasObjectLink(link ) ) dest->copyLinkFrom(src,link);
  // }
  //    copyLinks(decision, newDec);
  //  if ( src->hasObjectLink("self" ) ) dest->copyLinkFrom(src,"self","seed"); // make use of self-link 
//  return StatusCode::SUCCESS;
//}


StatusCode InputMakerBase::debugPrintOut(const EventContext& context, const std::vector< SG::WriteHandle<TrigCompositeUtils::DecisionContainer> >& outputHandles) const{
  size_t validInput=0;
  for ( auto inputKey: decisionInputs() ) {
    auto inputHandle = SG::makeHandle( inputKey, context );
    ATH_MSG_DEBUG(" " << inputKey.key() << " " << (inputHandle.isValid()? "valid": "not valid" ) );
    if (inputHandle.isValid()) validInput++;
  }
  size_t validOutput=0;
  ATH_MSG_DEBUG( "number of implicit ReadHandles for input decisions is " << decisionInputs().size() << ", " << validInput << " are valid" );
  for ( auto outHandle: outputHandles ) {
    if( not outHandle.isValid() ) continue;
    validOutput++;
  }
  ATH_MSG_DEBUG("Produced " << validOutput << " output decisions containers");
  if(validInput != validOutput) {
    ATH_MSG_ERROR("Found " << validInput << " inputs and " << validOutput << " outputs");
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



  
