/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_COMPOUNDCONDITION_H
#define TRIGHLTJETHYPO_COMPOUNDCONDITION_H

/********************************************************************
 *
 * NAME:     CompoundCondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *********************************************************************/

#include <string>
#include "./ICondition.h"
#include "./ConditionsDefs.h"

class ITrigJetHypoInfoCollector;

class CompoundCondition: public ICondition{
 public:  
  CompoundCondition(std::vector<Condition>& elements);

  CompoundCondition(const CompoundCondition&) = delete;

  virtual bool isSatisfied(const HypoJetVector&,
			   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;

  virtual unsigned int capacity() const override{return m_capacity;}


  std::string toString() const override;
 private:

  std::vector<Condition> m_elements;

  unsigned int m_capacity;
  
};

#endif
