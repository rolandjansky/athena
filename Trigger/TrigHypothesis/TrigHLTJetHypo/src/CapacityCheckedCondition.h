/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_CAPACITYCHECKEDCONDITIONMT_H
#define TRIGHLTJETHYPO_CAPACITYCHECKEDCONDITIONMT_H

/********************************************************************
 *
 * NAME:     CapacityCheckedConditionMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * 
 * an implementation of the  ICapacityCheckedConditionMT PABC.
 *          
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "ICapacityCheckedCondition.h"

#include <memory>
#include <string>


class ITrigJetHypoInfoCollector;

class CapacityCheckedCondition: public ICapacityCheckedCondition {
 public:
 CapacityCheckedCondition(std::unique_ptr<IConditionMT> cp):
  m_condition{std::move(cp)} {
  }
  
  virtual ~CapacityCheckedCondition(){
  }

  virtual bool
    capacitySatisfied(std::size_t jgMultiplicity,
		      const Collector&) const override {

    return m_condition->capacity() <= jgMultiplicity;
  }
  
  virtual bool isSatisfied(const HypoJetVector& v,
                           const std::unique_ptr<ITrigJetHypoInfoCollector>& c) const override {
    return m_condition->isSatisfied(v, c);
  }
  
  virtual unsigned int capacity() const override {
    return m_condition->capacity();
  }
  
  virtual std::string toString() const noexcept override {
    return m_condition->toString();
  }
  
 private:
  std::unique_ptr<IConditionMT> m_condition;
};

#endif
