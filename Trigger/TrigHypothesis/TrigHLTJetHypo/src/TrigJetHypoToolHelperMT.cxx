/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./TrigJetHypoToolHelperMT.h"
#include "./ITrigJetHypoHelperVisitor.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/groupsMatcherFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJet.h"  // TLorentzVec
#include "./nodeIDPrinter.h"
#include <algorithm>
#include <sstream>

TrigJetHypoToolHelperMT::TrigJetHypoToolHelperMT(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  base_class(type, name, parent),
  m_timer(std::make_unique<JetTrigTimer>()){
  }

StatusCode TrigJetHypoToolHelperMT::initialize() {
  /*
    const std::vector<CleanerBridge>& 
    cleaners,
    std::unique_ptr<IJetGrouper>
    grouper,
    std::unique_ptr<IGroupsMatcher> 
    matcher) :
    m_grouper(std::move(grouper)), 
    m_matcher(std::move(matcher)), 
    em_cleaners(cleaners){
  */

  m_conditions = m_config->getConditions();
  
  for(const auto& condition: m_conditions){
    ATH_MSG_DEBUG(condition.toString());
  }

  // create a matcher and a grouper object to ensure debuging
  // tools will not cause a crash
  // The instances are renewd each event in pass() method provided this is
  // called.
  m_matcher = groupsMatcherFactory(m_conditions);
  m_grouper = m_config->getJetGrouper();  // attribute for debug

  return StatusCode::SUCCESS;
}

bool TrigJetHypoToolHelperMT::pass(HypoJetVector& jets) {
  m_timer->start();
  if(jets.empty()){
    m_timer->stop();
    m_timer->reset();
    m_timer->stop();
    return false;
  }

  m_matcher = groupsMatcherFactory(m_conditions);
  m_grouper = m_config->getJetGrouper();  // attribute for debug
    
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
  m_matcher->match(jetGroups.begin(), jetGroups.end());
  m_pass = m_matcher->pass();

  m_timer->stop();
  return m_pass;
}
  
std::string TrigJetHypoToolHelperMT::toString() const {

  

  std::stringstream ss;
  ss << nodeIDPrinter(name(),
                      m_nodeID,
                      m_parentNodeID,
                      m_pass,
                      m_timer->readAndReset()
                      );
  
  
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

void TrigJetHypoToolHelperMT::accept(ITrigJetHypoHelperVisitor& v){
  v.visit(this);
}


void TrigJetHypoToolHelperMT::resetHistory() {
  for(auto& c : m_conditions){c.resetHistory();}
}

std::string TrigJetHypoToolHelperMT::toStringAndResetHistory() {
  auto result = toString();
  resetHistory();
  return result;
}



