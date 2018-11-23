/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigUpgradeTest includes
#include "InputMakerForRoI.h"
#include <xAODTrigger/TrigCompositeAuxContainer.h>
#include <TrigSteeringEvent/TrigRoiDescriptorCollection.h>
#include <DecisionHandling/TrigCompositeUtils.h>
#include <vector>



InputMakerForRoI:: InputMakerForRoI( const std::string& name, 
				     ISvcLocator* pSvcLocator )    
  : InputMakerBase( name, pSvcLocator ) {}


InputMakerForRoI::~ InputMakerForRoI() {}

StatusCode  InputMakerForRoI::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  
  // specific:
  ATH_MSG_DEBUG("Will produce output RoI collections: " << m_RoIs);
  CHECK( m_RoIs.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode  InputMakerForRoI::finalize() {
  ATH_MSG_INFO( "Finalizing " << name() << "..." );
  return StatusCode::SUCCESS;
}


StatusCode  InputMakerForRoI::execute_r( const EventContext& context ) const {  
  ATH_MSG_DEBUG( "Executing " << name() << "..." );
  
  // call base class helper method to read input decisions, loop over them create outputs and connect them, returns with outputHandles filled
  std::vector< SG::WriteHandle<TrigCompositeUtils::DecisionContainer> > outputHandles;
  CHECK (decisionInputToOutput(context, outputHandles));
  
  // Prepare Outputs
  std::unique_ptr< TrigRoiDescriptorCollection > oneRoIColl( new TrigRoiDescriptorCollection() );
 
  // use also this:    ElementLinkVector<xAOD::MuonRoIContainer> getMuonRoILinks = obj->objectCollectionLinks<xAOD::MuonRoIContainer>("ManyMuonRoIs");
  std::vector <ElementLink<TrigRoiDescriptorCollection> > RoIsFromDecision;  // used to check for duplicate features linked to different inputHandles

  // loop over output decisions, navigate to inputs
  for (auto outputHandle: outputHandles) {
    if( not outputHandle.isValid() ) {
      ATH_MSG_DEBUG( "Got no decisions from output "<< outputHandle.key() << " because handle not valid");
      continue;
    }
    if( outputHandle->size() == 0){ // input filtered out
      ATH_MSG_ERROR( "Got no decisions from output "<< outputHandle.key()<<": handle is valid but container is empty. Is this expected?");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Got output "<< outputHandle.key()<<" with " << outputHandle->size() << " elements" );
    // loop over output decisions in container of outputHandle, follow link to inputDecision
    for ( auto outputDecision : *outputHandle){ 
      ElementLink<DecisionContainer> inputLink = linkToPrevious(outputDecision);
      const Decision* inputDecision = *inputLink;
      auto roiEL = inputDecision->objectLink<TrigRoiDescriptorCollection>(m_linkName.value() ); //"initialRoI" 
      CHECK( roiEL.isValid() );
      
      // avoid adding the same feature multiple times: check if not in container, if not add it
      if ( find(RoIsFromDecision.begin(), RoIsFromDecision.end(), roiEL)
	   == RoIsFromDecision.end() ){
	RoIsFromDecision.push_back(roiEL); // just to keep track of which we have used 
	const TrigRoiDescriptor* roi = *roiEL;
	ATH_MSG_DEBUG("Found RoI:" <<*roi<<" FS="<<roi->isFullscan());
	//make a new one:
	TrigRoiDescriptor* newroi= new TrigRoiDescriptor(*roi); //use copy constructor
	oneRoIColl->push_back(newroi);
	ATH_MSG_DEBUG("Added RoI:" <<*newroi<<" FS="<<newroi->isFullscan());
      }            
    } // loop over decisions      
  } // loop over input keys
  
  
    // Finally, record output
  ATH_MSG_DEBUG("Produced "<<oneRoIColl->size() <<" output RoIs");
  auto roi_outputHandle = SG::makeHandle(m_RoIs, context);
  CHECK( roi_outputHandle.record(std::move(oneRoIColl)) );
  
  // call base class helper method to print some debug messages summarising the content of the outputHandles.
  CHECK( debugPrintOut(context, outputHandles) );
  
  return StatusCode::SUCCESS;
}



