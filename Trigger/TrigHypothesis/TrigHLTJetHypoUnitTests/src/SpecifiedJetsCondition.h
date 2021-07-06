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
#include "TrigHLTJetHypo/../src/ICondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"

namespace HypoJet{
  class IJet;
}

class ITrigJetHypoInfoCollector;

class SpecifiedJetsCondition: public ICondition{
 public:
  SpecifiedJetsCondition(const HypoJetGroupVector&);
  ~SpecifiedJetsCondition() override {}

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;

  virtual unsigned int capacity() const override{return m_capacity;}

  std::string toString() const noexcept override;
 private:
  HypoJetGroupVector m_acceptedJetGroups;
  std::size_t m_capacity;

  bool
    groupIsSatisfied(const HypoJetVector& ips,
		     const HypoJetVector& group,
		     const std::unique_ptr<ITrigJetHypoInfoCollector>&) const;
  
};

#endif
