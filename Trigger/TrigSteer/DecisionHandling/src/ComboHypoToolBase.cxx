/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "DecisionHandling/ComboHypoToolBase.h"
using namespace TrigCompositeUtils;


ComboHypoToolBase::ComboHypoToolBase(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent),
  m_decisionId(  HLT::Identifier::fromToolName( name ) )
{}


StatusCode ComboHypoToolBase::decide(LegDecisionsMap & passingLegs, const EventContext& /* context */ ) const
{
  // if no combinations passed, then exit 
  if (passingLegs.size()==0)  return StatusCode::SUCCESS;
  
   ATH_MSG_DEBUG( "Looking for "<< decisionId() <<" in the map. Map contains "<<passingLegs.size()<<" legs");
   //ATH_CHECK( printDebugInformation(passingLegs));

   // select the leg decisions from  the map with this ID:
   std::vector<std::vector<LegDecision>> leg_decisions;
   ATH_CHECK( selectLegs(passingLegs, leg_decisions) );

   if (leg_decisions.size() == 0) {
     ATH_MSG_INFO("Found 0 legs with this DecisionID: something failed?");
     return StatusCode::SUCCESS;
   }

   size_t nLegs = leg_decisions.size();
   
   // create the combinations between the legs:
   // next: TODO with Combinators.h, to crate one combination  at a time
   std::vector<std::vector<LegDecision>> combinations;
   createCombinations( leg_decisions, combinations,  nLegs, 2); // do we need to pass nLegs?

   std::vector<std::vector<LegDecision>> passing_combinations;
   // do the actual algorithm and select the decisions that passed
   for (std::vector<LegDecision>& thecomb: combinations){
     // to add: protection when the two decisions are the same object
     bool pass = executeAlg(thecomb);
     if (pass){
       passing_combinations.push_back(thecomb);
     }
   }

   updateLegDecisionsMap(passing_combinations, passingLegs);
   ATH_MSG_DEBUG("End of Tool  -----");
   ATH_CHECK( printDebugInformation(passingLegs));
   //ATH_CHECK( createOutput( passingLegs, context ));
   return StatusCode::SUCCESS;

}


// StatusCode ComboHypoToolBase::createOutput( LegDecisionsMap & passingLegs, const EventContext& context ) const {

//   std::vector< SG::WriteHandle<DecisionContainer> >  outputHandles = m_passingLegs.makeHandles(context);
//   ATH_MSG_DEBUG("Creating the output Handle Array of size "<<outputHandles.size());
  
//   int nleg=0;
//   DecisionContainer*  thisLegDecisions;
//   Decision* newDec;
//   for  (auto id: passingLegs){
//     ATH_MSG_DEBUG("Id "<<HLT::Identifier(id.first)<<" with "<<id.second.size()<<" elements");
//     createAndStore( outputHandles[nleg]);
//     thisLegDecisions = outputHandles[nleg].ptr();
//     ATH_MSG_DEBUG("got handles: "<< thisLegDecisions->size());
//     for (auto dEL: id.second){
//       const Decision* prevDec= (*dEL);
//       newDec = newDecisionIn( thisLegDecisions);//, prevDec , "", context);
//       linkToPrevious( newDec, prevDec, context ); // Link inputDecision object as the 'seed' of newDec
//       insertDecisionIDs( prevDec, newDec ); // Copy decision IDs from inputDecision into newDec
//     }
//     nleg++;
//   }
//   return StatusCode::SUCCESS;

// }

