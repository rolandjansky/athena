/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/GroupsMatcher.h"

GroupsMatcher::GroupsMatcher(const std::string& name):m_name(name){}

std::string GroupsMatcher::toString() const noexcept{
  return "GroupsMatcher: " + m_name + "\n";
}
