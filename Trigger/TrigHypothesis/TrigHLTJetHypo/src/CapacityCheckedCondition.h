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
#include <ostream>


class ITrigJetHypoInfoCollector;

class CapacityCheckedCondition: public ICapacityCheckedCondition {
 public:
 CapacityCheckedCondition(std::unique_ptr<IConditionMT> cp,
			  std::size_t mult,
			  const std::string& label="");
  virtual ~CapacityCheckedCondition();
  
  virtual bool
  multiplicitySatisfied(std::size_t jgMultiplicity,
			const Collector&) const override;
  virtual bool
    isSatisfied(const HypoJetVector& v,
		const std::unique_ptr<ITrigJetHypoInfoCollector>& c) const override;
  
  virtual unsigned int capacity() const override;
  
  virtual std::string toString() const override;

  virtual std::string label() const override;

private:

  std::unique_ptr<IConditionMT> m_condition;
  std::size_t m_multiplicity;
  std::string m_label;
};

std::ostream& operator<<(std::ostream&,
			 const CapacityCheckedCondition&);

#endif
