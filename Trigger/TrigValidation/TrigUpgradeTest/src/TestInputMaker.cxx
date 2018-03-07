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
    : AthAlgorithm( name, pSvcLocator ),
      m_linkName("feature") {
    declareProperty( "OutputType", m_outputType, "reserved for future use");
    declareProperty( "LinkName", m_linkName, "name of the link to the features in the decision, e.g. 'feature', 'InitialRoI'");
  }

  TestInputMaker::~TestInputMaker() {}

  StatusCode TestInputMaker::initialize() {
    ATH_MSG_INFO ("Initializing " << name() << "...");
    CHECK( m_inputs.initialize() );
    renounceArray(m_inputs);

    ATH_MSG_DEBUG("Will consume implicit input data:" );
    for (auto& input: m_inputs){  
      ATH_MSG_DEBUG(" "<<input.key());
    }
    ATH_MSG_DEBUG(" and produce " << m_recoOutput);

    CHECK( m_recoOutput.initialize() );
    CHECK( m_decisionsKey.initialize() );
    return StatusCode::SUCCESS;
  }

  StatusCode TestInputMaker::finalize() {
    ATH_MSG_INFO( "Finalizing " << name() << "..." );

    return StatusCode::SUCCESS;
  }

  StatusCode TestInputMaker::execute() {
    ATH_MSG_DEBUG( "Executing " << name() << "..." );

    // output decisions
    auto Outdecisions = std::make_unique<DecisionContainer>();
    auto dec_aux = std::make_unique<DecisionAuxContainer>();
    Outdecisions->setStore( dec_aux.get() );

    // output collection, as a view container so it can be given const features
    auto output = std::make_unique<xAOD::TrigCompositeContainer>();
    auto aux = std::make_unique<xAOD::TrigCompositeAuxContainer>();
    output->setStore( aux.get() );

    ATH_MSG_DEBUG( "number of implicit ReadHandles is " << m_inputs.size() );
    std::map<const FeatureOBJ*,    TrigCompositeUtils::Decision* > featureFromDecision;
      
    // Loop over all input containers, which are of course TrigComposites, and request their features    
    size_t outputIndex = 0;
    for ( auto inputKey: m_inputs ) {
      auto inputHandle = SG::makeHandle( inputKey );
      ATH_MSG_DEBUG( "Got DecisionContainer from input "<< inputKey.key()<<" with " << inputHandle->size() << " elements" );

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
	ATH_MSG_DEBUG("Found feature " <<m_linkName.value() <<": " << *feature );

       // merge outputs that are linked to the same feature (RoI): this avoids processing the same RoI from TC decisions from different chains
	bool already_exist=false;
	auto it = featureFromDecision.find(feature);
	if (it != featureFromDecision.end()){
	  already_exist=true;	 
	}

	TrigCompositeUtils::Decision* newd;
	if (! already_exist) {
	  ATH_MSG_DEBUG( "Make new decision");

	  // create the "reco" output
	  auto tc = new xAOD::TrigComposite;
	  output->push_back(tc);	  
	  // copy all features to a single output collection 
	  tc->setObjectLink(m_linkName.value(), featurelink);

	  // create new decision
	  newd = newDecisionIn( Outdecisions.get() );
	  newd->setObjectLink( m_linkName.value(), featurelink);
	  // this is done only once, the first time it is found
	  newd->setObjectLink( "initialRoI", roiEL );
	  // do we have to store all previous descisions?
	  newd->setObjectLink( "previousDecisions", ElementLink<DecisionContainer>(inputKey.key(), counter) );// link to previous decision object
	  // update map
	  featureFromDecision.insert( std::make_pair( feature,  ( TrigCompositeUtils::Decision* ) newd ) ); 
	  outputIndex++;	  
	}
	else{
	  newd=featureFromDecision[feature];	  
	}
	
	ATH_MSG_DEBUG( "Copy decisions from input " << counter <<" with exist flag=" <<already_exist <<" to output "<< &newd   );
	{
          //copy decisions ID
          TrigCompositeUtils::DecisionIDContainer objDecisions;      
          TrigCompositeUtils::decisionIDs( decision, objDecisions );
          for ( const HLT::Identifier& id: objDecisions ){
	    TrigCompositeUtils::addDecisionID( id, newd );
	    // ATH_MSG_DEBUG( " -- adding decision " << HLT::Identifier( id ) );
	  }
	  // debug
	  // TrigCompositeUtils::DecisionIDContainer newObjDecisions;      
	  // TrigCompositeUtils::decisionIDs( newd, newObjDecisions );
	  // for ( const HLT::Identifier& id: newObjDecisions ){
	  //   ATH_MSG_DEBUG( " -- has decision " << HLT::Identifier( id ) );
	  // }
        }
	
	counter++;
	
        // For early tests, create TC, link to RoiD, push back onto TCC.
        // Later will output RoID collection directly via tool.
        
      } // loop over decisions
    } // loop over input keys

    
   
    // Finally, record output
    ATH_MSG_DEBUG("Should find "<<outputIndex<<" obejcts and decisions");
    ATH_MSG_DEBUG("Produced "<<output->size() <<" objects");
    auto reco_outputHandle = SG::makeHandle(m_recoOutput);
    CHECK( reco_outputHandle.record(std::move(output), std::move(aux)) );


    // and decision output
    ATH_MSG_DEBUG("Produced "<<Outdecisions->size() <<" decisions");
    auto dec_handle =  SG::makeHandle( m_decisionsKey );
    CHECK( dec_handle.record( std::move( Outdecisions ), std::move( dec_aux ) ) );
   

    //debug

    for (auto outh:  *dec_handle){
      TrigCompositeUtils::DecisionIDContainer objDecisions;      
      TrigCompositeUtils::decisionIDs( outh, objDecisions );
      
      ATH_MSG_DEBUG("Number of positive decisions for this input: " << objDecisions.size() );

      for ( TrigCompositeUtils::DecisionID id : objDecisions ) {
	ATH_MSG_DEBUG( " --- found decision " << HLT::Identifier( id ) );
      }  
    }


    return StatusCode::SUCCESS;
  }

} //> end namespace HLTTest
