/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     SingleConditionMatcherMT.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include "./SingleConditionMatcherMT.h"
#include <sstream>
#include <algorithm>

SingleConditionMatcherMT::SingleConditionMatcherMT(const ConditionBridgeMT& cb):
  m_condition(cb){
}
  
bool SingleConditionMatcherMT::match(const HypoJetGroupCIter& jets_b,
                                     const HypoJetGroupCIter& jets_e,
                                     IConditionVisitor* v) {

  
  for(auto i=jets_b; i != jets_e; ++i){
    if (m_condition.isSatisfied(*i, v)){
      return true;
    }
  }
  return false;
}



std::string SingleConditionMatcherMT::toString() const noexcept {
  std::stringstream ss;

  ss << "SingleConditionMatcherMT/" << '\n';
  ss << "Condition:\n";

  ss << m_condition.toString() << '\n';

  
  return ss.str();
}


ConditionsMT SingleConditionMatcherMT::getConditions() const noexcept {
  ConditionsMT c {m_condition};
  return c;
}


