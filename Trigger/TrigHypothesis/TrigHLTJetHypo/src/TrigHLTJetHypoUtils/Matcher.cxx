/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/Matcher.h"

Matcher::Matcher(const std::string& name):m_name(name){}

std::string Matcher::toString() const noexcept{
  return "Matcher: " + m_name + "\n";
}
