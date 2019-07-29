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
#include "./xAODJetCollector.h"
#include "./DebugInfoCollector.h"
#include <sstream>
#include <algorithm>

SingleConditionMatcherMT::SingleConditionMatcherMT(std::unique_ptr<IConditionMT>&& cb):
  m_condition(std::move(cb)){
}
  
std::optional<bool>
SingleConditionMatcherMT::match(const HypoJetGroupCIter& jets_b,
                                const HypoJetGroupCIter& jets_e,
                                xAODJetCollector& jetCollector,
                                const std::unique_ptr<ITrigJetHypoInfoCollector>& v,
				bool ) const {


  for(auto i=jets_b; i != jets_e; ++i){
    if (m_condition->isSatisfied(*i, v)){
      jetCollector.addJets((*i).cbegin(), (*i).cend());
      return std::make_optional<bool>(true);
    }
  }
  return  std::optional<bool>(false);
}



std::string SingleConditionMatcherMT::toString() const noexcept {
  std::stringstream ss;

  ss << "SingleConditionMatcherMT/" << '\n';
  ss << "Condition:\n";

  ss << m_condition->toString() << '\n';

  
  return ss.str();
}


