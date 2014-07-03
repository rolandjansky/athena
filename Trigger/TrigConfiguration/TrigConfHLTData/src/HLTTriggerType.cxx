/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfHLTData/HLTTriggerType.h"
#include <iostream>
#include <sstream>

TrigConf::HLTTriggerType::HLTTriggerType( int bit )
  : m_bit(bit)
{}

TrigConf::HLTTriggerType::~HLTTriggerType( )
{}

void
TrigConf::HLTTriggerType::print(const std::string& indent, unsigned int /*detail*/) const {
   std::cout << indent << *this;
}

std::ostream &
TrigConf::operator<<(std::ostream & o, const TrigConf::HLTTriggerType & tt) {
  o << "- -- HLTTriggerType bit : " << tt.m_bit << std::endl;
  return o;
}
