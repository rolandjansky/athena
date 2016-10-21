/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoHelper.h"
#include <algorithm>
#include <sstream>

TrigHLTJetHypoHelper::TrigHLTJetHypoHelper(const std::vector<CleanerBridge>& 
                                           cleaners,
                                           std::shared_ptr<IJetGrouper>& 
                                           grouper,
                                           std::unique_ptr<IGroupsMatcher> 
                                           matcher) :
  m_grouper(grouper), m_matcher(std::move(matcher)), m_cleaners(cleaners){
}

bool TrigHLTJetHypoHelper::pass(HypoJetVector& jets){
  
  HypoJetIter begin = jets.begin(); 
  HypoJetIter end = jets.end(); 
  for(auto cleaner: m_cleaners){end = std::partition(begin, end, cleaner);}

  auto jetGroups = m_grouper->group(begin, end);

  m_matcher->match(jetGroups.begin(), jetGroups.end());

  return m_matcher->pass();

}

HypoJetVector TrigHLTJetHypoHelper::passedJets() const noexcept{
  return m_matcher->passedJets();
}

HypoJetVector TrigHLTJetHypoHelper::failedJets() const noexcept{
  return m_matcher->failedJets();
}

Conditions TrigHLTJetHypoHelper::getConditions() const noexcept{
  return m_matcher->getConditions();
}
    
  
std::string TrigHLTJetHypoHelper::toString() const {

  std::stringstream ss;

  ss << "TrigHLTJetHypo Helper"
     << " Cleaners ";
  for(auto cleaner : m_cleaners) {
    ss << cleaner.toString() 
       << '\n';
  }

  ss << "\n Grouper: " << m_grouper->toString() << '\n';

  ss << "\n Matcher: \n";
  ss << m_matcher -> toString();

  return ss.str();
}
