/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include <string>
#include "./IConditionMT.h"

namespace HypoJet{
  class IJet;
}

class ITrigJetHypoInfoCollector;

class HTConditionMT: public IConditionMT{
 public:
  HTConditionMT(double htMin);
  ~HTConditionMT() override {}

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;

  std::string toString() const noexcept override;

  virtual unsigned int capacity() const override {return s_capacity;}

 private:

  bool isSatisfied(const pHypoJet&,
		std::unique_ptr<ITrigJetHypoInfoCollector>& ) const;

  double m_htMin;

  const static  unsigned int s_capacity{1};

};

#endif
