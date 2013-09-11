/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CTP/SpecialTrigger.h"

#include "boost/lexical_cast.hpp"

#include <sstream>
#include <stdexcept>

using namespace std;

LVL1CTP::SpecialTrigger::SpecialTrigger() :
   m_rate(1),
   m_counter(1),
   m_type(TrigConf::L1DataDef::UNDEF),
   m_thrNumber(0)
{}
  
LVL1CTP::SpecialTrigger::SpecialTrigger(TrigConf::L1DataDef::TriggerType tt, unsigned int thrnumber) :
   m_rate(1),
   m_counter(1),
   m_type(tt),
   m_thrNumber(thrnumber)
{}
  
LVL1CTP::SpecialTrigger::SpecialTrigger(TrigConf::L1DataDef::TriggerType tt, unsigned int thrnumber, const unsigned int rate) :
   m_rate(rate),
   m_counter(rate),
   m_type(tt),
   m_thrNumber(thrnumber)
{}

LVL1CTP::SpecialTrigger::~SpecialTrigger()
{}

string
LVL1CTP::SpecialTrigger::name() const {
   return TrigConf::L1DataDef::typeAsString(m_type) + boost::lexical_cast<string,unsigned int>(m_thrNumber);
}

unsigned int
LVL1CTP::SpecialTrigger::rate() const {
   return m_rate;
}

unsigned int
LVL1CTP::SpecialTrigger::pit() const {
   switch (m_type) {
   case TrigConf::L1DataDef::BGRP:
      return 160 + m_thrNumber;
   case TrigConf::L1DataDef::RNDM:
      return 172 + m_thrNumber;
   case TrigConf::L1DataDef::PCLK:
      return 174 + m_thrNumber;
   case TrigConf::L1DataDef::UNDEF:
      return 0;
   default:
      throw runtime_error(TrigConf::L1DataDef::typeAsString(m_type) + " is not an internal trigger");
   }
   return 0;
}

string
LVL1CTP::SpecialTrigger::print() const {
   std::ostringstream s;
    
   s << "Name: " << this->name() << " Counter: " << m_counter << "/" << this->rate()
     << " " << ( this->evaluate() ? "ACTIVE" : "INACTIVE" );
    
   return s.str();
}

string
LVL1CTP::SpecialTrigger::dump() const {
   std::ostringstream s;
   s << this->name() << " " << this->rate() << " " << m_counter << " " << this->evaluate();
   return s.str();  
}
