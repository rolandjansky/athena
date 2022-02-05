/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DecisionHandling/ComboHypoToolBase.h"
#include "TrigCompositeUtils/Combinators.h"
using namespace TrigCompositeUtils;


ComboHypoToolBase::ComboHypoToolBase(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent),
  m_decisionId(HLT::Identifier::fromToolName(name)), m_legMultiplicities()
{}


StatusCode ComboHypoToolBase::setLegMultiplicity(const Combo::MultiplicityReqMap& multiplicityRequiredMap) {
  const std::string nameOfToolsChain = m_decisionId.name();
  const Combo::MultiplicityReqMap::const_iterator it = multiplicityRequiredMap.find(nameOfToolsChain);

  if (it == multiplicityRequiredMap.end()) {
    ATH_MSG_ERROR("ComboHypoTool for " << m_decisionId << " could not find its required multiplcity data in the map supplied by its parent alg.");
    return StatusCode::FAILURE;
  }

  m_legMultiplicities = it->second;
  if (m_legMultiplicities.size() == 0) {
    ATH_MSG_ERROR("ComboHypoTool for " << m_decisionId << " was listed in the supplied multiplicityRequiredMap, but data was not supplied for any legs.");
    return StatusCode::FAILURE;
  }

  m_legDecisionIds.clear();
  for (size_t i = 0; i < m_legMultiplicities.size(); ++i) {
    if(m_legMultiplicities.at(i) <= 0) {
      ATH_MSG_ERROR("ComboHypoTool for " << m_decisionId << " has been configured with an impossible multiplicity requirement of " << m_legMultiplicities.at(i) << " on leg " << i);
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("ComboHypoTool for " << m_decisionId << " will require multiplicity " << m_legMultiplicities.at(i) << " on leg " << i);
    if (m_legMultiplicities.size() > 1) { // We only have per-leg IDs when there is more than one leg
      m_legDecisionIds.push_back(createLegName(m_decisionId, i));
    } else {  // For one leg, just repete the chain's ID
      m_legDecisionIds.push_back(m_decisionId);
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode ComboHypoToolBase::decide(Combo::LegDecisionsMap& passingLegs, const EventContext& /*context*/) const {
  if (m_legMultiplicities.size() == 0) {
    ATH_MSG_ERROR("ComboHypoTool for " << m_decisionId << " has not been properly configured. setLegMultiplicity should be called by the parent alg in initalize");
    return StatusCode::FAILURE;
  }

  // if no combinations passed, then exit 
  if (passingLegs.size() == 0) {
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG("Looking for legs from " << decisionId() << " in the map. Map contains features for " << passingLegs.size() << " legs, which may be data for many chains.");

  // select the leg decisions from the map with this ID:
  std::vector<std::vector<Combo::LegDecision>> legDecisions;
  ATH_CHECK(selectLegs(passingLegs, legDecisions));

  for (size_t legIndex = 0; legIndex < m_legMultiplicities.size(); ++legIndex) {
    // The static cast is because this gets read as a Gaudi property which only seems to work with vector<int> rather than vector<size_t> 
    if (legDecisions.at(legIndex).size() < static_cast<size_t>(m_legMultiplicities.at(legIndex))) {
      ATH_MSG_DEBUG("Too few features are found for " << m_decisionId << " on leg " << legIndex <<", require:" << m_legMultiplicities.at(legIndex) << " have:" << legDecisions.at(legIndex).size());
      ATH_MSG_DEBUG("This ComboHypoTool cannot run in this event, this chain **REJECTS** this event.");
      eraseFromLegDecisionsMap(passingLegs);
      ATH_CHECK(printDebugInformation(passingLegs));
      return StatusCode::SUCCESS;
    }
  }

  // Create and initialise the combinations generator for the requirements of this chain, given the objects available in this event.
  HLT::NestedUniqueCombinationGenerator nucg;
  for (size_t legIndex = 0; legIndex < m_legMultiplicities.size(); ++legIndex) {
    const size_t choose_any = m_legMultiplicities.at(legIndex);
    const size_t out_of = legDecisions.at(legIndex).size();
    nucg.add({out_of, choose_any});
    ATH_MSG_DEBUG("For leg " << legIndex << " we will be choosing any " << choose_any << " Decision Objects out of " << out_of);
  }

  std::vector<std::vector<Combo::LegDecision>> passingCombinations;

  size_t warnings = 0, iterations = 0;
  do {

    const std::vector<size_t> combination = nucg();
    ++nucg;
    std::vector<Combo::LegDecision> combinationToCheck;

    size_t location_in_combination = 0;
    for (size_t legIndex = 0; legIndex < m_legMultiplicities.size(); ++legIndex) {
      // We loop over however many objects are required on the leg,
      // but we take their index from the 'combination'. Hence 'object' is not used directly
      for (size_t object = 0; object < static_cast<size_t>(m_legMultiplicities.at(legIndex)); ++object) {
        const size_t objectIndex = combination.at(location_in_combination++);
        combinationToCheck.push_back( legDecisions.at(legIndex).at(objectIndex) );
      }
    }

    ++iterations;

    try {
      if (executeAlg(combinationToCheck)) {
        ATH_MSG_DEBUG("Combination " << (iterations - 1) << " decided to be passing");
        passingCombinations.push_back(combinationToCheck);
        if (m_modeOR == true and m_enableOverride) {
          break;
        }
      } else { // the combination failed
        if (m_modeOR == false and m_enableOverride) {
          break;
        }
      }
    } catch (std::exception& e) {
      ATH_MSG_ERROR(e.what());
      return StatusCode::FAILURE;
    }

    if ((iterations >= m_combinationsThresholdWarn && warnings == 0) or (iterations >= m_combinationsThresholdBreak)) {
      ATH_MSG_WARNING("Have so far processed " << iterations << " combinations for " << m_decisionId << " in this event, " << passingCombinations.size() << " passing.");
      ++warnings;
      if (iterations >= m_combinationsThresholdBreak) {
        ATH_MSG_WARNING("Too many combinations! Breaking the loop at this point.");
        break;
      }
    }

  } while (nucg);


  if (m_modeOR) {

    ATH_MSG_DEBUG("Passing " << passingCombinations.size() << " combinations out of " << iterations << ", " 
      << m_decisionId << (passingCombinations.size() ? " **ACCEPTS**" : " **REJECTS**") << " this event based on OR logic.");

    if (m_enableOverride) {
      ATH_MSG_DEBUG("Note: stopped after the first successful combination due to the EnableOverride flag.");  
    }

  } else {  // modeAND

    const bool passAll = (passingCombinations.size() == iterations);

    ATH_MSG_DEBUG("Passing " << passingCombinations.size() << " combinations out of " << iterations << ", " 
      << m_decisionId << (passAll ? " **ACCEPTS**" : " **REJECTS**") << " this event based on AND logic.");

    if (m_enableOverride) {
      ATH_MSG_DEBUG("Note: stopped after the first failed combination due to the EnableOverride flag.");  
    }

    if (not passAll) {
      passingCombinations.clear();
    }

  }

  if (not passingCombinations.empty()) { // need partial erasure of the decsions (only those not present in any combination)
    updateLegDecisionsMap(passingCombinations, passingLegs);
  } else { // need complete erasure of input decisions
    eraseFromLegDecisionsMap(passingLegs);
  }

  ATH_CHECK(printDebugInformation(passingLegs));
  return StatusCode::SUCCESS;
}


StatusCode ComboHypoToolBase::selectLegs(const Combo::LegDecisionsMap& IDCombMap, std::vector<std::vector<Combo::LegDecision>>& legDecisions) const
{
  /*
    legDecisions: nested vector like:
    [(leg0, el0), (leg0, el1), (leg0, el2), (leg0, el3)], <-- All leg0 objects are in legDecisions[0][X]
    [(leg1, mu0), (leg1, mu1)] <-- All leg1 objects are in legDecisions[1][X]
    We keep the legID in the std::pair inside the inner vector as these pairs will be flattened into a single vector
    when individual combinations of objects are passed to executeAlg, pair.first then contains the leg, with pair.second containing the Decision Object.
  */

  // Extract from IDCombMap the features for this chain. Wrap the features up in a pair along with their leg ID
  for (size_t legIndex = 0; legIndex < m_legMultiplicities.size(); ++ legIndex) {

    // If the chain has more than one leg, then we have per-leg IDs. Otherwise we just use the chain's ID
    const HLT::Identifier& legIdentifier = (m_legMultiplicities.size() > 1 ? m_legDecisionIds.at(legIndex) : m_decisionId);

    // Combo::LegDecision is a pair of <DecisionID, ElementLink<Decision>>
    std::vector<Combo::LegDecision> decisionObjectsOnLeg;

    // Find physics objects on this leg. May be zero.
    const Combo::LegDecisionsMap::const_iterator it = IDCombMap.find(legIdentifier.numeric());

    if (it != IDCombMap.end()) {
      for (const ElementLink<DecisionContainer>& el : it->second) {
        decisionObjectsOnLeg.emplace_back(legIdentifier, el);
      }
    }

    legDecisions.push_back(std::move(decisionObjectsOnLeg));
  }

  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG("Getting " << legDecisions.size() << " legs to combine, for ID: " << decisionId());
    size_t count = 0;
    for (const auto& leg : legDecisions) {
      ATH_MSG_DEBUG("Leg " << count++ << " --");
      for (const auto& dEL : leg) {
        ATH_MSG_DEBUG("-- " << HLT::Identifier(dEL.first) << " container:" << dEL.second.dataID() << ", index:" << dEL.second.index());
      }
    }
  }
  return StatusCode::SUCCESS;
}

void ComboHypoToolBase::updateLegDecisionsMap(const std::vector<std::vector<Combo::LegDecision>>& passingComb, Combo::LegDecisionsMap& passingLegs) const {
  if (msgLvl(MSG::DEBUG)) {
    size_t count = 0;
    for (const std::vector<Combo::LegDecision>& comb : passingComb) {
      ATH_MSG_DEBUG("-- Passing combination " << count++ << " of " << passingComb.size());
      for (const auto& [id, EL] : comb) {
        ATH_MSG_DEBUG("-- -- " << HLT::Identifier(id) << " container:" << EL.dataID() << ", index:" << EL.index());
      }
    }
  }

  // remove combinations that didn't pass from the final map passingLegs
  for (auto& it : passingLegs) {
    DecisionID legId = it.first;
    if (not(legId == m_decisionId or (isLegId(legId) and getIDFromLeg(legId) == m_decisionId))) {
      continue; // Some other chain, ignore it to get faster execution.
    } 
    std::vector<ElementLink<DecisionContainer>> updatedDecisionObjectsOnLeg;
    bool update = false;
    // Loop over all passing combinations, and all Decision Objects in each passing combination. Find Decision Objects on this leg.
    for (const std::vector<Combo::LegDecision>& comb : passingComb) {
      for (const auto& [id, EL] : comb) {
        // Check that this Decision Object is on the correct leg, and that we haven't collated it already from another combination.
        if (id == legId and std::find(updatedDecisionObjectsOnLeg.begin(), updatedDecisionObjectsOnLeg.end(), EL) == updatedDecisionObjectsOnLeg.end()) {
          ATH_MSG_VERBOSE("Keeping on leg " << HLT::Identifier(id) << " the Decision Object container:" << EL.dataID() << ", index:" << EL.index());
          updatedDecisionObjectsOnLeg.push_back(EL);
          update = true;
        }
      }
    }
    // only update those concerning this tool ID
    if (update){ 
      it.second = updatedDecisionObjectsOnLeg;
    }
  }
}

void ComboHypoToolBase::eraseFromLegDecisionsMap(Combo::LegDecisionsMap& passingLegs) const {
  for (auto& it : passingLegs) {
    DecisionID id = it.first;
    if (id == m_decisionId or (isLegId(id) and getIDFromLeg(id) == m_decisionId)) {
      const size_t nDecisionObjects = it.second.size();
      it.second.clear();
      ATH_MSG_VERBOSE("-- Removed " << nDecisionObjects << " from " << id);
    }
  }
}

StatusCode ComboHypoToolBase::printDebugInformation(const Combo::LegDecisionsMap& passingLegs) const {
  ATH_MSG_DEBUG("ComboHypoToolBase: End of " << m_decisionId << ", passing elements are: ");
  for (const auto& [id, ELV] : passingLegs) {
    // Only print for this chain
    if (id == m_decisionId or (isLegId(id) and m_decisionId == getIDFromLeg(id))) { 
      ATH_MSG_DEBUG("-- " << HLT::Identifier(id) << " with " << ELV.size() << " elements");
      for (const auto& EL : ELV) {
        ATH_MSG_DEBUG("-- -- container:" << EL.dataID() << ", index:" << EL.index());
      }
    }
  }
  return StatusCode::SUCCESS;
}


bool ComboHypoToolBase::executeAlg(const std::vector<Combo::LegDecision>& /*combination*/) const {
  ATH_MSG_ERROR("Do not use ComboHypoToolBase on its own, inherit this class and override executeAlg.");
  return false;
}

StatusCode ComboHypoToolBase::decideOnSingleObject(Decision*, const std::vector<const TrigCompositeUtils::DecisionIDContainer*>&) const {
  ATH_MSG_ERROR("Do not use ComboHypoToolBase on its own, inherit this class and override decideOnSingleObject.");
  ATH_MSG_ERROR("NOTE: Only if you are also supplying your own decide(...) implimentation, or similar.");
  ATH_MSG_ERROR("NOTE: Most uses cases should only need to override executeAlg(...).");
  return StatusCode::FAILURE;
}
