/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ICONDITION_H
#define TRIGHLTJETHYPO_ICONDITION_H
/********************************************************************
 *
 * NAME:     ICondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *           
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include <string>


class ITrigJetHypoInfoCollector;

class ICondition {
 public:
  virtual ~ICondition(){}

  virtual bool isSatisfied(const HypoJetVector&,
                           const std::unique_ptr<ITrigJetHypoInfoCollector>&) const = 0;
  
  virtual unsigned int capacity() const = 0;
  virtual std::string toString() const = 0;
};

#endif
