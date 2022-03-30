/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./TrigJetHypoToolHelperNoGrouper.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "./xAODJetCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJet.h"  // TLorentzVec
#include "./nodeIDPrinter.h"
#include "./DebugInfoCollector.h"
#include "./CompoundCondition.h"
#include <algorithm>
#include <sstream>

TrigJetHypoToolHelperNoGrouper::TrigJetHypoToolHelperNoGrouper(const std::string& type,
							       const std::string& name,
							       const IInterface* parent) :
  base_class(type, name, parent){
}

StatusCode TrigJetHypoToolHelperNoGrouper::initialize() {

  for (const auto& config : m_configs) {
    auto matcher = config->getMatcher();
    if (!matcher->valid()) {
      ATH_MSG_ERROR(matcher->msg());
      return StatusCode::FAILURE;
    }
		    
    m_matchers.push_back(std::move(matcher));
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

  // prefilters are now local variables
  std::vector<FilterPtr> prefilters{}; 
  prefilters.reserve(m_prefilterMakers.size());
  for (const auto& pf_maker : m_prefilterMakers){
    prefilters.push_back(pf_maker->getHypoJetVectorFilter());
  }

  auto hjv = jetsIn;
  for (const auto& pf : prefilters) {
    hjv = pf->filter(hjv, collector);
  }
  
  // see if matchers pass. Each matcher conatains a FastReducer tree.
  // if  > matcher, this means the conditions of different trees may
  // share jets.
  bool pass = true;
  for (const auto& matcher : m_matchers){
    auto matcher_pass = matcher->match(hjv,
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



  std::vector<FilterPtr> prefilters{};
  prefilters.reserve(m_prefilterMakers.size());
  for (const auto& pf_maker : m_prefilterMakers){
    prefilters.push_back(pf_maker->getHypoJetVectorFilter());
  }


  ss << "prefilters: [" << prefilters.size() << "]:\n";
  for (const auto& pf : prefilters){
    ss << '\n'<<  *pf;
  }
  
  ss << '\n';
  
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



