/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./ConditionInverter.h"

ConditionInverterMT::ConditionInverterMT(ConditionMT condition):
  m_condition{std::move(condition)}{}

unsigned int  ConditionInverterMT::capacity() const{
  return m_condition->capacity();
}

bool
ConditionInverterMT::isSatisfied(const HypoJetVector& hjv,
				 const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {
  
  return !m_condition->isSatisfied(hjv, collector);
}


std::string ConditionInverterMT::toString() const {
  return "Condition Inerverter, negate " + m_condition->toString();
}
