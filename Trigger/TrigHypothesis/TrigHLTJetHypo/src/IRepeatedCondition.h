/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_IREPEATEDCONDITION_H
#define TRIGHLTJETHYPO_IREPEATEDCONDITION_H

/********************************************************************
 *
 * NAME:     IRepeatedCondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * 
 * extend ICondition by adding a method that checks whether the
 * capacity of the Condition has been attained.
 *          
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "ICondition.h"

#include <string>
typedef std::unique_ptr<ITrigJetHypoInfoCollector> Collector;


class ITrigJetHypoInfoCollector;

class IRepeatedCondition: public ICondition {
 public:
  virtual ~IRepeatedCondition(){}

  virtual bool multiplicitySatisfied(std::size_t jgMultiplicity,
				     const Collector&) const = 0;


  virtual int label() const = 0;
  virtual std::size_t multiplicity() const = 0;
  virtual int clique() const = 0;
  virtual std::string toString() const = 0;
  virtual bool isFromChainPart() const = 0;

};

#endif
