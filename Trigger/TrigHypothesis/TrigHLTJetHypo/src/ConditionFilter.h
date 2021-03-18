/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_CONDITIONFILTER_H
#define TRIGHLTJETHYPO_CONDITIONFILTER_H

#include "./IHypoJetVectorFilter.h"
#include "./ConditionsDefsMT.h"
#include <ostream>

class ConditionFilter: public IHypoJetVectorFilter  {
 public:

  ConditionFilter(){};

  ConditionFilter(ConditionsMT&);

  // find the subset of jets which satisfy a sequence of conditions
  virtual std::pair<HypoJetCIter, HypoJetCIter>
  filter (const HypoJetCIter& b,
	  const HypoJetCIter& e,
	  const std::unique_ptr<ITrigJetHypoInfoCollector>&
	  ) override;

  virtual std::string toString() const override;  
 private:

  ConditionsMT m_conditions;
  HypoJetVector m_filtered;  
};

std::ostream& operator<<(std::ostream&, const ConditionFilter&);


#endif
