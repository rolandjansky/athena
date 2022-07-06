/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_TIMINGCONDITION_H
#define TRIGHLTJETHYPO_TIMINGCONDITION_H

/********************************************************************
 *
 * NAME:     TimingCondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "./ICondition.h"
#include <vector>
#include <string>

class ITrigJetHypoInfoCollector;

class TimingCondition: public ICondition{
 public:
  TimingCondition(double t_min,
		  double t_max);

  bool isSatisfied(const HypoJetVector&, const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;
  
  std::string toString() const override;

  virtual unsigned int capacity() const override{return s_capacity;}

 private:

  
  double m_min;
  double m_max;
  const static unsigned int s_capacity{1};

};

#endif
