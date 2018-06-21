/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ComboHypo.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "DecisionHandling/HLTIdentifier.h"

using namespace TrigCompositeUtils;
ComboHypo::ComboHypo(const std::string& name, ISvcLocator* pSvcLocator) 
  : ::AthReentrantAlgorithm(name, pSvcLocator)
{}

ComboHypo::~ComboHypo()
{}

StatusCode ComboHypo::initialize() {
  //ATH_MSG_DEBUG("Use macros for logging!");
  ATH_CHECK( m_outputs.initialize() );
  ATH_CHECK( m_inputs.initialize() );
  ATH_CHECK( m_inputs.size() == m_outputs.size() );


  ATH_CHECK( m_multiplicitiesReqMap.size() != 0 );

  // find max inputs size
  auto maxMultEl = std::max_element( m_multiplicitiesReqMap.begin(), m_multiplicitiesReqMap.end(),  
				   []( MultiplicityReqMap::value_type a, MultiplicityReqMap::value_type b ){  
				     return a.second.size() < b.second.size(); } ); 
  
  const size_t maxMult = maxMultEl->second.size();

  bool errorOccured = false;
  for ( auto m: m_multiplicitiesReqMap ) {
    if ( m.second.size() != maxMult )  {
      errorOccured =  true;
      ATH_MSG_ERROR( "Chain " << m.first << " configured with input multiplicity " << m.second.size() << " like this: " <<  m.second << " which is lower than for this chain " << maxMultEl->first <<  " " << maxMult);
    }
  }

  return ( errorOccured ?  StatusCode::FAILURE :  StatusCode::SUCCESS );
}

StatusCode ComboHypo::finalize() {
  return StatusCode::SUCCESS;
}


StatusCode ComboHypo::copyDecisions( const DecisionIDContainer& passing ) const {
  for ( size_t input_counter = 0; input_counter < m_inputs.size(); ++input_counter ) {

    auto outDecisions = std::make_unique<DecisionContainer>();
    auto outDecAux = std::make_unique<DecisionAuxContainer>();
    outDecisions->setStore( outDecAux.get() );
    

    auto inputHandle = SG::makeHandle( m_inputs[input_counter] );
    const DecisionContainer* inputs = inputHandle.cptr();
    const size_t sz = inputs->size();

    for ( size_t i = 0; i < sz; ++i  ) {
      const Decision *inputDecision = inputs->at(i);
      DecisionIDContainer inputDecisionIDs;
      decisionIDs( inputDecision, inputDecisionIDs );

      // from all poitive decision in the input only the ones that survived counting are passed over
      DecisionIDContainer common;      
      std::set_intersection( inputDecisionIDs.begin(), inputDecisionIDs.end(), passing.begin(), passing.end(),
			     std::inserter( common, common.end() ) );

      Decision*  newDec = newDecisionIn( outDecisions.get() );
      linkToPrevious( newDec, inputHandle.key(), i );      

      for ( auto id: common ) {
	addDecisionID( id, newDec );
      }      
    }
    auto handle = SG::makeHandle( m_outputs[input_counter] );
    ATH_CHECK( handle.record( std::move( outDecisions ), std::move( outDecAux ) ) );
  }
  return StatusCode::SUCCESS;
}

void ComboHypo::fillDecisionsMap( std::vector< MultiplicityMap >&  dmap) const {
  for ( size_t i = 0; i < m_inputs.size(); ++i ) {   
    auto inputHandle = SG::makeHandle( m_inputs.at(i) );
    MultiplicityMap& thisInputDmap = dmap[i]; 
    for ( const Decision* decision : *inputHandle.cptr() ) {
      for ( DecisionID id: decisionIDs( decision ) ) {
	thisInputDmap[id] ++;
      }	    
    }
  }
}

StatusCode ComboHypo::execute_r(const EventContext& ) const {
  DecisionIDContainer passing;
  // this map is filled with the count of positive decisions from each input
  std::vector< MultiplicityMap > dmap( m_inputs.size() );
  fillDecisionsMap( dmap );

  for ( auto m: m_multiplicitiesReqMap ) {
    const DecisionID requiredDecisionID = HLT::Identifier( m.first ).numeric();
    bool overallDecision = true;

    for ( size_t inputContainerIndex = 0; inputContainerIndex <  m.second.size(); ++inputContainerIndex ) {
      const int requiredMultiplicity =  m.second[ inputContainerIndex ];
      const int observedMultiplicity = dmap[ inputContainerIndex ][ requiredDecisionID ];
      ATH_MSG_DEBUG( "Required multiplicity " << requiredMultiplicity << " observed multiplicity " << observedMultiplicity << " in container " << inputContainerIndex << " for chain " << m.first  );
      if ( observedMultiplicity < requiredMultiplicity ) {
	overallDecision = false;
	break;
      }
    }
    ATH_MSG_DEBUG( "Chain " << m.first <<  ( overallDecision ? "is accepted" : "is rejected") );
    if ( overallDecision == true ) {
      passing.insert( requiredDecisionID );
    }      
  }

  ATH_CHECK( copyDecisions( passing ) );
  
  return StatusCode::SUCCESS;
}

