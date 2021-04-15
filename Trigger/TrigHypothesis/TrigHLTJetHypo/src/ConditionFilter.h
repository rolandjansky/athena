/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_CONDITIONFILTER_H
#define TRIGHLTJETHYPO_CONDITIONFILTER_H

#include "./IHypoJetVectorFilter.h"
#include "./RepeatedConditionsDefs.h"
#include <ostream>

class ConditionFilter: public IHypoJetVectorFilter  {
 public:

  ConditionFilter(){};

  ConditionFilter(ConditionPtrs&&);

  // find the subset of jets which satisfy a sequence of conditions
  virtual std::pair<HypoJetCIter, HypoJetCIter>
  filter (const HypoJetCIter& b,
	  const HypoJetCIter& e,
	  const std::unique_ptr<ITrigJetHypoInfoCollector>&
	  ) override;

  virtual std::string toString() const override;  
 private:

  ConditionPtrs m_conditions;
  HypoJetVector m_filtered;  
};

std::ostream& operator<<(std::ostream&, const ConditionFilter&);


#endif
