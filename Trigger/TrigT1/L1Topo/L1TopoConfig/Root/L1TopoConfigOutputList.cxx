/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoConfig/L1TopoConfigOutputList.h"

#include "boost/foreach.hpp"

#include <iostream>
#include <algorithm>

using namespace std;
using namespace TXC;


void
L1TopoConfigOutputList::addOutputListElement(const OutputListElement & output) {
  m_outputListElements.push_back(output);
}


void
L1TopoConfigOutputList::addTriggerLine(const TriggerLine & trigger) {
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

std::ostream & operator<<(std::ostream &o, const TXC::L1TopoConfigOutputList &outputlist) {

   o << "Trigger lines (sorted by connectors):" << endl;
   unsigned int modIdx = 999;
   for(const TXC::TriggerLine& trigger: outputlist.getTriggerLines()) {
      if(trigger.counter() / 100 != modIdx) {
         modIdx = trigger.counter() / 100;
         o << "Module=" << trigger.module() << ", FPGA=" << trigger.fpga() << ", Clock=" << trigger.clock() << endl;
         o << "-------------------------" << endl;
      }
      o << "  " << trigger.name() <<  " (algorithm " << trigger.algoname() << " (id=" << trigger.algoid() << "))" <<  " on bit " << trigger.bit() << endl;
   }
   return o;
}

std::ostream & operator<<(std::ostream &o, const TXC::TriggerLine &trigger) {
   o << trigger.counter() << " : " << trigger.name() << " [" << trigger.module() << "/" << trigger.fpga() << "/" << trigger.clock() << "/" << trigger.bit() << "]";
   return o;
}

