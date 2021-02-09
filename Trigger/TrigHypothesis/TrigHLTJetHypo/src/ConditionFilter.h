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
  HypoJetVector filter (const HypoJetCIter& b,
			const HypoJetCIter& e,
			const std::unique_ptr<ITrigJetHypoInfoCollector>&
			) const;

  HypoJetVector filter (const HypoJetVector&,
			const std::unique_ptr<ITrigJetHypoInfoCollector>&
			) const;
  
  std::string toString() const;  
 private:

  ConditionsMT m_conditions;
};

std::ostream& operator<<(std::ostream&, const ConditionFilter&);


#endif
