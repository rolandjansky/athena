/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_COMPOUNDCONDITIONMT_H
#define TRIGHLTJETHYPO_COMPOUNDCONDITIONMT_H

/********************************************************************
 *
 * NAME:     CompoundConditionMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *********************************************************************/

#include <string>
#include "./IConditionMT.h"
#include "./ConditionsDefsMT.h"

namespace HypoJet{
  class IJet;
}

class ITrigJetHypoInfoCollector;

class CompoundConditionMT: public IConditionMT{
 public:  
  CompoundConditionMT(std::vector<ConditionMT>& elements);

  CompoundConditionMT(const CompoundConditionMT&) = delete;

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;

  virtual unsigned int capacity() const override{return m_capacity;}


  std::string toString() const noexcept override;
 private:

  std::vector<ConditionMT> m_elements;

  unsigned int m_capacity;
  
};

#endif
