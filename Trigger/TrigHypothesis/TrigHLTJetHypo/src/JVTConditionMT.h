/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_JVTCONDITIONMT_H
#define TRIGHLTJETHYPO_JVTCONDITIONMT_H

/********************************************************************
 *
 * NAME:     JVTConditionMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR: Jona Bossio
 *           
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "./IConditionMT.h"
#include <vector>
#include <string>

class ITrigJetHypoInfoCollector;

class JVTConditionMT: public IConditionMT{
 public:
  JVTConditionMT(double workingPoint);

  bool isSatisfied(const HypoJetVector&, const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;
  
  std::string toString() const noexcept override;

  virtual unsigned int capacity() const override{return s_capacity;}

 private:

  double m_workingPoint;

  const static unsigned int s_capacity{1};

};

#endif
