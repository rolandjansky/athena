/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ComboHypoToolBase.h"
using namespace TrigCompositeUtils;


ComboHypoToolBase::ComboHypoToolBase(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent),
  m_decisionId(  HLT::Identifier::fromToolName( name ) )
{}


StatusCode ComboHypoToolBase::decide(const LegDecisionsMap & IDCombMap, LegDecisionsMap & passingCombinations ) const
{
  // if no combinations passed, then exit 
  size_t nLegs=IDCombMap.size();
  if (nLegs==0)  return StatusCode::SUCCESS;
  
  //  check that no other toold have efilled the map with this id
   ATH_CHECK( passingCombinations[decisionId()].empty() );
 
   ATH_MSG_DEBUG( "Looking for "<< decisionId() <<" in the map. Map contains "<<nLegs<<" legs");

   // select the leg decisions from  the map with this ID:
   std::vector<ElementLinkVector<TrigCompositeUtils::DecisionContainer>> leg_decisions;
   ATH_CHECK( selectLegs(IDCombMap, leg_decisions) );

   if (leg_decisions.size() == 0) {
     ATH_MSG_INFO("Found 0 legs with this DecisionID: something failed?");
     return StatusCode::SUCCESS;
   }
   
   // create the combinations between the legs
   std::vector<ElementLinkVector<TrigCompositeUtils::DecisionContainer>> combinations;
   createCombinations( leg_decisions, combinations,  nLegs, 2);

   
   // do the actual algorithm and select the decisions that passed
   for (ElementLinkVector<TrigCompositeUtils::DecisionContainer> thecomb: combinations){
     // to add: protection when the two decisions are the same object
     bool pass = executeAlg(thecomb);
     if (pass){
       setDecisionIds(thecomb, passingCombinations);
     }
   }
  
   printExecute(passingCombinations);
   return StatusCode::SUCCESS;

}

StatusCode ComboHypoToolBase::selectLegs(const LegDecisionsMap & IDCombMap, std::vector<ElementLinkVector<TrigCompositeUtils::DecisionContainer>>& leg_decisions) const
{
  size_t nLegs=IDCombMap.size();
  if (nLegs==0)  return StatusCode::SUCCESS;
  
  // collect combinations from all the legs, searching both chain name and leg name
  for (auto id: IDCombMap){
    // get the Element links from the chainID (from all the legs)
    HLT::Identifier chainId=0;
    if (TrigCompositeUtils::isLegId(id.first))
      chainId= TrigCompositeUtils::getIDFromLeg(id.first);
    else
      chainId=id.first;
    if ( chainId != decisionId() ) continue;    
    auto comb = id.second; 
    leg_decisions.push_back(comb);
  }

  if (nLegs != leg_decisions.size()){
    ATH_MSG_ERROR("Expecting "<<nLegs<<" legs, but found "<< leg_decisions.size() <<" legs to combine");
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_DEBUG("Getting "<<leg_decisions.size()<<" legs to combine, for ID: "<< decisionId());
  for (auto leg: leg_decisions){
    ATH_MSG_DEBUG("Leg --");
    for (auto dEL:leg) ATH_MSG_DEBUG(dEL.dataID() << " , " << dEL.index());
  }

  return StatusCode::SUCCESS;
}


// Include case of one leg and multiple legs with mult=1 on each leg. Need to be exended to cover cases with multiple legs qith different multiplicities (2mu_3e)
void ComboHypoToolBase::createCombinations(const std::vector<ElementLinkVector<TrigCompositeUtils::DecisionContainer>> & v_legs,
			  std::vector<ElementLinkVector<TrigCompositeUtils::DecisionContainer>> & combinations, int nLegs, int nToGroup) const
{
  if (nLegs ==1) {
    auto elements = v_legs[0];
    std::vector<int> selector(elements.size());
    fill(selector.begin(), selector.begin() + nToGroup, 1);
    ElementLinkVector<TrigCompositeUtils::DecisionContainer> thecomb;
    do {
      for (u_int i = 0; i < elements.size(); i++) {
	if (selector[i]) {
	  thecomb.push_back(elements[i]);
	}
      }
      combinations.push_back(thecomb);    
      thecomb.clear();
    }  while (std::prev_permutation(selector.begin(), selector.end()));
  }

  else {
    ElementLinkVector<TrigCompositeUtils::DecisionContainer> local;
    recursive_combine(v_legs, combinations, local, 0);
  }

  for (auto comb: combinations){
    ATH_MSG_DEBUG("Combination");
    for (auto dEL :comb){
      ATH_MSG_DEBUG(dEL.dataID() << " , " << dEL.index());
    }
  }
  return;
}


void ComboHypoToolBase::setDecisionIds(const ElementLinkVector<TrigCompositeUtils::DecisionContainer>& thecomb, LegDecisionsMap & passingCombinations) const {

  for (ElementLink<TrigCompositeUtils::DecisionContainer> dEL: thecomb){
    const TrigCompositeUtils:: Decision* decision= (*dEL);
    // get back the decID of this element
    const std::vector<DecisionID>& allDecIDs = TrigCompositeUtils::decisionIDs( decision );
    ATH_MSG_DEBUG( dEL.dataID() << " , " << dEL.index() <<"  with "<<allDecIDs.size() <<" decisionIDs");
    for (auto id: allDecIDs){
      if ((HLT::Identifier(id).name()).find(decisionId().name())!=std::string::npos){
	// we may have this element already stored, so there might be duplications
	// add to both legID and chainID?
	passingCombinations[id].push_back(dEL);
	passingCombinations[ decisionId() ].push_back(dEL);
      }
    }
  }

  // remove duplicates?
  return;
}



StatusCode ComboHypoToolBase::printExecute(const LegDecisionsMap & passingCombinations) const {
  ATH_MSG_DEBUG("End of tool: Passing elments are: ");
  for  (auto id: passingCombinations){
    ATH_MSG_DEBUG("Id "<<HLT::Identifier(id.first)<<" with "<<id.second.size()<<" elements");
    for (auto dEL: id.second){
      ATH_MSG_DEBUG( dEL.dataID() << " , " << dEL.index());
    }
  }
  return StatusCode::SUCCESS;
}


void ComboHypoToolBase::recursive_combine(const std::vector<ElementLinkVector<TrigCompositeUtils::DecisionContainer>> &all,
			 std::vector<ElementLinkVector<TrigCompositeUtils::DecisionContainer>> & tocombine,
			 ElementLinkVector<TrigCompositeUtils::DecisionContainer> & local, u_int lindex)  const
{
  
  for (size_t leg =lindex; leg<all.size(); leg++){
    for (size_t i=0; i<all[leg].size(); i++){
      local.push_back(all[leg][i]);
      recursive_combine(all, tocombine,local, leg+1);
      local.pop_back(); 
    }
  }
  if (local.size() == all.size())
    tocombine.push_back(local);
 
 return;
 
}
