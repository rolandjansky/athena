/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/TIP.h"

#include <stdexcept>
#include <iostream>
#include <iomanip>

using namespace std;

TrigConf::TIP::TIP() :
   L1DataBaseclass(), 
   m_ThresholdName(""), 
   m_Slot(0),
   m_Connector(0),
   m_TipNumber(0),
   m_ThresholdBit(0),
   m_CableBit(0),
   m_TmToTtId(0),
   m_TriggerThresholdId(0),
   m_ThresholdActive(0),
   m_ThresholdMapping(0),
   m_IsDirect(false),
   m_Clock(0)
{}
	
TrigConf::TIP::~TIP()
{}
	
void
TrigConf::TIP::print(const std::string& indent, unsigned int /*detail*/) const {
   cout << indent << "TIP " << setw(3) << m_TipNumber;
   cout << (m_IsDirect ? " (direct)  " :  " (PIT)     ");
   cout << ": phase " << m_Clock;
   cout << " slot " << setw(2) << m_Slot;
   cout << " cable bit " << setw(2) << m_CableBit;
   cout << " connector " << m_Connector;
   cout << " threshold bit " << m_ThresholdBit; 
   cout << " threshold " << m_ThresholdName;
   cout << endl; 
}

