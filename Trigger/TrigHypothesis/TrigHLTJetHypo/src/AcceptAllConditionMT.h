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
  ~AcceptAllConditionMT() override {}

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;

  virtual unsigned int capacity() const override{return 0;}
  std::string toString() const override;
  
 private:
  
};

#endif
