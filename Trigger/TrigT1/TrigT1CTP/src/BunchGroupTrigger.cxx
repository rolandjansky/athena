/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CTP/BunchGroupTrigger.h"
#include "TrigT1CTP/MsgWriter.h"

#include <sstream>

// gaudi includes
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"

LVL1CTP::BunchGroupTrigger::BunchGroupTrigger() : SpecialTrigger() 
{}

LVL1CTP::BunchGroupTrigger::BunchGroupTrigger(unsigned int bgrp_number, const std::vector<int>& bunches, unsigned int ctpVersionNumber) :
   SpecialTrigger(TrigConf::L1DataDef::BGRP, bgrp_number, ctpVersionNumber),
   m_bunches(bunches)
{}

StatusCode
LVL1CTP::BunchGroupTrigger::execute(int bcid=1) {
   //nominal bcid is set to 1 unless explicit value is given
   m_counter = bcid;
   return StatusCode::SUCCESS;
}

bool
LVL1CTP::BunchGroupTrigger::evaluate() const {
   for (std::vector<int>::const_iterator iter(m_bunches.begin()); iter != m_bunches.end(); ++iter)
      if (m_counter == *iter) return true;
   return false;
}

std::string
LVL1CTP::BunchGroupTrigger::print() const {
   std::ostringstream s;
    
   s << "Name: " << this->name() << " Bunch Group ID: " << m_counter << " - ";
   for (size_t i(0); i < m_bunches.size(); ++i) {
      s << m_bunches[i] << " ";
   }
   s << ( this->evaluate() ? "ACTIVE" : "INACTIVE" );

   return s.str();
}

std::string
LVL1CTP::BunchGroupTrigger::dump() const {
   std::ostringstream s;
   
   s << this->name() << " " << m_counter << " ";
   for (size_t i(0); i < m_bunches.size(); ++i) {
      s << m_bunches[i] << " ";
   }
   s << this->evaluate();
    
   return s.str();  
}
