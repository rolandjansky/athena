/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./TrigJetHypoToolHelperNoGrouper.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "./xAODJetCollector.h"
#include "./JetTrigTimer.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJet.h"  // TLorentzVec
#include "./nodeIDPrinter.h"
#include "./DebugInfoCollector.h"
#include <algorithm>
#include <sstream>

TrigJetHypoToolHelperNoGrouper::TrigJetHypoToolHelperNoGrouper(const std::string& type,
							       const std::string& name,
							       const IInterface* parent) :
  base_class(type, name, parent){
}

StatusCode TrigJetHypoToolHelperNoGrouper::initialize() {

  for (const auto& config : m_configs) {
    m_matchers.push_back(std::move(config->getMatcher()));
  }
		  
  return StatusCode::SUCCESS;
}

void
TrigJetHypoToolHelperNoGrouper::collectData(const std::string& exetime,
					    const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
					    const std::optional<bool>& pass) const {
  if(!collector){return;}
  auto helperInfo = nodeIDPrinter("TrigJetHypoToolHelperNoGrouper",
                                  pass,
                                  exetime
                                  );
  
  collector->collect(name(), helperInfo);
}


bool
TrigJetHypoToolHelperNoGrouper::pass(HypoJetVector& jets,
				     xAODJetCollector& jetCollector,
				     const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {
  
  if(collector){
    std::stringstream ss;
    ss <<  "No of jets " + std::to_string(jets.size()) + '\n';
    ss << jets; 
    collector->collect(name(), ss.str());
  }

  JetTrigTimer timer;
  timer.start();
  
  if(jets.empty()){   
    timer.stop();
    bool pass = false;
    collectData(timer.readAndReset(), collector, pass);
    return pass;
  }

  // jet cleaning
  HypoJetIter jets_begin = jets.begin(); 
  HypoJetIter jets_end = jets.end(); 
  for(auto cleaner: m_cleaners){
    jets_end = std::partition(jets_begin,
			      jets_end,
			      [cleaner](const pHypoJet& j){
				return cleaner->select(j);}
			      );
  }

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
  
  timer.stop();
  collectData(timer.readAndReset(), collector, pass);

  return pass;
}

std::string TrigJetHypoToolHelperNoGrouper::toString() const {
  
  
  std::stringstream ss;
  ss << name();
  
  ss << "Cleaners:\n No of cleaners: "  << m_cleaners.size() << '\n';
  
  for(auto cleaner : m_cleaners) {
    ss << cleaner->toString() 
       << '\n';
  }
  
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




