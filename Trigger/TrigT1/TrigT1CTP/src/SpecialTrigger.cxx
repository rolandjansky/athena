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
   m_thrNumber(0),
   m_ctpVersionNumber(4)
{
  m_ctpVersion = new CTPdataformatVersion(m_ctpVersionNumber);
}
  
LVL1CTP::SpecialTrigger::SpecialTrigger(TrigConf::L1DataDef::TriggerType tt, unsigned int thrnumber, unsigned int ctpVersionNumber) :
   m_rate(1),
   m_counter(1),
   m_type(tt),
   m_thrNumber(thrnumber),
   m_ctpVersionNumber(ctpVersionNumber)
{
  m_ctpVersion = new CTPdataformatVersion(m_ctpVersionNumber);
}
  
LVL1CTP::SpecialTrigger::SpecialTrigger(TrigConf::L1DataDef::TriggerType tt, unsigned int thrnumber, unsigned int ctpVersionNumber, const unsigned int rate) :
   m_rate(rate),
   m_counter(rate),
   m_type(tt),
   m_thrNumber(thrnumber),
   m_ctpVersionNumber(ctpVersionNumber)
{
  m_ctpVersion = new CTPdataformatVersion(m_ctpVersionNumber);
}

LVL1CTP::SpecialTrigger::~SpecialTrigger()
{
  if (m_ctpVersion) delete m_ctpVersion;
  m_ctpVersion = NULL;

}

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
  /*
   This information is stored in an additional (or auxiliary) word in the event format, after the trigger input words.
   The lowest 4 bits of this word are used for BCID information, 
   then there are the bunchgroup triggers, the random triggers and the prescaled clock triggers
   */
  switch (m_type) {
   case TrigConf::L1DataDef::BGRP:
      //return 160 + m_thrNumber; //this is a bug! the 4 lowest bits are the BCID! should be 164 + threshold number
      return m_ctpVersion->getMaxTIPtotal() + m_ctpVersion->getBunchGroupShift() + m_thrNumber;
   case TrigConf::L1DataDef::RNDM:
      //return 172 + m_thrNumber;
      return m_ctpVersion->getMaxTIPtotal() +  m_ctpVersion->getRandomTrigShift() + m_thrNumber;
   case TrigConf::L1DataDef::PCLK:
      //return 174 + m_thrNumber;
			if (m_ctpVersionNumber>3) {
                                throw runtime_error("There are no prescaled clock triggers in CTP version " + std::to_string(m_ctpVersionNumber));
				return 9999;
			}else {
				return m_ctpVersion->getMaxTIPtotal() + m_ctpVersion->getPrescaledClockShift() + m_thrNumber;
			}
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
