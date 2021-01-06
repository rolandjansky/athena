/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_HTCONDITIONFASTREDUCTION_H
#define TRIGHLTJETHYPO_HTCONDITIONFASTREDUCTION_H

/********************************************************************
 *
 * NAME:     HTConditionFastReduction.h
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

class HTConditionFastReduction: public IConditionMT{
 public:

  
  HTConditionFastReduction(double htMin, double htMax);

  
  ~HTConditionFastReduction() override {}

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;

  std::string toString() const override;
  virtual unsigned int capacity() const override {return s_capacity;}

 private:
  
  double m_htMin;
  
  const static  unsigned int s_capacity{std::numeric_limits<int>::max()};
  
  
};

#endif
