/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     SingleConditionMatcher.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SingleConditionMatcher.h"
#include <sstream>
#include <algorithm>

SingleConditionMatcher::SingleConditionMatcher(const ConditionBridge& cb):
  GroupsMatcher("SingleConditionMatcher"), m_condition(cb), m_pass(false){
}
  
void SingleConditionMatcher::match(const HypoJetGroupCIter& jets_b,
				    const HypoJetGroupCIter& jets_e){
  m_passed_jets.clear();
  m_failed_jets.clear();

  auto last = std::find_if(jets_b, jets_e, m_condition);
  if (last == jets_e){
    m_pass = false;
    addToFailedJets(jets_b, jets_e);
  } else {
    m_pass = true;
    addToPassedJets(last);
    addToFailedJets(jets_b, last);
    addToFailedJets(last, jets_e);
  }
}


bool SingleConditionMatcher::pass() const {return m_pass;}


HypoJetVector SingleConditionMatcher::passedJets() const noexcept {
return m_passed_jets;
}


HypoJetVector SingleConditionMatcher::failedJets() const noexcept {
return m_failed_jets;
}


std::string SingleConditionMatcher::toString() const noexcept {
  std::stringstream ss;

  ss << "SingleConditionMatcher/" << GroupsMatcher::toString() << '\n';
  ss << "Condition:\n";

  ss << m_condition.toString() << '\n';

  ss << "Passed jets:[" << m_passed_jets.size() <<"]\n";
  for(auto j : m_passed_jets){ss << j->toString() << '\n';}
  
  ss << "Failed jets:[" << m_failed_jets.size() <<"]\n";
  for(auto j : m_failed_jets){ss <<  j->toString() << '\n';}
  
  return ss.str();
}


Conditions SingleConditionMatcher::getConditions() const noexcept {
  Conditions c {m_condition};
  return c;
}



void  
SingleConditionMatcher::addToFailedJets(HypoJetGroupCIter begin,
                                        HypoJetGroupCIter end){
  // copy the jets residing in a vector of vectors of jets into the failed jet
  // vector
  for(;begin != end; ++begin){
    m_failed_jets.insert(m_failed_jets.end(), (*begin).begin(), (*begin).end());
  }
}

void  
SingleConditionMatcher::addToPassedJets(HypoJetGroupCIter iter){
  // copy the jets residing in a vector of vectors of jets into the passed jet
  // vector
  m_passed_jets.insert(m_passed_jets.end(), (*iter).begin(), (*iter).end());
}
