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


