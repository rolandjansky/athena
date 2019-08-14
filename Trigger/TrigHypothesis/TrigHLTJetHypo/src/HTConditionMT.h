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
  HTConditionMT(double htMin, unsigned int conditionID=0);
  ~HTConditionMT() override {}

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;

  std::string toString() const noexcept override;

  virtual unsigned int conditionID() const override{return m_conditionID;}
  virtual unsigned int capacity() const override {return s_capacity;}

 private:

  bool isSatisfied(const pHypoJet&,
		std::unique_ptr<ITrigJetHypoInfoCollector>& ) const;

  double m_htMin;

  unsigned int m_conditionID{0}; 
  const static  unsigned int s_capacity{1};

};

#endif
