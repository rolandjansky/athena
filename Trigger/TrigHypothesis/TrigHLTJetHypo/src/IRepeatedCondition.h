/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_IREPEATEDCONDITIONMT_H
#define TRIGHLTJETHYPO_IREPEATEDCONDITIONMT_H

/********************************************************************
 *
 * NAME:     IRepeatedConditionMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * 
 * extend IConditionMT by adding a method that checks whether the
 * capacity of the Condition has been attained.
 *          
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "IConditionMT.h"

#include <string>
typedef std::unique_ptr<ITrigJetHypoInfoCollector> Collector;


class ITrigJetHypoInfoCollector;

class IRepeatedCondition: public IConditionMT {
 public:
  virtual ~IRepeatedCondition(){}

  virtual bool multiplicitySatisfied(std::size_t jgMultiplicity,
				     const Collector&) const = 0;


  virtual int label() const = 0;
  virtual std::size_t multiplicity() const = 0;
  virtual std::string toString() const = 0;
  virtual bool isFromChainPart() const = 0;

};

#endif
