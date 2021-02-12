/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ACCEPTALLMT_H
#define TRIGHLTJETHYPO_ACCEPTALLMT_H

/********************************************************************
 *
 * NAME:     AcceptAllConditionMT.h
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
  AcceptAllConditionMT(std::size_t capacity);
  ~AcceptAllConditionMT() override {}

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;

  virtual unsigned int capacity() const override{return m_capacity;}
  std::string toString() const override;
  
 private:

  std::size_t m_capacity;

  
};

#endif
