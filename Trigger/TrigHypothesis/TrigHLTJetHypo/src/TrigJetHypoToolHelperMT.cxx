/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./TrigJetHypoToolHelperMT.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "./groupsMatcherFactoryMT.h"
#include "./JetTrigTimer.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJet.h"  // TLorentzVec
#include "./nodeIDPrinter.h"
#include "./DebugInfoCollector.h"
#include <algorithm>
#include <sstream>

TrigJetHypoToolHelperMT::TrigJetHypoToolHelperMT(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  base_class(type, name, parent){
}

StatusCode TrigJetHypoToolHelperMT::initialize() {

  auto conditions = m_config->getConditions();
  m_grouper  = std::move(m_config->getJetGrouper());
  m_matcher = std::move(groupsMatcherFactoryMT(conditions));
  // std::string s = toString();

  return StatusCode::SUCCESS;
}

void
TrigJetHypoToolHelperMT::collectData(const std::string& exetime,
                                     const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
                                     bool pass) const {
  if(!collector){return;}
  auto helperInfo = nodeIDPrinter(name(),
                                  m_nodeID,
                                  m_parentNodeID,
                                  pass,
                                  exetime
                                  );
  
  collector->collect(name(), helperInfo);
}
      

bool
TrigJetHypoToolHelperMT::pass(HypoJetVector& jets,
			      const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {


  JetTrigTimer timer;
  timer.start();

  if(jets.empty()){   
    timer.stop();
    bool pass = false;
    collectData(timer.readAndReset(), collector, pass);
    return pass;
  }

  HypoJetIter begin = jets.begin(); 
  HypoJetIter end = jets.end(); 
  for(auto cleaner: m_cleaners){
    end = std::partition(begin,
                         end,
                         [cleaner](const pHypoJet& j){
                           return cleaner->select(j);}
                         );
  }

  auto jetGroups = m_grouper->group(begin, end);
  bool pass = m_matcher->match(jetGroups.begin(), jetGroups.end(), collector);
  timer.stop();

  collectData(timer.readAndReset(), collector, pass);
  return pass;
}
  
std::string TrigJetHypoToolHelperMT::toString() const {

  

  std::stringstream ss;
  ss << nodeIDPrinter(name(),
                      m_nodeID,
                      m_parentNodeID);
  
  
  ss << " Cleaners [" << m_cleaners.size() << "]: \n";

  for(auto cleaner : m_cleaners) {
    ss << cleaner->toString() 
       << '\n';
  }
    
  ss << "\n Grouper: " << m_grouper->toString() << '\n';

  ss << "\n Matcher: \n";
  ss << m_matcher -> toString();

  return ss.str();
}


StatusCode
TrigJetHypoToolHelperMT::getDescription(ITrigJetHypoInfoCollector& c) const {
  c.collect(name(), toString());
  return StatusCode::SUCCESS;
}

