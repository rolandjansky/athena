//  GlobalDecision.cxx
//  L1TopoCoreSimulation
//  Created by Joerg Stelzer on 11/25/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#include <iomanip>

#include "L1TopoCoreSim/GlobalDecision.h"
#include "L1TopoCoreSim/DecisionConnector.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/DecisionAlg.h"

using namespace std;
using namespace TCS;

uint32_t
GlobalDecision::decision(unsigned int module, unsigned int clock) const {
   if(clock==0) {
      // lower 32 bit
      return (uint32_t) (m_decision[module] & 0xffffffff);
   } else {
      // upper 32 bit
      uint64_t clock1 = m_decision[module] & 0xffffffff00000000;
      return (uint32_t) (clock1 >> 32);
   }
}

uint32_t
GlobalDecision::overflow(unsigned int module, unsigned int clock) const {
   if(clock==0) {
      // lower 32 bit
      return (uint32_t) (m_overflow[module] & 0xffffffff);
   } else {
      // upper 32 bit
      uint64_t clock1 = m_overflow[module] & 0xffffffff00000000;
      return (uint32_t) (clock1 >> 32);
   }
}


GlobalDecision::GlobalDecision() :
   TrigConfMessaging("L1TopoGlobalDecision")
{}

/****************************************************************
 *
 * Fill the decision bits with the pass/fail of all decision algs.
 * This is called by the TopoSteering after the connectors have
 * been executed.
 *
 ****************************************************************/

StatusCode
GlobalDecision::collectDecision(const set<DecisionConnector*> & outconn) {
   resetDecision();

   for( const DecisionConnector * conn : outconn ) {

      const Decision& dec = conn->decision();

      unsigned int pos = 0; // for multi-output algorithms pos is the output index
      for(const TXC::TriggerLine & trigger : conn->triggers() ) {

         unsigned int bit = trigger.counter() % 64;  // trigger bit in module
         
         uint64_t & moduleDec = m_decision[trigger.module()];
         uint64_t & moduleOvf = m_overflow[trigger.module()];
         uint64_t mask(0x1);

         //std::cout << "JOERG GlobalDecision::collectDecision: trigger line " << trigger.name() << " [counter="<<trigger.counter()<<"] on module " << trigger.module() << " and bit [0-63] " << bit << " -> dec " << dec << std::endl;

         if( dec.bit(pos++) )  // bit set?
            moduleDec |= (mask << bit);
         if( dec.overflow())
            moduleOvf |= (mask << bit);
      }

   }
   m_valid = true;
   return StatusCode::SUCCESS;
}


StatusCode
GlobalDecision::resetDecision() {
   m_decision[0] = m_decision[1] = m_decision[2] = 0;
   m_overflow[0] = m_overflow[1] = m_overflow[2] = 0;
   m_valid = false;
   return StatusCode::SUCCESS;
}


namespace TCS {


std::ostream&
operator<<(std::ostream& o, const TCS::GlobalDecision & dec) {

   if(!dec.isValid()) 
      o << "Note that the overall decision has not been calculated" << endl;

   for(unsigned int module = 0; module<3; ++module)
      o << "Overall decision module " << module << ": 0x" << right << hex << setfill('0') << setw(16) << dec.decision(module) << std::dec << setfill(' ') << endl;

   
   if(dec.isValid()) {
      for(const TXC::TriggerLine & trigger : dec.m_triggers)
         o << "  " << setw(30) << left << trigger.name() << "  " << (dec.passed(trigger.module(), trigger.counter() % 64) ? "pass" : "fail") << endl;
   } else {
      for(const TXC::TriggerLine & trigger : dec.m_triggers)
         o << "  " << setw(30) << left << trigger.name() << "  unset" << endl;
   }
   
   return o;
}
//----------------------------------------------------------
void
GlobalDecision::print() const {

   if(!isValid()) 
      TRG_MSG_INFO("Note that the overall decision has not been calculated");

   for(unsigned int module = 0; module<3; ++module)
      TRG_MSG_INFO("Overall decision module " << module << ": 0x" << right << hex << setfill('0') << setw(16) << decision(module) << std::dec << setfill(' '));

   
   if(isValid()) {
      for(const TXC::TriggerLine & trigger : m_triggers)
         TRG_MSG_INFO("      " << setw(30) << left << trigger.name() << "  " << (passed(trigger.module(), trigger.counter() % 64) ? "pass" : "fail") );
   } else {
      for(const TXC::TriggerLine & trigger : m_triggers)
         TRG_MSG_INFO("      " << setw(30) << left << trigger.name() << "  unset" );
   }
   
}
}
