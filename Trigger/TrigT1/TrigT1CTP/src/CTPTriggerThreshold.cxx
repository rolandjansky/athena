/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./CTPTriggerThreshold.h"

LVL1CTP::CTPTriggerThreshold::CTPTriggerThreshold(const std::string & name) :
   m_name(name)
{}

LVL1CTP::CTPTriggerThreshold::~CTPTriggerThreshold()
{}

int
LVL1CTP::CTPTriggerThreshold::value() const {
   return m_value;
}

void
LVL1CTP::CTPTriggerThreshold::setValue( int value ) {
   m_value = value;
}

unsigned int
LVL1CTP::CTPTriggerThreshold::startBit() const {
   return m_start_bit;
}

void
LVL1CTP::CTPTriggerThreshold::setStartBit( unsigned int start ) {
   m_start_bit = start;
}

unsigned int
LVL1CTP::CTPTriggerThreshold::endBit() const {
   return m_end_bit;
}

void
LVL1CTP::CTPTriggerThreshold::setEndBit( unsigned int end ) {
   m_end_bit = end;
}
