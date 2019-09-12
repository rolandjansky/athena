/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ACCEPTALLMT_H
#define TRIGHLTJETHYPO_ACCEPTALLMT_H

/********************************************************************
 *
 * NAME:     EtaEtConditionMT.h
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

class AcceptAllConditionMT: public IConditionMT{
 public:
  AcceptAllConditionMT(std::size_t, unsigned int conditionId=0);
  ~AcceptAllConditionMT() override {}

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;

  virtual unsigned int capacity() const override{return m_capacity;}
  virtual unsigned int conditionID() const override{return m_conditionID;}


  std::string toString() const noexcept override;
 private:

  std::size_t m_capacity;
  unsigned int m_conditionID{0}; 

  
};

#endif
