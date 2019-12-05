/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJetConditionConfig_acceptAll.h"

#include "./AcceptAllConditionMT.h"

TrigJetConditionConfig_acceptAll::TrigJetConditionConfig_acceptAll
(const std::string& type,
 const std::string& name,
 const IInterface* parent) :  base_class(type, name, parent){

}

ConditionMT TrigJetConditionConfig_acceptAll::getCondition() const {
  return std::make_unique<AcceptAllConditionMT>(m_capacity);
}



bool TrigJetConditionConfig_acceptAll::addToCapacity(std::size_t cap) {
  m_capacity  += cap;
  return true;
}


std::size_t TrigJetConditionConfig_acceptAll::capacity() const {
  return m_capacity;
}
