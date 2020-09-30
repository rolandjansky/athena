/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoConfig/L1TopoConfigOutputList.h"
#include "L1TopoCommon/Exception.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;
using namespace TXC;

void
TriggerLine::calcCounter() { 
   //m_counter = 64 * m_module + 16 * m_fpga + 32 * m_clock + m_bit;
   m_counter = 2 * (32 * m_module + 16 * m_fpga + m_bit) + m_clock;
   if(m_counter>191) {
      TCS_EXCEPTION("Trigger line '" << *this << "' has illegal counter " << m_counter);
   }
}


void
L1TopoConfigOutputList::addOutputListElement(const OutputListElement & output) {
  m_outputListElements.push_back(output);
}


void
L1TopoConfigOutputList::addTriggerLine(const TriggerLine & trigger) {

   auto insname = m_triggernames.insert(trigger.name());
   if(!insname.second)
      TCS_EXCEPTION("Trigger line '" << trigger.name() << "' has been defined more than once");

   auto inscount = m_triggercounters.insert(trigger.counter());
   if(!inscount.second) {
      string triggerWithSameCounter("");
      for(auto & tl : m_triggerlines)
         if(tl.counter() == trigger.counter())
            triggerWithSameCounter = tl.name();
      TCS_EXCEPTION("Trigger line '" << trigger.name() << "' is on the same output line (" << trigger.counter() << ") as trigger line '" << triggerWithSameCounter << "'");
   }

   m_triggerlines.push_back(trigger);
   
}

bool
L1TopoConfigOutputList::hasTrigger(const std::string & trigger) const {
   for(const TriggerLine & t : m_triggerlines)
      if(t.name() == trigger) return true;
   return false;
}


const TriggerLine &
L1TopoConfigOutputList::getTrigger(const std::string & trigger) const {
   for(const TriggerLine & t : m_triggerlines)
      if(t.name() == trigger) return t;
   return m_undef;
}

namespace {
   bool comp(const TriggerLine & x, const TriggerLine & y) { return x.counter() < y.counter(); }
}

void
L1TopoConfigOutputList::sort() {
   std::sort( m_triggerlines.begin(), m_triggerlines.end(), comp);
}


namespace TXC {

std::ostream & operator<<(std::ostream &o, const L1TopoConfigOutputList &outputlist) {

   o << "Trigger lines (sorted by connectors):" << endl;
   for(const TXC::TriggerLine& trigger: outputlist.getTriggerLines()) {
      if(trigger.counter() % 32 == 0) {
         //modIdx = trigger.counter() / 100;
         o << endl << "Module=" << trigger.module() << ", FPGA=" << trigger.fpga() << endl;
         o << "----------------" << endl;
      }
      o << "  " << setiosflags(ios::left) << setw(30) << trigger.name() <<  "   (algorithm " << trigger.algoname() << " (id=" << trigger.algoid() << "))"
        <<  " on line " << trigger.bit() << " and clock " << trigger.clock() << endl;
   }
   return o;
}

std::ostream & operator<<(std::ostream &o, const TriggerLine &trigger) {
   o << trigger.counter() << " : " << trigger.name() << " [" << trigger.module() << "/" << trigger.fpga() << "/" << trigger.clock() << "/" << trigger.bit() << "]";
   return o;
}


} // namespace TXC
