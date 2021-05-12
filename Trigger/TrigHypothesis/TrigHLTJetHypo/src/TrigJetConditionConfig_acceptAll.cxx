/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJetConditionConfig_acceptAll.h"

#include "./AcceptAllCondition.h"

TrigJetConditionConfig_acceptAll::TrigJetConditionConfig_acceptAll
(const std::string& type,
 const std::string& name,
 const IInterface* parent) :  base_class(type, name, parent){

}

Condition TrigJetConditionConfig_acceptAll::getCondition() const {
  return std::make_unique<AcceptAllCondition>();
}
