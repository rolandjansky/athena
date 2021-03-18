/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_IHYPOJETVECTORFILTER_H
#define TRIGHLTJETHYPO_IHYPOJETVECTORFILTER_H

#include "./ConditionsDefsMT.h"

class IHypoJetVectorFilter {
 public:
  virtual ~IHypoJetVectorFilter() = default;
  virtual std::pair<HypoJetCIter, HypoJetCIter>
    filter (const HypoJetCIter& b,
	    const HypoJetCIter& e,
	    const std::unique_ptr<ITrigJetHypoInfoCollector>&
	    ) = 0;
  
  virtual std::string toString() const = 0;
};

std::ostream& operator<<(std::ostream&, const IHypoJetVectorFilter&);
#endif

