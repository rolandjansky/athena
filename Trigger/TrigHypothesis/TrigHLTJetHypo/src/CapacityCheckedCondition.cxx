/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./CapacityCheckedCondition.h"

#include <memory>
#include <string>


CapacityCheckedCondition::CapacityCheckedCondition(std::unique_ptr<IConditionMT> cp,
						   std::size_t mult,
						   const std::string& label):
  m_condition{std::move(cp)}, m_multiplicity{mult}, m_label{label}{}


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
     << m_multiplicity << " label "
     << m_label << '\n'
     << m_condition->toString();

  return ss.str();
}

std::string CapacityCheckedCondition::label() const {
  return m_label;
}

std::ostream& operator<<(std::ostream& out,
			 const CapacityCheckedCondition& c){

  out << c.toString();
  return out;
}
