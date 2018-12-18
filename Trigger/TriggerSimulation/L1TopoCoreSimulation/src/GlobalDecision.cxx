/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  GlobalDecision.cxx
//  L1TopoCoreSimulation
//
//  Created by Joerg Stelzer on 11/25/12.
//

#include <iomanip>

#include "L1TopoCoreSimulation/GlobalDecision.h"
#include "L1TopoCoreSimulation/DecisionAlg.h"
#include "L1TopoCoreSimulation/Exception.h"

using namespace std;

TCS::StatusCode
TCS::GlobalDecision::addDecisionAlg(const DecisionAlg* alg) {
   m_algo[alg->name()] = alg;
   return StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::GlobalDecision::collectDecision() {
   m_decision = 0;
   for(algmap_t::const_iterator algIt = m_algo.begin();
       algIt != m_algo.end(); ++algIt)
   {
      const Decision& dec = algIt->second->decision();
      for(unsigned int bit = 0; bit<dec.nBits(); ++bit) {
         if(dec.bit(bit)) { // bit set?
            m_decision |= 0x1<<(dec.firstBit()+bit);
         }
      }
   }
   m_valid = true;
   return StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::GlobalDecision::resetDecision() {
   m_decision = 0;
   m_valid = false;
   return StatusCode::SUCCESS;
}

std::ostream&
operator<<(std::ostream& o, const TCS::GlobalDecision & dec) {
   if(!dec.isValid()) {
      o << "Overall decision has not been calculated";
      return o;
   }
   o << "Overall decision: 0x" << hex << setw(8) << setfill('0') << dec.decision() << std::dec << endl;
   for(TCS::GlobalDecision::algmap_t::const_iterator algIt = dec.m_algo.begin();
       algIt != dec.m_algo.end(); ++algIt)
   {
      const TCS::Decision& algDec = algIt->second->decision();
      o << algIt->second->name() << " " << algDec << endl;
   }
   return o;
}
