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
			  int chainPartind = -1);
  virtual ~CapacityCheckedCondition();
  
  virtual bool
  multiplicitySatisfied(std::size_t jgMultiplicity,
			const Collector&) const override;
  virtual bool
    isSatisfied(const HypoJetVector& v,
		const std::unique_ptr<ITrigJetHypoInfoCollector>& c) const override;
  
  virtual unsigned int capacity() const override;
  
  virtual std::string toString() const override;

  virtual int label() const override;

  virtual bool isFromChainPart() const override;

private:

  std::unique_ptr<IConditionMT> m_condition;
  std::size_t m_multiplicity{1};

  // record of which chain part in the chainDict this conditions comes from
  int m_chainPartInd{-1};
};

std::ostream& operator<<(std::ostream&,
			 const CapacityCheckedCondition&);

#endif
