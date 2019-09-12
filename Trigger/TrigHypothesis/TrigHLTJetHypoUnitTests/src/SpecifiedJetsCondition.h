/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_SPECIFIEDJETSCONDITION_H
#define TRIGHLTJETHYPO_SPECIFIEDJETSCONDITION_H

/********************************************************************
 *
 * NAME:     SpecifiedJetsCondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *********************************************************************/

#include <string>
#include "TrigHLTJetHypo/../src/IConditionMT.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"

namespace HypoJet{
  class IJet;
}

class ITrigJetHypoInfoCollector;

class SpecifiedJetsCondition: public IConditionMT{
 public:
  SpecifiedJetsCondition(const HypoJetVector&,
			 std::size_t conditionID=0);
  ~SpecifiedJetsCondition() override {}

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;

  virtual unsigned int capacity() const override{return m_capacity;}
  virtual unsigned int conditionID() const override{return m_conditionID;}


  std::string toString() const noexcept override;
 private:
  HypoJetVector m_acceptedJets;
  std::size_t m_capacity;
  std::size_t m_conditionID{0};
  
};

#endif
