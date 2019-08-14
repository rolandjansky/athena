/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ICONDITIONMT_H
#define TRIGHLTJETHYPO_ICONDITIONMT_H
/********************************************************************
 *
 * NAME:     IConditionMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *           
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include <string>


class ITrigJetHypoInfoCollector;

class IConditionMT {
 public:
  virtual ~IConditionMT(){}
  virtual bool isSatisfied(const HypoJetVector&,
                           const std::unique_ptr<ITrigJetHypoInfoCollector>&) const = 0;
  
  virtual unsigned int capacity() const = 0;
  virtual unsigned int conditionID() const = 0;
  virtual std::string toString() const noexcept = 0;
};

#endif
