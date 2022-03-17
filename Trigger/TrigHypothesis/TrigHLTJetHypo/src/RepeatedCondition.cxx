/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./RepeatedCondition.h"

#include <memory>
#include <string>


RepeatedCondition::RepeatedCondition(std::unique_ptr<ICondition> cp,
				     std::size_t mult,
				     int clique,
				     int cpInd,
				     bool invert):
  m_condition{std::move(cp)}, m_multiplicity{mult},
  m_clique{clique},
  m_chainPartInd{cpInd}, m_invert{invert}{}


RepeatedCondition::~RepeatedCondition(){}
  
bool
RepeatedCondition::multiplicitySatisfied(std::size_t jgMultiplicity,
						const Collector&) const {
  return m_multiplicity <= jgMultiplicity;
}
  
bool
RepeatedCondition::isSatisfied(const HypoJetVector& v,
				      const std::unique_ptr<ITrigJetHypoInfoCollector>& c) const {
  
  bool result =  m_condition->isSatisfied(v, c);
  return m_invert ? !result : result;
}
  
unsigned int RepeatedCondition::capacity() const {
  return m_condition->capacity();
}

std::size_t RepeatedCondition::multiplicity() const {
  return m_multiplicity;
}


int RepeatedCondition::clique() const {
  return m_clique;
}
  
std::string RepeatedCondition::toString() const {
  std::stringstream ss;
  const void* address = static_cast<const void*>(this);
  
  ss << "RepeatedCondition (" << address << ") Multiplicity: "
     << m_multiplicity
     << " clique " << m_clique
     << " chainPartInd " << m_chainPartInd
     << " invert " << std::boolalpha << m_invert
     << '\n';
    if (m_condition){
      ss << m_condition->toString();
    } else {
      ss << " no internal condition ";
    }

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
