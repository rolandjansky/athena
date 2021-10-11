/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_JVTCONDITION_H
#define TRIGHLTJETHYPO_JVTCONDITION_H

/********************************************************************
 *
 * NAME:     JVTCondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR: Jona Bossio
 *           
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "./ICondition.h"
#include <vector>
#include <string>

class ITrigJetHypoInfoCollector;

class JVTCondition: public ICondition{
 public:
  JVTCondition(double workingPoint);

  bool isSatisfied(const HypoJetVector&, const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;
  
  std::string toString() const override;

  virtual unsigned int capacity() const override{return s_capacity;}

 private:

  double m_workingPoint;
  double m_maxEta = 2.5;
  double m_maxPt  = 120;

  const static unsigned int s_capacity{1};

};

#endif
