/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./TrigJetHypoToolHelperNoGrouper.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "./xAODJetCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJet.h"  // TLorentzVec
#include "./nodeIDPrinter.h"
#include "./DebugInfoCollector.h"
#include "./ConditionInverter.h"
#include "./CompoundConditionMT.h"
#include <algorithm>
#include <sstream>

TrigJetHypoToolHelperNoGrouper::TrigJetHypoToolHelperNoGrouper(const std::string& type,
							       const std::string& name,
							       const IInterface* parent) :
  base_class(type, name, parent){
}

StatusCode TrigJetHypoToolHelperNoGrouper::initialize() {

  CHECK(makePrefilter());
  for (const auto& config : m_configs) {
    m_matchers.push_back(config->getMatcher());
  }
		  
  return StatusCode::SUCCESS;
}

bool
TrigJetHypoToolHelperNoGrouper::pass(HypoJetVector& jetsIn,
				     xAODJetCollector& jetCollector,
				     const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {
  
  if(collector){
    std::stringstream ss;
    ss <<  "No of jets " + std::to_string(jetsIn.size()) + '\n';
    ss << jetsIn; 
    collector->collect(name(), ss.str());
  }

  if(jetsIn.empty()){
    if (collector){
      collector->collect(name(), "empty input jet collection");
    }
    bool pass = false;
    return pass;
  }

  // prefiltering.

  auto jets = m_prefilter.filter(jetsIn, collector);
  
  HypoJetIter jets_begin = jets.begin(); 
  HypoJetIter jets_end = jets.end(); 
  
  // see if matchers pass. Each matcher conatains a FastReducer tree.
  // if  > matcher, this means the conditions of different trees may
  // share jets.
  bool pass = true;
  for (const auto& matcher : m_matchers){
    auto matcher_pass = matcher->match(jets_begin,
				       jets_end,
				       jetCollector,
				       collector);
    if (!matcher_pass.has_value()) {
      ATH_MSG_ERROR("Matcher cannot determine result. Config error?");
      return false;
    }
    
    if (!(*matcher_pass)){
      pass = false;
      break;
    }
  }
  
  return pass;
}

std::string TrigJetHypoToolHelperNoGrouper::toString() const {
  
  std::stringstream ss;
  ss << name();
  
  ss << "prefilter:\n " << m_prefilter << '\n';
  
  ss << "\nMatchers [" << m_matchers.size() << "]:\n\n";
  unsigned int imatcher{0};
  for (const auto & matcher : m_matchers) {
    ss << "matcher " << imatcher++ << '\n';
    ss << matcher -> toString();
  }
  
  return ss.str();
}


StatusCode
TrigJetHypoToolHelperNoGrouper::getDescription(ITrigJetHypoInfoCollector& c) const {
  c.collect(name(), toString());
  return StatusCode::SUCCESS;
}


std::size_t TrigJetHypoToolHelperNoGrouper::requiresNJets() const {
  return m_configs[0]->requiresNJets();
}

StatusCode TrigJetHypoToolHelperNoGrouper::makePrefilter(){
  /* set up the prefilter by looping over the precondition 
     Condition maker AlgTools to obtain the elemental Conditions,
     place these in a single compound Condition, and warp this in a
     CondtionInverter. This is passed to the ConditionFilter object.
  */

  // if no conditions the filter will apply n inverter to an empty
  // Compound Condition, which will kill all events.
  if (m_prefilterConditionMakers.empty()) {return StatusCode::SUCCESS;}
  
  auto makeElementalFilterCondition = [](auto& conditionMaker)->ConditionMT {
    return conditionMaker->getRepeatedCondition();
  };

  // fill a container with pointers to an elemental condition
  // note: IRepeatedCondition derives from IConditionMT
  ConditionsMT prefilterConditions{};
  std::transform(m_prefilterConditionMakers.begin(),
		 m_prefilterConditionMakers.end(),
		 std::back_inserter(prefilterConditions),
		 makeElementalFilterCondition);

  // create a compound condition pointer.
  auto cc = std::make_unique<CompoundConditionMT>(prefilterConditions);

  // create a conditonsMT vec, add the inversuion of the compound condition
  // to it. With the inversion, the invert compound condition acts as veto
  ConditionsMT condVec;
  condVec.push_back(std::make_unique<ConditionInverterMT>(std::move(cc)));
  
  // create an filter from the vector containing the inverted condition.
  m_prefilter = ConditionFilter{condVec};
  
  return StatusCode::SUCCESS;
}

