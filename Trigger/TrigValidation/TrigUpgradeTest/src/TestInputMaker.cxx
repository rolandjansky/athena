/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigUpgradeTest includes
#include "TestInputMaker.h"
#include <xAODTrigger/TrigCompositeAuxContainer.h>
#include <TrigSteeringEvent/TrigRoiDescriptorCollection.h>
#include <DecisionHandling/TrigCompositeUtils.h>
#include <vector>

namespace HLTTest {
  
  TestInputMaker::TestInputMaker( const std::string& name, 
			    ISvcLocator* pSvcLocator )    
    : InputMakerBase( name, pSvcLocator ) {}
   

  TestInputMaker::~TestInputMaker() {}

  StatusCode TestInputMaker::initialize() {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    // specific:
    ATH_MSG_DEBUG("Will produce output reco collections: " << m_recoOutput);
    CHECK( m_recoOutput.initialize() );
    return StatusCode::SUCCESS;
  }

  StatusCode TestInputMaker::finalize() {
    ATH_MSG_INFO( "Finalizing " << name() << "..." );

    return StatusCode::SUCCESS;
  }


  StatusCode TestInputMaker::execute_r( const EventContext& context ) const {  
    ATH_MSG_DEBUG( "Executing " << name() << "..." );
 
    // call base class helper method to read input decisions, loop over them create outputs and connect them, returns with outputHandles filled
    std::vector< SG::WriteHandle<TrigCompositeUtils::DecisionContainer> > outputHandles;
    CHECK (decisionInputToOutput(context, outputHandles));

    // output collection, as a view container so it can be given const features
    auto reco_output = std::make_unique<xAOD::TrigCompositeContainer>();
    auto aux = std::make_unique<xAOD::TrigCompositeAuxContainer>();
    reco_output->setStore( aux.get() );
    std::vector<const FeatureOBJ*> featuresFromDecision; // used to check for duplicate features linked to different inputHandles
    
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
        ATH_MSG_DEBUG( "followed seed link to input "<< inputLink.key() );
        const Decision* inputDecision = *inputLink;
 	auto roiEL = inputDecision->objectLink<TrigRoiDescriptorCollection>( "initialRoI" );
 	CHECK( roiEL.isValid() );
	
 	// retrieve input feature from input decision (TrigComposite), will in this case be a TrigRoiDescriptor
 	auto featureLink = inputDecision->objectLink<FeatureContainer>( m_linkName.value() );
 	if ( not featureLink.isValid() )  {
 	  ATH_MSG_ERROR( " Can not find reference to " + m_linkName.value() + " from the decision" );
 	  return StatusCode::FAILURE;
 	}
        
        // link input reco object to outputDecision
        outputDecision->setObjectLink(m_linkName.value(), featureLink);

 	const FeatureOBJ* feature = *featureLink;
 	ATH_MSG_DEBUG(" Found feature " <<m_linkName.value() );
        
        // merge reco outputs that are linked to the same feature (RoI): this avoids processing the same RoI from TC decisions from different chains

        // avoid adding the same feature multiple times: check if not in container, if not add it
        if ( find(featuresFromDecision.begin(), featuresFromDecision.end(), feature)
             == featuresFromDecision.end() ){
 	  featuresFromDecision.push_back(feature); // just to keep track of which we have used 
 	  // create the "reco" output: this would normally be a copy of the reco input or something derived from it, e.g. detector data inside a RoI. A TrigComposite is used here just for a trivial example.
 	  auto newFeature = new xAOD::TrigComposite;
 	  reco_output->push_back(newFeature); 
          // 
 	  newFeature->setObjectLink("initialRoI", roiEL);
 	  newFeature->setObjectLink(m_linkName.value(), featureLink);
 	  ATH_MSG_DEBUG(" Added " <<m_linkName.value() << " and initialRoI " << " to reco object");
 	}
        // For early tests, create TC, link to RoiD, push back onto TCC.
        // Later will output RoID collection directly via tool.        
      } // loop over decisions      
    } // loop over input keys

    
    // Finally, record output
    ATH_MSG_DEBUG("Produced "<<reco_output->size() <<" reco objects");
    auto reco_outputHandle = SG::makeHandle(m_recoOutput, context);
    CHECK( reco_outputHandle.record(std::move(reco_output), std::move(aux)) );

    // call base class helper method to print some debug messages summarising the content of the outputHandles.
    CHECK( debugPrintOut(context, outputHandles) );

    return StatusCode::SUCCESS;
  }

 
} //> end namespace HLTTest
