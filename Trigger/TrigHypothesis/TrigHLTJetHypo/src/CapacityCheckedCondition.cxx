/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./CapacityCheckedCondition.h"

#include <memory>
#include <string>


CapacityCheckedCondition::CapacityCheckedCondition(std::unique_ptr<IConditionMT> cp,  std::size_t mult):
  m_condition{std::move(cp)}, m_multiplicity{mult} {}


CapacityCheckedCondition::~CapacityCheckedCondition(){}
  
bool
CapacityCheckedCondition::multiplicitySatisfied(std::size_t jgMultiplicity,
						const Collector&) const {
  return m_multiplicity <= jgMultiplicity;
}
  
bool
CapacityCheckedCondition::isSatisfied(const HypoJetVector& v,
				      const std::unique_ptr<ITrigJetHypoInfoCollector>& c) const {
  return m_condition->isSatisfied(v, c);
}
  
unsigned int CapacityCheckedCondition::capacity() const {
  return m_condition->capacity();
}
  
std::string CapacityCheckedCondition::toString() const {
  std::stringstream ss;
  const void* address = static_cast<const void*>(this);
  
  ss << "CapacityCheckedCondition (" << address << ") Multiplicity: "
     << m_multiplicity << '\n' << m_condition->toString();

  return ss.str();
}
