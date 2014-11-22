/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/PIT.h"

#include <stdexcept>
#include <iostream>

using namespace std;

TrigConf::PIT::PIT() :
   L1DataBaseclass(), 
   m_ThresholdName(""), 
   m_CtpinSlot(0),
   m_CtpinConnector(0),
   m_PitNumber(0),
   m_ThresholdBit(0),
   m_CableBit(0),
   m_TmToTtId(0),
   m_TriggerThresholdId(0),
   m_ThresholdActive(0),
   m_ThresholdMapping(0)
{}
	
TrigConf::PIT::~PIT()
{}
	
void
TrigConf::PIT::print(const std::string& indent, unsigned int /*detail*/) const{
   cout << indent << "PIT " << m_PitNumber;
   cout << ": threshold '" << m_ThresholdName << "'";
   cout << ", threshold bit " << m_ThresholdBit;
   cout << ", ctpin slot " << m_CtpinSlot;
   cout << ", connector " << m_CtpinConnector;
   cout << ", cable bit " << m_CableBit;
   cout << endl; 
}

