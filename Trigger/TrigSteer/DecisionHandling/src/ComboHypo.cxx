/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ComboHypo.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "AthViews/View.h"

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
  renounceArray( m_inputs ); // do we need it?
  ATH_CHECK( m_inputs.size() == m_outputs.size() );
  ATH_MSG_INFO( "with these inputs: ");
  for (auto inp:m_inputs){
    ATH_MSG_INFO("-- "<< inp.key());
  }
  

  ATH_CHECK( m_multiplicitiesReqMap.size() != 0 );

  

  // find max inputs size
  auto maxMultEl = std::max_element( m_multiplicitiesReqMap.begin(), m_multiplicitiesReqMap.end(),  
   []( MultiplicityReqMap::value_type a, MultiplicityReqMap::value_type b ){  
     return a.second.size() < b.second.size(); } ); 
  
  const size_t maxMult = maxMultEl->second.size();

  ATH_MSG_INFO( "with this multiplicity map: ");
  for ( auto m: m_multiplicitiesReqMap ) {
    ATH_MSG_INFO("-- "<< m.first<<" multiplicities of size "<<m.second.size());
    for (auto mult: m.second){
      ATH_MSG_INFO("-- "<< mult<<", ");
    }
  }

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


StatusCode ComboHypo::copyDecisions( const DecisionIDContainer& passing, const EventContext& context ) const {

  ATH_MSG_DEBUG( "Copying "<<passing.size()<<" positive decisions to outputs");
  for ( size_t input_counter = 0; input_counter < m_inputs.size(); ++input_counter ) {

    // new output decisions
    SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(m_outputs[input_counter], context ); 
    auto outDecisions = outputHandle.ptr();    

    auto inputHandle = SG::makeHandle( m_inputs[input_counter], context );
    if ( inputHandle.isValid() ) {
      
      for (const Decision* inputDecision: *inputHandle) {

        DecisionIDContainer inputDecisionIDs;
        decisionIDs( inputDecision, inputDecisionIDs );

        // from all poitive decision in the input only the ones that survived counting are passed over
        DecisionIDContainer common;      
        std::set_intersection( inputDecisionIDs.begin(), inputDecisionIDs.end(), passing.begin(), passing.end(),
          std::inserter( common, common.end() ) );

        Decision*  newDec = newDecisionIn( outDecisions );
        linkToPrevious( newDec, inputDecision );
        ATH_MSG_DEBUG("New decision has "<< (TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( newDec, "initialRoI")).isValid()
          << " valid initialRoI and "<< TrigCompositeUtils::getLinkToPrevious(newDec).size() <<" previous decisions");   

        insertDecisionIDs( common, newDec );

        // add View?
        if ( inputDecision->hasObjectLink( "view" ) ) {
          auto viewEL = inputDecision->objectLink< ViewContainer >( "view" );
          CHECK( viewEL.isValid() );
          newDec->setObjectLink( "view",    viewEL );
        }
      }
    }

    // debug printout:
    if ( msgLvl(MSG::DEBUG) ) {
      ATH_MSG_DEBUG(outputHandle.key() <<" with "<< outputHandle->size() <<" decisions:");
      for (auto outdecision:  *outputHandle){
        TrigCompositeUtils::DecisionIDContainer objDecisions;      
        TrigCompositeUtils::decisionIDs( outdecision, objDecisions );    
        ATH_MSG_DEBUG("Number of positive decisions for this output: " << objDecisions.size() );
        for ( TrigCompositeUtils::DecisionID id : objDecisions ) {
          ATH_MSG_DEBUG( " ---  decision " << HLT::Identifier( id ) );
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

void ComboHypo::fillDecisionsMap( std::vector< MultiplicityMap >&  dmap, const EventContext& context) const {
  for ( size_t i = 0; i < m_inputs.size(); ++i ) {   
    auto inputHandle = SG::makeHandle( m_inputs.at(i), context );
    if ( inputHandle.isValid() ) {
      ATH_MSG_DEBUG( "Found implicit RH from " << inputHandle.key() <<" with "<< inputHandle->size() << " elements:"  );
      MultiplicityMap thisInputDmap;
      for ( const Decision* decision : *inputHandle.cptr() ) {
        ATH_MSG_DEBUG( "Decision with "<< decisionIDs( decision ).size() << " chains passed:" );
        for ( DecisionID id: decisionIDs( decision ) ) {
          ATH_MSG_DEBUG( " +++ " << HLT::Identifier( id ) );
          thisInputDmap[id] ++;
        }     
      }
      dmap[i]= thisInputDmap; 
    }
    else {
      ATH_MSG_DEBUG( "No implicit RH from " << inputHandle.key()  );
      //dmap empty in this case
    }
  }

  //debug
  ATH_MSG_DEBUG( "Decision map filled :" );
  int i=0;
  for (auto onemap: dmap){
    ATH_MSG_DEBUG("map ["<<i<<"]: ");
    for (auto m: onemap){
      ATH_MSG_DEBUG(" +++ " << HLT::Identifier( m.first ) <<" mult: "<<m.second);
    }
    i++;
  }
}

StatusCode ComboHypo::execute(const EventContext& context ) const {
  ATH_MSG_DEBUG( "Executing " << name() << "..." );

  
  DecisionIDContainer passing;
  // this map is filled with the count of positive decisions from each input

  std::vector< MultiplicityMap > dmap( m_inputs.size() );
  fillDecisionsMap( dmap, context );

  for ( auto m: m_multiplicitiesReqMap ) {
    const DecisionID requiredDecisionID = HLT::Identifier( m.first ).numeric();
    bool overallDecision = true;
    
    for ( size_t inputContainerIndex = 0; inputContainerIndex <  m.second.size(); ++inputContainerIndex ) {
      const int requiredMultiplicity =  m.second[ inputContainerIndex ];
      const int observedMultiplicity = dmap[ inputContainerIndex ][ requiredDecisionID ];
      ATH_MSG_DEBUG( "Required multiplicity " << requiredMultiplicity  << " for chain " << m.first<< ": observed multiplicity " << observedMultiplicity << " in container " << inputContainerIndex  );
      if ( observedMultiplicity < requiredMultiplicity ) {
        overallDecision = false;
        break;
      }
    }
    ATH_MSG_DEBUG( "Chain " << m.first <<  ( overallDecision ? " is accepted" : " is rejected") );
    if ( overallDecision == true ) {
      passing.insert( requiredDecisionID );
    }      
  }
  
  ATH_CHECK( copyDecisions( passing, context ) );
  
  return StatusCode::SUCCESS;
}

