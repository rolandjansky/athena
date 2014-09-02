//  GlobalDecision.cxx
//  L1TopoCoreSimulation
//  Created by Joerg Stelzer on 11/25/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#include <iomanip>

#include "L1TopoCoreSim/GlobalDecision.h"
#include "L1TopoCoreSim/DecisionConnector.h"
#include "L1TopoCommon/Exception.h"

using namespace std;
using namespace TCS;


StatusCode
GlobalDecision::collectDecision(const set<DecisionConnector*> & outconn) {
   resetDecision();

   for( const DecisionConnector * conn : outconn ) {

      const Decision& dec = conn->decision();

      unsigned int pos = 0; // for multi-output algorithms pos is the output index
      for(const TXC::TriggerLine & trigger : conn->triggers() ) {

         unsigned int bit = trigger.counter() % 64;  // line in module
         
         uint64_t & moduleDec = m_decision[trigger.module()];
         uint64_t mask(0x1);

         if( dec.bit(pos++) )  // bit set?
            moduleDec |= (mask << bit);
      }

   }
   m_valid = true;
   return StatusCode::SUCCESS;
}


StatusCode
GlobalDecision::resetDecision() {
   m_decision[0] = m_decision[1] = m_decision[2] = 0;
   m_valid = false;
   return StatusCode::SUCCESS;
}


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
