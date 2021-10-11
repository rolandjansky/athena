/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_IHYPOJETVECTORFILTER_H
#define TRIGHLTJETHYPO_IHYPOJETVECTORFILTER_H

#include "./ConditionsDefs.h"

class IHypoJetVectorFilter {
 public:
  virtual ~IHypoJetVectorFilter() = default;
  virtual HypoJetVector
  filter (const HypoJetVector& jv, 
	  const std::unique_ptr<ITrigJetHypoInfoCollector>&
	  ) const = 0;
  
  virtual std::string toString() const = 0;
};

std::ostream& operator<<(std::ostream&, const IHypoJetVectorFilter&);
#endif

