/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_PASSTHROUGHFILTER_H
#define TRIGHLTJETHYPO_PASSTHROUGHFILTER_H

#include "./IHypoJetVectorFilter.h"
#include "./RepeatedConditionsDefs.h"
#include <ostream>

class PassThroughFilter: public IHypoJetVectorFilter  {
 public:

  PassThroughFilter(){};
  // find the subset of jets which satisfy a sequence of conditions
  virtual HypoJetVector
  filter (const HypoJetVector& jv,
	  const std::unique_ptr<ITrigJetHypoInfoCollector>&
	  ) const override;

  virtual std::string toString() const override;  
};

std::ostream& operator<<(std::ostream&, const PassThroughFilter&);


#endif
