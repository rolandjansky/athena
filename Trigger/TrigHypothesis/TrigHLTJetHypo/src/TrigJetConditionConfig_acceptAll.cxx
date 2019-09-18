/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJetConditionConfig_acceptAll.h"

#include "./conditionsFactoryMT.h"

TrigJetConditionConfig_acceptAll::TrigJetConditionConfig_acceptAll
(const std::string& type,
 const std::string& name,
 const IInterface* parent) :  base_class(type, name, parent){

}

std::optional<ConditionsMT>
TrigJetConditionConfig_acceptAll::getConditions() const {
  return
    std::make_optional<ConditionsMT>(conditionsFactoryAcceptAllMT(m_capacity));
}