StatusCode ComboHypoToolBase::selectLegs(const LegDecisionsMap & IDCombMap, std::vector<std::vector<LegDecision>>& leg_decisions) const
{
  /*
    leg_decisions: vector with vectors like:
    [(leg0, el0), (leg0, el1)],
    [(leg1, mu0), (leg1, mu1)]
  */

  ElementLinkVector<TrigCompositeUtils::DecisionContainer> comb;
  // collect combinations from all the legs, searching both chain name and leg name
  for (auto id: IDCombMap){
    // get the Element links from the chainID (from all the legs)
    HLT::Identifier chainId=0;
    if (TrigCompositeUtils::isLegId(id.first))
      chainId= TrigCompositeUtils::getIDFromLeg(id.first);
    else
      chainId=id.first;
  
    if ( chainId != decisionId() ) continue;
    comb = id.second; 
    std::vector<LegDecision> oneleg;
    for (auto  el:  comb){
      auto newel = std::make_pair (id.first,el);      
      oneleg.push_back(newel);
    }
    leg_decisions.push_back(oneleg);
  }

  size_t nLegs= leg_decisions.size(); // the legs for this chain only
  if (nLegs==0) {
    ATH_MSG_INFO("There are no decisions in the legs to combine for ID "<< decisionId());
    return StatusCode::SUCCESS;
  }
  
  ATH_MSG_DEBUG("Getting "<<nLegs<<" legs to combine, for ID: "<< decisionId());
  for (auto leg: leg_decisions){
    ATH_MSG_DEBUG("Leg --");
    for (auto dEL:leg) {
      ATH_MSG_DEBUG(HLT::Identifier(dEL.first)
		    <<" "<<dEL.second.dataID() << " , " << dEL.second.index());
    }
  }

  return StatusCode::SUCCESS;
}


// TODO: Include case of one leg and multiple legs with mult=1 on each leg. Need to be exended to cover cases with multiple legs qith different multiplicities (2mu_3e)
void ComboHypoToolBase::createCombinations(const std::vector<std::vector<LegDecision>>& v_legs, std::vector<std::vector<LegDecision>> & combinations, int nLegs, int nToGroup) const
  
{
  if (nLegs ==1) {
    auto elements = v_legs[0];
    std::vector<int> selector(elements.size());
    fill(selector.begin(), selector.begin() + nToGroup, 1);
    std::vector<LegDecision> thecomb;
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
    std::vector<LegDecision> local;
    recursive_combine(v_legs, combinations, local, 0);
  }

  for (auto comb: combinations){
    ATH_MSG_DEBUG("Combination");
    for (auto dEL :comb){
      ATH_MSG_DEBUG(dEL.second.dataID() << " , " << dEL.second.index());
    }
  }
  return;
}


void ComboHypoToolBase::updateLegDecisionsMap(const std::vector<std::vector<LegDecision>> &passing_comb, LegDecisionsMap & passingLegs) const {

  // remove combinations that didn't pass from the final map passingLegs
  ATH_MSG_DEBUG("Updat with this combinations passing:");   
  for (auto &leg: passingLegs){
    auto legId= leg.first;
    ElementLinkVector<DecisionContainer> newLegs;
    bool update =  false;
    // neew passing
    for (auto thecomb: passing_comb){
      for (auto  dEL:  thecomb){
	if (dEL.first ==  legId ){
	  ElementLink<DecisionContainer>  ELV=dEL.second;
	  ATH_MSG_DEBUG(HLT::Identifier(dEL.first) <<"  "<<
			ELV.dataID() << " , " << ELV.index());
	  newLegs.push_back(ELV);
	  update=true;
	}      
      }
      // only update those concerning this tool ID
      if  (update) leg.second=newLegs;
    }
  }
    
 
 
  // how to remove duplicates?
  return;
}



StatusCode ComboHypoToolBase::printDebugInformation(const LegDecisionsMap & passingLegs) const {
  ATH_MSG_DEBUG("ComboHypoToolBase: Passing elements are: ");
  for  (auto id: passingLegs){
    ATH_MSG_DEBUG(" --- "<<HLT::Identifier(id.first)<<" with "<<id.second.size()<<" elements");
    for (auto dEL: id.second){
      ATH_MSG_DEBUG( "       "<<dEL.dataID() << " , " << dEL.index());
    }
  }
  return StatusCode::SUCCESS;
}


void ComboHypoToolBase::recursive_combine( const std::vector<std::vector<LegDecision>> & all, std::vector<std::vector<LegDecision>> & tocombine, std::vector<LegDecision> &local, u_int lindex) const
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
