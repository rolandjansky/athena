/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1CTP/PrescaledClockTrigger.h"

// gaudi includes
#include "GaudiKernel/StatusCode.h"

LVL1CTP::PrescaledClockTrigger::PrescaledClockTrigger() :
   SpecialTrigger() 
{}

LVL1CTP::PrescaledClockTrigger::PrescaledClockTrigger(unsigned int clock_number, const unsigned int aRate, unsigned int ctpVersionNumber) :
   SpecialTrigger(TrigConf::L1DataDef::PCLK, clock_number,ctpVersionNumber, aRate)
{}

StatusCode
LVL1CTP::PrescaledClockTrigger::execute(int /*bcid=1*/) {
   if (m_counter <= 0) m_counter = this->rate();
   m_counter -= 1.;
   return StatusCode::SUCCESS;
}

bool
LVL1CTP::PrescaledClockTrigger::evaluate() const {
   return (m_counter <= 0);
}
