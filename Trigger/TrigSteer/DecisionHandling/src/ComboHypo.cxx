/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ComboHypo.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "AthViews/View.h"
#include <sstream>

using namespace TrigCompositeUtils;
ComboHypo::ComboHypo(const std::string& name, ISvcLocator* pSvcLocator) 
: ::AthReentrantAlgorithm(name, pSvcLocator)
{}

ComboHypo::~ComboHypo()
{}

StatusCode ComboHypo::initialize() {
  ATH_CHECK( m_outputs.initialize() );
  ATH_CHECK( m_inputs.initialize() );
  ATH_CHECK( m_inputs.size() == m_outputs.size() );
  ATH_MSG_INFO( "with these inputs: ");
  for (auto inp:m_inputs){
    ATH_MSG_INFO("-- "<< inp.key());  }
  

  // find max inputs size
  auto maxMultEl = std::max_element( m_multiplicitiesReqMap.begin(), m_multiplicitiesReqMap.end(),  
   []( MultiplicityReqMap::value_type a, MultiplicityReqMap::value_type b ){  
     return a.second.size() < b.second.size(); } ); 
  
  const size_t maxMult = maxMultEl->second.size();

  if (msgLvl(MSG::INFO)){ 
    ATH_MSG_INFO( "with this multiplicity map: ");
    for ( auto m: m_multiplicitiesReqMap ) {
      std::ostringstream msg;
      std::string mms = "[";
      msg<<"[";
      for (auto mult: m.second){
	msg<< mult<<", ";
	mms+= mult  + ",";
      }
      msg<<"]";
      ATH_MSG_INFO("-- "<< m.first<<" multiplicities: "<<msg.str() <<"     "<<mms);
    }
  }

  ATH_CHECK( m_multiplicitiesReqMap.size() != 0 );
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
    int decCounter = 0;
    auto inputHandle = SG::makeHandle( m_inputs[input_counter], context );
    if ( inputHandle.isValid() ) {
      
      for (const Decision* inputDecision: *inputHandle) {

        DecisionIDContainer inputDecisionIDs;
        decisionIDs( inputDecision, inputDecisionIDs );

        // from all poitive decision in the input only the ones that survived counting are passed over
        DecisionIDContainer common;      
        std::set_intersection( inputDecisionIDs.begin(), inputDecisionIDs.end(), passing.begin(), passing.end(),
          std::inserter( common, common.end() ) );

	// collect only chain name from leg name: do not propagate the legdIds?
	DecisionIDContainer finalIds;   
	for (auto c: common){
	  finalIds.insert(c); // all Ids used by the Filter, including legs
	  if (TrigCompositeUtils::isLegId ( HLT::Identifier(c) )){
	    auto mainChain = TrigCompositeUtils::getIDFromLeg ( HLT::Identifier(c) );
	    finalIds.insert( mainChain.numeric() );
	  }
	}

        Decision*  newDec = newDecisionIn( outDecisions );
        linkToPrevious( newDec, inputDecision, context );
        ATH_MSG_DEBUG("New decision ("<< input_counter<<", "<<decCounter<<") has "
		      << (TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( newDec, "initialRoI")).isValid()
		      << " valid initialRoI and "<< TrigCompositeUtils::getLinkToPrevious(newDec).size() <<" previous decisions; valid Ids="<<finalIds.size()) ;   

        insertDecisionIDs( finalIds, newDec );

        // add View?
        if ( inputDecision->hasObjectLink( "view" ) ) {
          auto viewEL = inputDecision->objectLink< ViewContainer >( "view" );
          CHECK( viewEL.isValid() );
          newDec->setObjectLink( "view",    viewEL );
        }
	decCounter++;
      }
    }


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


StatusCode ComboHypo::execute(const EventContext& context ) const {
  ATH_MSG_DEBUG( "Executing " << name() << "..." );
  
  DecisionIDContainer passing;
  // this map is filled with the count of positive decisions from each input
  CombinationMap dmap;
  std::map<TrigCompositeUtils::DecisionID,std::map<int, int>> dmapFeatures;
  uint nRequiredUnique;
  fillDecisionsMap( dmap, dmapFeatures, context );

  int nmaps=0;
  for ( auto m: m_multiplicitiesReqMap ) {
    nRequiredUnique=0;
    const DecisionID requiredDecisionID = HLT::Identifier( m.first ).numeric();
    DecisionIDContainer allDecisionIds;
    allDecisionIds.insert(requiredDecisionID);

    bool overallDecision = true;

    std::set< int> uniqueDecisions;
    for ( size_t legIndex = 0; legIndex <  m.second.size(); ++legIndex ) {
      const size_t requiredMultiplicity =  m.second[ legIndex ];
      nRequiredUnique += requiredMultiplicity;

      auto legId = TrigCompositeUtils::createLegName(HLT::Identifier(m.first), legIndex);
      if (m.second.size()==1) legId = HLT::Identifier(m.first); // if one part in the menu name, do not use leg00 in name
      const DecisionID requiredDecisionIDLeg = legId.numeric();
      ATH_MSG_DEBUG("Container "<< legIndex<<" Looking at leg : "<<legId );
     
      //check each leg of the chain passes with required multiplicity
      size_t observedMultiplicity = 0;
      if ( dmap.count(requiredDecisionIDLeg) != 0 ){
	observedMultiplicity = dmap[ requiredDecisionIDLeg ].size();
	ATH_MSG_DEBUG( "Required multiplicity " << requiredMultiplicity  << " for leg " << legId.name() << ": observed multiplicity " << observedMultiplicity << " in leg " << legIndex  );
      }
      if ( observedMultiplicity < requiredMultiplicity ) {
	overallDecision = false;
	break;
      }
	
      
      const size_t uniqueDecisionsInLeg = dmapFeatures[requiredDecisionIDLeg].size();
      
      //check that the multiplicity of unique features is high enough
      ATH_MSG_DEBUG("Number of unique decisions per Leg: "<<uniqueDecisionsInLeg<<", number of required decisions: "<<requiredMultiplicity);
      if(uniqueDecisionsInLeg<requiredMultiplicity) overallDecision=false;
      else
	allDecisionIds.insert(requiredDecisionIDLeg);
      for (auto feat: dmapFeatures[requiredDecisionIDLeg]){
	uniqueDecisions.insert( feat.first );
      }
    }

    //check that the multiplicity of unique features is high enough
    ATH_MSG_DEBUG("Number of unique decisions: "<<uniqueDecisions.size()<<", number of required unique decisions: "<<nRequiredUnique);
    if (uniqueDecisions.size()<nRequiredUnique) overallDecision=false;

    //Overall chain decision
    ATH_MSG_DEBUG( "Chain " << m.first <<  ( overallDecision ? " is accepted" : " is rejected") );
    if ( overallDecision == true ) {
      for (auto decID: allDecisionIds)
	passing.insert( passing.end(), decID );
    }      
    nmaps++;
  }

  if (msgLvl(MSG::DEBUG)){
    for (auto p: passing)
      ATH_MSG_DEBUG("Passing "<<HLT::Identifier(p));
  }
  ATH_CHECK( copyDecisions( passing, context ) );
  
  return StatusCode::SUCCESS;
}

void ComboHypo::fillDecisionsMap(  CombinationMap &  dmap, std::map<TrigCompositeUtils::DecisionID,std::map<int,int>> & featureMap, const EventContext& context) const {
  /* pair<index_of_input, index_of_decision>
     map <decision_id, vector<pairs<>> list of all decisions of that leg-decID
     vector < map<> > = list of all leg-decID, one element per legID
     leg0 leg1
  */

  for ( size_t i = 0; i < m_inputs.size(); ++i ) {   
    auto inputHandle = SG::makeHandle( m_inputs.at(i), context );
    if ( inputHandle.isValid() ) {
      ATH_MSG_DEBUG( "Found implicit RH from " << inputHandle.key() <<" with "<< inputHandle->size() << " elements:"  );
      std::map<TrigCompositeUtils::DecisionID,std::map<int,int>> thisFeatureMap;
      int decCounter = 0;
      for ( const Decision* decision : *inputHandle.cptr() ) {
        ATH_MSG_DEBUG( "Input Decision "<<decCounter <<" with "<< decisionIDs( decision ).size() << " active chains IDs" );
        for ( DecisionID id: decisionIDs( decision ) ) {
	  for ( auto m: m_multiplicitiesReqMap ) {
	    // search for the chain name in the list of active ones (including legs)
	    if (HLT::Identifier(id).name().find(m.first) != std::string::npos){
	      ATH_MSG_DEBUG( " +++ " << HLT::Identifier( id ) );
	      std::pair <int,int> combinationIndex = std::make_pair(i, decCounter);
	      dmap[id].push_back(combinationIndex);
	      ATH_MSG_DEBUG("Added in map: "<<dmap[id]);
	  
	  //Map features to make sure we are considering unique objects. decision->linkColKeys()[idx] 
	  //is the feature collection's name-hash, which is unique per-RoI. Adding the object index 
	  //allows to correctly identify different objects within the same RoI (example: EF muons)
	  //or different objects produced per event (example: MET) as being unique
	  auto features = decision->linkColNames();
	  int idx=-1;
	  for(uint f=0; f<features.size(); f++){
	    if(features[f]==featureString()){
	      idx=f;
	      break;
	    }
	  }
	  if(idx>=0){
	    featureMap[id][decision->linkColKeys()[idx]+decision->linkColIndices()[idx]] ++;
	  }
	  else ATH_MSG_DEBUG("Did not find "<<featureString());
        }
	  }}
	decCounter ++;
      }
    }
    else {
      ATH_MSG_DEBUG( "No implicit RH from " << inputHandle.key()  );
      //dmap empty in this case
    }
  }


  if (msgLvl(MSG::DEBUG)){
    ATH_MSG_DEBUG( "Decision map filled :" );
    int legcount=0;
    for (auto m: dmap){
      ATH_MSG_DEBUG("leg ["<<legcount<<"]: ");
      std::vector<std::pair<int,int>> combinations = m.second;
      ATH_MSG_DEBUG(" +++ " << HLT::Identifier( m.first ) <<" mult: "<<combinations.size());
      for (auto comb: combinations){
      	ATH_MSG_DEBUG("     Comb: ("<<comb.first<<", "<<comb.second<<")");
      }

      ATH_MSG_DEBUG("FeatureMap: found "<<featureMap[m.first].size()<<" entries");
      for (auto feat:featureMap[m.first])
	ATH_MSG_DEBUG("    Feature Id "<<feat.first<<", mult "<<feat.second);
      legcount++;
    }
  }

  
}
