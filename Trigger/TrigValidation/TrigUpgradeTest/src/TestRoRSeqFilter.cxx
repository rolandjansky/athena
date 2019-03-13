/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iterator>
#include <algorithm>

#include "DecisionHandling/HLTIdentifier.h"
#include "TestRoRSeqFilter.h"

#include "GaudiKernel/Property.h"

using namespace TrigCompositeUtils;

namespace HLTTest {

  TestRoRSeqFilter::TestRoRSeqFilter( const std::string& name, 
    ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
  {
    declareProperty( "Inputs", m_inputs );
    declareProperty( "Outputs", m_outputs );
    declareProperty( "Chains", m_chainsProperty, "Chains to filter the object on" );
    declareProperty( "AlwaysPass", m_alwaysPass, "Override negative decision" );
  }

  TestRoRSeqFilter::~TestRoRSeqFilter()
  {}

  StatusCode TestRoRSeqFilter::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    ATH_MSG_DEBUG("Will consume the input data: " << m_inputs << " and produce " << m_outputs );
    ATH_MSG_DEBUG("This filter has alwaysPass=" << m_alwaysPass);
    ATH_MSG_DEBUG("Configured chains are:");
    for ( auto& el: m_chainsProperty ) {
      ATH_MSG_DEBUG(el);
    }
    // if ( m_inputs.size() != m_outputs.size() ) {
    //   ATH_MSG_ERROR("Inputs and Outputs have different size, the mapping is unclear");
    //   return StatusCode::FAILURE;
    // }

    for ( auto& el: m_chainsProperty ) {
      m_chains.insert( HLT::Identifier(el).numeric() );
    }

    // minimal validity crosscheck
    if ( m_chainsProperty.size() == 0 and m_alwaysPass == false ) {
      ATH_MSG_WARNING("This filter will always reject as it has no chains of interest configured");
    }

    if ( m_chains.size() != m_chainsProperty.size() ) {
      ATH_MSG_ERROR("Repeated chain requirements (annoyance) or chain to hash conversion clash (major configuration issue therefore can not run)");
      return StatusCode::FAILURE;
    }  

    return StatusCode::SUCCESS;
  }

  StatusCode TestRoRSeqFilter::finalize()
  {
    ATH_MSG_INFO ("Finalizing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  StatusCode TestRoRSeqFilter::execute() 
  {
    ATH_MSG_DEBUG ( "Executing " << name() << "..." );
    bool pass = false;

    for ( size_t inputIndex = 0; inputIndex < m_inputs.size(); ++inputIndex ) {
      auto input = m_inputs[inputIndex];
      ATH_MSG_DEBUG( "Processing input " << input );
      // this will only store links to the inputs that are passing
      auto decisionOutput = std::make_unique<DecisionContainer>();
      auto decisionAux    = std::make_unique<DecisionAuxContainer>();
      decisionOutput->setStore( decisionAux.get() );
      copyPassing( input, decisionOutput.get() );

      // not bother recording if there is no output
      if ( not decisionOutput->empty() ) {
        ATH_MSG_DEBUG( "Saving output " << m_outputs[inputIndex] );
        pass = true;      
        SG::WriteHandle<DecisionContainer> outputDH( m_outputs[inputIndex] );
        CHECK( outputDH.record( std::move( decisionOutput ), std::move( decisionAux ) ) );  
      } else {
        ATH_MSG_DEBUG( "None of the decisions in the input " << input << " passed, skipping recording output " );
      }
    }

    ATH_MSG_DEBUG( "The overall decision is : " << ( pass or m_alwaysPass ? "positive" : "negative") );

    setFilterPassed( pass or m_alwaysPass );
    return StatusCode::SUCCESS;
  }

  void TestRoRSeqFilter::copyPassing( const std::string& inputKey, DecisionContainer* output ) const {
    SG::ReadHandle<DecisionContainer> inputDH( inputKey );
    if ( not inputDH.isValid() ) {
      ATH_MSG_DEBUG( "Void input: " << inputKey );
      return;
    }

    ATH_MSG_DEBUG( "Filtering "<<inputDH->size()<<" obejcts with key "<<inputKey);

    size_t counter=0;
    for ( auto objIter =  inputDH->begin(); objIter != inputDH->end(); ++objIter, ++counter ) {      
      DecisionIDContainer objDecisions;      
      decisionIDs( *objIter, objDecisions ); // this should be replaced in production code by method passingIDs but the later is not printing so in tests we want the IDs
      // here we have to sets of IDs, those we are interested in (via chain property) and those comming from the decision obj

      std::vector<DecisionID> intersection;
      std::set_intersection( m_chains.begin(), m_chains.end(),
      objDecisions.begin(), objDecisions.end(),
      std::back_inserter( intersection ) );
    
      ATH_MSG_DEBUG( "Found "<<intersection.size()<<" objects of interest for key "<<inputKey);
      for ( auto positive: intersection ) {
        ATH_MSG_DEBUG( "Found positive decision for chain " << HLT::Identifier( positive ) );
      }

      const bool positiveObjectDecision = not intersection.empty();
      if ( positiveObjectDecision ) {
        auto d = newDecisionIn( output );
        linkToPrevious( d, *objIter );
      } else {
        ATH_MSG_DEBUG( "Skipping objects with key " << inputKey <<" as they passed no chain of interest to this filter" );
      }
    }
  }
  
} //> end namespace HLTTest


