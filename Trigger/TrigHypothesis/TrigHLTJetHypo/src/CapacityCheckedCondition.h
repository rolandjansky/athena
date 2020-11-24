/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_CAPACITYCHECKEDCONDITIONMT_H
#define TRIGHLTJETHYPO_CAPACITYCHECKEDCONDITIONMT_H

/********************************************************************
 *
 * NAME:     CapacityCheckedConditionMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * 
 * an implementation of the  ICapacityCheckedConditionMT PABC.
 *          
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "ICapacityCheckedCondition.h"

#include <memory>
#include <string>


class ITrigJetHypoInfoCollector;

class CapacityCheckedCondition: public ICapacityCheckedCondition {
 public:
 CapacityCheckedCondition(std::unique_ptr<IConditionMT> cp,
			  std::size_t mult);
  virtual ~CapacityCheckedCondition();
  
  virtual bool
  multiplicitySatisfied(std::size_t jgMultiplicity,
			const Collector&) const override;
  virtual bool
    isSatisfied(const HypoJetVector& v,
		const std::unique_ptr<ITrigJetHypoInfoCollector>& c) const override;
  
  virtual unsigned int capacity() const override;
  
  virtual std::string toString() const override;

private:

  std::unique_ptr<IConditionMT> m_condition;
  std::size_t m_multiplicity;
};

#endif
