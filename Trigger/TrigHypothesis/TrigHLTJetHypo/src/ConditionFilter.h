/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_CONDITIONFILTER_H
#define TRIGHLTJETHYPO_CONDITIONFILTER_H

#include "./ConditionsDefsMT.h"
#include <ostream>

class ConditionFilter {
 public:

  ConditionFilter(){};

  ConditionFilter(ConditionsMT&);

  // find the subset of jets which satisfy a sequence of conditions
  std::pair<HypoJetCIter, HypoJetCIter>
  filter (const HypoJetCIter& b,
	  const HypoJetCIter& e,
	  const std::unique_ptr<ITrigJetHypoInfoCollector>&
	  );

  std::string toString() const;  
 private:

  ConditionsMT m_conditions;
  HypoJetVector m_filtered;  
};

std::ostream& operator<<(std::ostream&, const ConditionFilter&);


#endif
