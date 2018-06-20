/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ComboHypo.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "DecisionHandling/HLTIdentifier.h"

using namespace TrigCompositeUtils;
ComboHypo::ComboHypo(const std::string& name, ISvcLocator* pSvcLocator) 
  : InputMakerBase(name, pSvcLocator)
{}

ComboHypo::~ComboHypo()
{}

StatusCode ComboHypo::initialize() {
  //ATH_MSG_DEBUG("Use macros for logging!");
  for ( auto m: m_multiplicitiesMap ) {
    // check if all is correctly configured, the same multiplicity everywhere
  }
  return StatusCode::SUCCESS;
}

StatusCode ComboHypo::finalize() {
  return StatusCode::SUCCESS;
}


StatusCode ComboHypo::copyDecisions( const DecisionIDContainer& passing ) const {
  for ( size_t input_counter = 0; input_counter < decisionInputs().size(); ++input_counter ) {

    auto outDecisions = std::make_unique<DecisionContainer>();
    auto outDecAux = std::make_unique<DecisionAuxContainer>();
    outDecisions->setStore( outDecAux.get() );
    

    auto inputHandle = SG::makeHandle( decisionInputs()[input_counter] );
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
    auto handle = SG::makeHandle( decisionOutputs()[input_counter] );
    ATH_CHECK( handle.record( std::move( outDecisions ), std::move( outDecAux ) ) );
  }
  return StatusCode::SUCCESS;
}

void ComboHypo::fillDecisionsMap( std::vector< MultiplicityMap >&  dmap) const {
  for ( size_t i = 0; i < decisionInputs().size(); ++i ) {   
    auto inputHandle = SG::makeHandle( decisionInputs().at(i) );
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
  std::vector< MultiplicityMap > dmap( decisionInputs().size() );
  fillDecisionsMap( dmap );

  for ( auto m: m_multiplicitiesMap ) {
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

