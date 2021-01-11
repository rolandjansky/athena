/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ICAPACITYCHECKEDCONDITIONMT_H
#define TRIGHLTJETHYPO_ICAPACITYCHECKEDCONDITIONMT_H

/********************************************************************
 *
 * NAME:     ICapacityCheckedConditionMT.h
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

class ICapacityCheckedCondition: public IConditionMT {
 public:
  virtual ~ICapacityCheckedCondition(){}

  virtual bool multiplicitySatisfied(std::size_t jgMultiplicity,
				     const Collector&) const = 0;


  virtual int label() const = 0;
  virtual std::size_t multiplicity() const = 0;
  virtual std::string toString() const = 0;
  virtual bool isFromChainPart() const = 0;

};

#endif
