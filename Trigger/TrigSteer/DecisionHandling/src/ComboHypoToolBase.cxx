/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DecisionHandling/ComboHypoToolBase.h"
using namespace TrigCompositeUtils;


ComboHypoToolBase::ComboHypoToolBase(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent),
  m_decisionId(HLT::Identifier::fromToolName(name))
{}


StatusCode ComboHypoToolBase::decide(LegDecisionsMap& passingLegs, const EventContext& /* context */) const
{
  // if no combinations passed, then exit 
  if (passingLegs.size() == 0)  return StatusCode::SUCCESS;
  ATH_MSG_DEBUG("Looking for " << decisionId() << " in the map. Map contains " << passingLegs.size() << " legs");
  //ATH_CHECK( printDebugInformation(passingLegs));
  // select the leg decisions from  the map with this ID:
  std::vector<std::vector<LegDecision>> legDecisions;
  ATH_CHECK(selectLegs(passingLegs, legDecisions));
  if (legDecisions.size() == 0) {
    ATH_MSG_DEBUG("Found 0 legs with this DecisionID: something failed?");
    return StatusCode::SUCCESS;
  }
  size_t nLegs = legDecisions.size();
  // create the combinations between the legs:
  // next: TODO with Combinators.h, to crate one combination  at a time
  std::vector<std::vector<LegDecision>> combinations;
  createCombinations(legDecisions, combinations, nLegs, 2); // do we need to pass nLegs?
  std::vector<std::vector<LegDecision>> passingCombinations;
  // do the actual algorithm and select the decisions that passed
  for (std::vector<LegDecision>& thecomb : combinations) {
    // to add: protection when the two decisions are the same object
    bool pass = executeAlg(thecomb);
    if (pass) {
      ATH_MSG_DEBUG("Combination decided to be passing");
      passingCombinations.push_back(thecomb);
    }
  }

  if (not passingCombinations.empty()) {// need partial erasure of the decsions (only those not present in any combination)
    updateLegDecisionsMap(passingCombinations, passingLegs);
  }
  else { // need complete erasure of input decisions
    eraseFromLegDecisionsMap(passingLegs);
  }

  ATH_MSG_DEBUG("End of Tool  -----");
  ATH_CHECK(printDebugInformation(passingLegs));
  //ATH_CHECK( createOutput( passingLegs, context ));
  return StatusCode::SUCCESS;

}


StatusCode ComboHypoToolBase::selectLegs(const LegDecisionsMap& IDCombMap, std::vector<std::vector<LegDecision>>& legDecisions) const
{
  /*
    legDecisions: vector with vectors like:
    [(leg0, el0), (leg0, el1)],
    [(leg1, mu0), (leg1, mu1)]
  */

  ElementLinkVector<TrigCompositeUtils::DecisionContainer> comb;
  // collect combinations from all the legs, searching both chain name and leg name
  for (auto id : IDCombMap) {
    // get the Element links from the chainID (from all the legs)
    HLT::Identifier chainId = 0;
    if (TrigCompositeUtils::isLegId(id.first))
      chainId = TrigCompositeUtils::getIDFromLeg(id.first);
    else
      chainId = id.first;

    if (chainId != decisionId()) continue;
    comb = id.second;
    std::vector<LegDecision> oneleg;
    for (auto el : comb) {
      auto newel = std::make_pair(id.first, el);
      oneleg.push_back(newel);
    }
    legDecisions.push_back(oneleg);
  }

  size_t nLegs = legDecisions.size(); // the legs for this chain only
  if (nLegs == 0) {
    ATH_MSG_DEBUG("There are no decisions in the legs to combine for ID " << decisionId());
    return StatusCode::SUCCESS;
  }
  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG("Getting " << nLegs << " legs to combine, for ID: " << decisionId());
    for (auto leg : legDecisions) {
      ATH_MSG_DEBUG("Leg --");
      for (auto dEL : leg) {
        ATH_MSG_DEBUG(HLT::Identifier(dEL.first)
          << " " << dEL.second.dataID() << " , " << dEL.second.index());
      }
    }
  }
  return StatusCode::SUCCESS;
}


