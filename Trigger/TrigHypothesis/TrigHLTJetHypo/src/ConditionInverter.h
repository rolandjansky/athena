/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_CONDITIONINVERTERMT_H
#define TRIGHLTJETHYPO_CONDITIONINVERTERMT_H

/********************************************************************
 *
 * NAME:     ConditionInverterMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *
 * Invert the result of an IConditionMT::isSatisfied()
 *********************************************************************/

#include <string>
#include "./IConditionMT.h"
#include "./ConditionsDefsMT.h"

namespace HypoJet{
  class IJet;
}

class ITrigJetHypoInfoCollector;

class ConditionInverterMT: public IConditionMT{
 public:  
  ConditionInverterMT(ConditionMT condition);

  ConditionInverterMT(const ConditionInverterMT&) = delete;

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;
  
  virtual unsigned int
  capacity() const override;

  
  std::string toString() const override;
private:
  
  ConditionMT m_condition;
};

#endif
