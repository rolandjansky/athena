/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ACCEPTALL_H
#define TRIGHLTJETHYPO_ACCEPTALL_H

/********************************************************************
 *
 * NAME:     AcceptAllCondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *********************************************************************/

#include <string>
#include "./ICondition.h"

namespace HypoJet{
  class IJet;
}

class ITrigJetHypoInfoCollector;

class AcceptAllCondition: public ICondition{
 public:
  ~AcceptAllCondition() override {}

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;

  virtual unsigned int capacity() const override{return 0;}
  std::string toString() const override;
  
 private:
  
};

#endif
