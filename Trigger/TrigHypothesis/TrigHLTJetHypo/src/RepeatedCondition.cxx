/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./RepeatedCondition.h"

#include <memory>
#include <string>


RepeatedCondition::RepeatedCondition(std::unique_ptr<IConditionMT> cp,
						   std::size_t mult,
						   int cpInd):
  m_condition{std::move(cp)}, m_multiplicity{mult}, m_chainPartInd{cpInd}{}


RepeatedCondition::~RepeatedCondition(){}
  
bool
RepeatedCondition::multiplicitySatisfied(std::size_t jgMultiplicity,
						const Collector&) const {
  return m_multiplicity <= jgMultiplicity;
}
  
bool
RepeatedCondition::isSatisfied(const HypoJetVector& v,
				      const std::unique_ptr<ITrigJetHypoInfoCollector>& c) const {
  return m_condition->isSatisfied(v, c);
}
  
unsigned int RepeatedCondition::capacity() const {
  return m_condition->capacity();
}

std::size_t RepeatedCondition::multiplicity() const {
  return m_multiplicity;
}
  
std::string RepeatedCondition::toString() const {
  std::stringstream ss;
  const void* address = static_cast<const void*>(this);
  
  ss << "RepeatedCondition (" << address << ") Multiplicity: "
     << m_multiplicity
     << " chainPartInd " << m_chainPartInd << '\n'
     << m_condition->toString();

  return ss.str();
}

int RepeatedCondition::label() const {
  return m_chainPartInd;
}

bool RepeatedCondition::isFromChainPart() const {
  return m_chainPartInd >= 0;
}

std::ostream& operator<<(std::ostream& out,
			 const RepeatedCondition& c){

  out << c.toString();
  return out;
}
