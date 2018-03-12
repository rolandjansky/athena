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
    : AthAlgorithm( name, pSvcLocator ) {}
   

  TestInputMaker::~TestInputMaker() {}

  StatusCode TestInputMaker::initialize() {
    ATH_MSG_INFO ("Initializing " << name() << "...");
    CHECK( m_inputs.initialize() );
    renounceArray(m_inputs);

    ATH_MSG_DEBUG("Will consume implicit input data:" );
    for (auto& input: m_inputs){  
      ATH_MSG_DEBUG(" "<<input.key());
    }

    CHECK( m_decisionsKey.initialize() );
    //   renounceArray(m_decisionsKey);
    ATH_MSG_DEBUG(" and produce decisions: ");
    for (auto& output: m_decisionsKey){  
      ATH_MSG_DEBUG(" "<<output.key());
    }

    // specific:
    ATH_MSG_DEBUG(" with reco collections: " << m_recoOutput);
    CHECK( m_recoOutput.initialize() );
    return StatusCode::SUCCESS;
  }

  StatusCode TestInputMaker::finalize() {
    ATH_MSG_INFO( "Finalizing " << name() << "..." );

    return StatusCode::SUCCESS;
  }

  StatusCode TestInputMaker::execute() {
    ATH_MSG_DEBUG( "Executing " << name() << "..." );

 
    auto outputHandles = m_decisionsKey.makeHandles();

    // output collection, as a view container so it can be given const features
    auto reco_output = std::make_unique<xAOD::TrigCompositeContainer>();
    auto aux = std::make_unique<xAOD::TrigCompositeAuxContainer>();
    reco_output->setStore( aux.get() );

    size_t validInput=0;
    for ( auto inputKey: m_inputs ) {
      auto inputHandle = SG::makeHandle( inputKey );
      ATH_MSG_DEBUG(" "<<inputKey.key()<<(inputHandle.isValid()? "valid": "not valid" ) );
      if (inputHandle.isValid()) validInput++;
    }
    ATH_MSG_DEBUG( "number of implicit ReadHandles is " << m_inputs.size() <<", "<< validInput<<" are valid" );
    
    // Loop over all input containers, which are of course TrigComposites, and request their features    
    std::vector<const FeatureOBJ*> featureFromDecision;
    size_t count_reco=0;
    size_t outputIndex = 0;
    for ( auto inputKey: m_inputs ) {
      auto inputHandle = SG::makeHandle( inputKey );
      if( not inputHandle.isValid() ) {
	ATH_MSG_DEBUG( "Got no decisions from input "<< inputKey.key());
	outputIndex++;
	continue;
      }
      if( inputHandle->size() == 0){ // input filtered out
	ATH_MSG_ERROR( "Got 0 decisions from valid input "<< inputKey.key()<<". Is it expected?");
	return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG( "Got input "<< inputKey.key()<<" with " << inputHandle->size() << " elements" );
       // crete the output container
       auto Outdecisions = std::make_unique<DecisionContainer>();
       auto dec_aux = std::make_unique<DecisionAuxContainer>();
       Outdecisions->setStore( dec_aux.get() );
       
      // loop over decisions retrieved from this input
      size_t counter =0;
      for ( auto decision : *inputHandle){
	auto roiEL = decision->objectLink<TrigRoiDescriptorCollection>( "initialRoI" );
	CHECK( roiEL.isValid() );
	
	// retrieve feature  from TrigComposite, will in this case be a TrigRoiDescriptor
	auto featurelink = (decision)->objectLink<FeatureContainer>( m_linkName.value() );
	if ( not featurelink.isValid() )  {
	  ATH_MSG_ERROR( " Can not find reference to " + m_linkName.value() + " from the decision" );
	  return StatusCode::FAILURE;
	}
        
	const FeatureOBJ* feature = *featurelink;
	ATH_MSG_DEBUG(" Found feature " <<m_linkName.value() );

       // merge reco outputs that are linked to the same feature (RoI): this avoids processing the same RoI from TC decisions from different chains
	bool already_exist=false;
	size_t pos=distance(featureFromDecision.begin(), find(featureFromDecision.begin(), featureFromDecision.end(), feature));
	if (pos < featureFromDecision.size()){
	  already_exist=true;	 
       }

	if (! already_exist) {
	  // create the "reco" output
	  auto tc = new xAOD::TrigComposite;
	  reco_output->push_back(tc);	  
	  // copy all features to a single output collection 
	  //  tc->setObjectLink(m_linkName.value(), featurelink);
	  tc->setObjectLink("initialRoI", roiEL);
	  ATH_MSG_DEBUG(" Added " <<m_linkName.value() <<" to reco object");
	  featureFromDecision.push_back( feature);
	  count_reco++;
	}
	
	// create new decision for each input	
	TrigCompositeUtils::Decision* newd;
	newd = newDecisionIn( Outdecisions.get() );
	//newd->setObjectLink( m_linkName.value(), featurelink);
	newd->setObjectLink( "initialRoI", roiEL );
	newd->setObjectLink( "previousDecisions", ElementLink<DecisionContainer>(inputKey.key(), counter) );// link to previous decision object

	{
          //copy decisions ID
          TrigCompositeUtils::DecisionIDContainer objDecisions;      
          TrigCompositeUtils::decisionIDs( decision, objDecisions );
          for ( const HLT::Identifier& id: objDecisions ){
	    TrigCompositeUtils::addDecisionID( id, newd );
	  }
        }
	counter++;	
        // For early tests, create TC, link to RoiD, push back onto TCC.
        // Later will output RoID collection directly via tool.        
      } // loop over decisions
        
      ATH_MSG_DEBUG( "Recording output key " <<  m_decisionsKey[ outputIndex ].key() <<" of size "<<Outdecisions->size()  <<" at index "<< outputIndex);
      CHECK( outputHandles[outputIndex].record( std::move( Outdecisions ), std::move( dec_aux ) ) );
      outputIndex++;	       
    } // loop over input keys

    
      // Finally, record output
    ATH_MSG_DEBUG("Produced "<<reco_output->size() <<" reco objects");
    auto reco_outputHandle = SG::makeHandle(m_recoOutput);
    CHECK( reco_outputHandle.record(std::move(reco_output), std::move(aux)) );


    //debug
    size_t validOutput=0;
    for ( auto outHandle: outputHandles ) {
      if( not outHandle.isValid() ) continue;
      validOutput++;
    }
    ATH_MSG_DEBUG("Produced "<<validOutput<<" decisions containers");
    if(validInput != validOutput) {
      ATH_MSG_ERROR("Found "<<validInput<<" inputs and "<<validOutput<<" outputs");
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

} //> end namespace HLTTest