// TODO: Include case of one leg and multiple legs with mult=1 on each leg. Need to be extended to cover cases with multiple legs qith different multiplicities (2mu_3e)
void ComboHypoToolBase::createCombinations(const std::vector<std::vector<LegDecision>>& legs, std::vector<std::vector<LegDecision>>& combinations,
                                           int nLegs, int nToGroup) const {
  if (nLegs == 1) {
    auto elements = legs[0];
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
    } while (std::prev_permutation(selector.begin(), selector.end()));
  } else {
    std::vector<LegDecision> local;
    recursive_combine(legs, combinations, local, 0);
  }

  if (msgLvl(MSG::DEBUG)) {
    for (auto comb : combinations) {
      ATH_MSG_DEBUG("Combination");
      for (auto dEL : comb) {
        ATH_MSG_DEBUG(dEL.second.dataID() << " , " << dEL.second.index());
      }
    }
  }
  return;
}

void ComboHypoToolBase::updateLegDecisionsMap(const std::vector<std::vector<LegDecision>>& passingComb, LegDecisionsMap& passingLegs) const {
  // remove combinations that didn't pass from the final map passingLegs
  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG("Update output because these combinations are passing, number of combinations: " << passingComb.size());
    for (auto combV : passingComb) {
      ATH_MSG_DEBUG("  combination of size " << combV.size() << " should be contant identical to the multiplcity ");
      for (auto [id, EL] : combV) {
        ATH_MSG_DEBUG("    obj in combination " << id << " link " << EL.dataID() << " , " << EL.index());
      }
    }
  }
  for (auto& [legId, decisionsForLeg] : passingLegs) {
    ElementLinkVector<DecisionContainer> newLegs;
    bool update = false;
    // new passing
    for (auto thecomb : passingComb) {
      for (auto dEL : thecomb) {
        if (dEL.first == legId) {
          ElementLink<DecisionContainer>  EL = dEL.second;
          ATH_MSG_DEBUG("decision object in passing combination " << HLT::Identifier(dEL.first) << "  " << EL.dataID() << " , " << EL.index());
          newLegs.push_back(EL);
          update = true;
        }
      }
      // only update those concerning this tool ID
      if (update)
        decisionsForLeg = newLegs;
    }
  }

  // how to remove duplicates?
  return;
}

void ComboHypoToolBase::eraseFromLegDecisionsMap(LegDecisionsMap& passingLegs) const {
  ATH_MSG_DEBUG("Combinations rejected, removing respective IDs");

  for (auto& [id, ELV] : passingLegs) {
    if (id == m_decisionId) {
      passingLegs[id] = {};
      ATH_MSG_DEBUG(" Removed " << id);

    }
    else if (TrigCompositeUtils::isLegId(id) and m_decisionId == TrigCompositeUtils::getIDFromLeg(id)) {
      passingLegs[id] = {};
      ATH_MSG_DEBUG(" Removed " << id);
    }
  }
}

StatusCode ComboHypoToolBase::printDebugInformation(const LegDecisionsMap& passingLegs) const {
  ATH_MSG_DEBUG("ComboHypoToolBase: Passing elements are: ");
  for (auto& [id, ELV]: passingLegs) {
    if (id == m_decisionId or (TrigCompositeUtils::isLegId(id) and m_decisionId == TrigCompositeUtils::getIDFromLeg(id)) ) { 
      ATH_MSG_DEBUG(" --- " << HLT::Identifier(id) << " with " << ELV.size() << " elements");
      for (auto EL : ELV) {
        ATH_MSG_DEBUG("       " << EL.dataID() << " , idx: " << EL.index());
      }
    }
  }
  return StatusCode::SUCCESS;
}


void ComboHypoToolBase::recursive_combine(const std::vector<std::vector<LegDecision>>& all, std::vector<std::vector<LegDecision>>& toCombine, 
                                          std::vector<LegDecision>& local, u_int lindex) const {
  for (size_t leg = lindex; leg < all.size(); leg++) {
    for (size_t i = 0; i < all[leg].size(); i++) {
      local.push_back(all[leg][i]);
      recursive_combine(all, toCombine, local, leg + 1);
      local.pop_back();
    }
  }
  if (local.size() == all.size())
    toCombine.push_back(local);

  return;

}
