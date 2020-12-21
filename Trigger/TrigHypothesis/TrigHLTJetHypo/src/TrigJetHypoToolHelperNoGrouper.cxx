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

  m_matcher = m_config->getMatcher();
  if(!m_matcher){
    ATH_MSG_ERROR("Error setting matcher");
    return StatusCode::FAILURE;
  }
		  
  return StatusCode::SUCCESS;
}

void
TrigJetHypoToolHelperNoGrouper::collectData(const std::string& exetime,
					    const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
					    const std::optional<bool>& pass) const {
  if(!collector){return;}
  auto helperInfo = nodeIDPrinter("TrigJetHypoToolHelperNoGrouper",
                                  m_nodeID,
                                  m_parentNodeID,
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

  HypoJetIter jets_begin = jets.begin(); 
  HypoJetIter jets_end = jets.end(); 
  for(auto cleaner: m_cleaners){
    jets_end = std::partition(jets_begin,
			      jets_end,
			      [cleaner](const pHypoJet& j){
				return cleaner->select(j);}
			      );
  }
  
  auto pass = m_matcher->match(jets_begin,
			       jets_end,
			       jetCollector,
			       collector);
  
  timer.stop();
  
  collectData(timer.readAndReset(), collector, pass);
  
  if(!pass.has_value()){
    ATH_MSG_ERROR("Matcher cannot determine result. Config error?");
    return false;
  }
    
  return *pass;
}

std::string TrigJetHypoToolHelperNoGrouper::toString() const {
  
  
  std::stringstream ss;
  ss << nodeIDPrinter(name(),
                      m_nodeID,
                      m_parentNodeID);
  
  
  ss << "Cleaners:\n No of cleaners: "  << m_cleaners.size() << '\n';
  
  for(auto cleaner : m_cleaners) {
    ss << cleaner->toString() 
       << '\n';
  }
  
  ss << "\nMatcher:\n";
  ss << m_matcher -> toString();
  
  return ss.str();
}


StatusCode
TrigJetHypoToolHelperNoGrouper::getDescription(ITrigJetHypoInfoCollector& c) const {
  c.collect(name(), toString());
  return StatusCode::SUCCESS;
}


std::size_t TrigJetHypoToolHelperNoGrouper::requiresNJets() const {
  return m_config->requiresNJets();
}




