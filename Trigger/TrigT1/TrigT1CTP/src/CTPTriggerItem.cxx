/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./CTPTriggerItem.h"
#include "TrigConfData/LogicParser.h"
#include "TrigConfL1Data/TriggerItem.h"

#include <exception>

LVL1CTP::CTPTriggerItem::CTPTriggerItem()
{}

LVL1CTP::CTPTriggerItem::~CTPTriggerItem()
{}

void
LVL1CTP::CTPTriggerItem::setPrescale(int prescale) {
   m_prescale = prescale;
}


int
LVL1CTP::CTPTriggerItem::prescale() const {
   return m_prescale;
}

void
LVL1CTP::CTPTriggerItem::setName( const std::string & name ) {
   m_name = name;
}


const std::string &
LVL1CTP::CTPTriggerItem::name() const {
   return m_name;
}

unsigned int
LVL1CTP::CTPTriggerItem::ctpId() const {
   return m_ctpid;
}

void 
LVL1CTP::CTPTriggerItem::setCtpId( unsigned int ctpid ) {
   m_ctpid = ctpid;
}

void
LVL1CTP::CTPTriggerItem::setTriggerType(unsigned char triggerType) {
   m_TriggerType = (triggerType & 0xff);
}

unsigned char
LVL1CTP::CTPTriggerItem::triggerType() const {
   return m_TriggerType;
}

void
LVL1CTP::CTPTriggerItem::setLogic(const std::string & logicExpr) {
   TrigConf::LogicParser p;
   m_logic = p.parse(logicExpr);
}


const std::shared_ptr<TrigConf::Logic> &
LVL1CTP::CTPTriggerItem::logic() const {
   return m_logic;
}

void
LVL1CTP::CTPTriggerItem::setBunchGroups(const std::vector<std::string> & bunchGroups) {
   m_bunchGroups = bunchGroups;
}

const std::vector<std::string> &
LVL1CTP::CTPTriggerItem::bunchGroups() const {
   return m_bunchGroups;
}


bool
LVL1CTP::CTPTriggerItem::evaluate( const std::map<std::string, unsigned int> & thrDecMap ) const
{
   if(m_logic) {
      bool dec = m_logic->evaluate(thrDecMap);
      if( dec && (! m_bunchGroups.empty()) ) {
         // apply bunchgroups, if set (if not set, it is part of the logic)
         for( auto & bgName : m_bunchGroups ) {
            try {
               if( thrDecMap.at(bgName) == 0 ) {
                  dec = false; break;
               }
            }
            catch(std::exception &) {
               throw std::runtime_error( "Problem accessing decision for bunchgroup " + bgName);
            }
         }
      }
      return dec;
   } else {
      std::cerr << "No logic set for this ctpItem " << name() << std::endl;
      return false;
   }
}
