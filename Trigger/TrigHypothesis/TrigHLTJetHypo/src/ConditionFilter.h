/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_CONDITIONFILTER_H
#define TRIGHLTJETHYPO_CONDITIONFILTER_H

#include "./CapacityCheckedConditionsDefs.h"

class ConditionFilter {
 public:
  ConditionFilter(ConditionPtrs&);

  // find the subset of jets which satisfy a sequence of conditions
  HypoJetVector filter (const HypoJetCIter& b,
			const HypoJetCIter& e,
			const std::unique_ptr<ITrigJetHypoInfoCollector>&
			) const;
  
  std::string toString() const;  
 private:
  ConditionPtrs m_conditions;

  
};

#endif
