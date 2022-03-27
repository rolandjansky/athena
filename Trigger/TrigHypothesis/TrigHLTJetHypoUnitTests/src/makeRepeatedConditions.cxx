/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./makeRepeatedConditions.h"
#include "TrigHLTJetHypo/../src/EtaConditionAbs.h"
#include "TrigHLTJetHypo/../src/RepeatedCondition.h"

#include <memory>

ConditionPtrs makeRepeatedConditions(const EtaPair& eta){

  ConditionPtrs conditions;

  auto el_condition = std::unique_ptr<ICondition>(nullptr);
  el_condition.reset(new EtaConditionAbs(eta.first, eta.second));
      
  auto rep_condition = std::unique_ptr<IRepeatedCondition>(nullptr);
  
  std::size_t multiplicity{1u};
  int clique{0};
  rep_condition.reset(new RepeatedCondition(std::move(el_condition),
					    multiplicity,
					    clique));
  
  conditions.push_back(std::move(rep_condition));

  return conditions;
}
  

ConditionPtrs makeRepeatedConditions(const EtaPairs& etas){

  ConditionPtrs conditions;

  for (const auto& ep : etas){
    auto el_condition = std::unique_ptr<ICondition>(nullptr);
    el_condition.reset(new EtaConditionAbs(ep.first, ep.second));
      
    auto rep_condition = std::unique_ptr<IRepeatedCondition>(nullptr);

    std::size_t multiplicity{1u};
    int clique{0};
    rep_condition.reset(new RepeatedCondition(std::move(el_condition),
					      multiplicity,
					      clique));
    
    conditions.push_back(std::move(rep_condition));
  }
  
  return conditions;
}
