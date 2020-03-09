/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_HTCONDITIONMT_H
#define TRIGHLTJETHYPO_HTCONDITIONMT_H

/********************************************************************
 *
 * NAME:     HTConditionMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *********************************************************************/

#include "./IConditionMT.h"

#include <string>
#include <limits>
// #include <memory>

namespace HypoJet{
  class IJet;
}


class ITrigJetHypoInfoCollector;

class HTConditionMT: public IConditionMT{
 public:

  
  HTConditionMT(double htMin,
		double etMin,
		double absetamin,
		double absetamax);

  
  ~HTConditionMT() override {}

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;

  std::string toString() const noexcept override;
  virtual unsigned int capacity() const override {return s_capacity;}

 private:
  struct Filter{
    
  Filter(double etmin, double absetamin, double absetamax):
    etmin_(etmin), absetamin_(absetamin), absetamax_(absetamax){}
    
    bool operator() (const pHypoJet& j) const {
      return(j->et() > etmin_ &&
	     std::abs(j->eta()) > absetamin_ &&
	     std::abs(j->eta()) < absetamax_);
    }
    
    double etmin_{0.};
    double absetamin_{0.};
    double absetamax_{std::numeric_limits<double>::max()};
  };
  
  double m_htMin;
  std::shared_ptr<Filter> m_filter{nullptr};
  
  const static  unsigned int s_capacity{std::numeric_limits<int>::max()};
  
  
};

#endif
