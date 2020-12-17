/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <iomanip>

#include "L1TopoCoreSim/GlobalDecision.h"
#include "L1TopoCoreSim/DecisionConnector.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/DecisionAlg.h"

using namespace std;
using namespace TCS;

void
GlobalDecision::setTriggerLines(const vector<TrigConf::TriggerLine> & triggers) {

  m_triggers = triggers;
  vector<string> connNames = {};
  for (const TrigConf::TriggerLine & trigger : triggers){
    auto it = find(connNames.begin(), connNames.end(), trigger.connName());
    if (it == connNames.end()){
      connNames.push_back(trigger.connName());
      m_decision[trigger.connName()] = 0;
    }
  }
}
  
uint32_t
GlobalDecision::decision_field(string connName, unsigned int clock) const {
   try {
      if(clock==0) {
         // lower 32 bit
         return (uint32_t) (m_decision.at(connName) & 0xffffffff);
      } else {
         // upper 32 bit
         uint64_t clock1 = m_decision.at(connName) & 0xffffffff00000000;
         return (uint32_t) (clock1 >> 32);
      }
   }
   catch(std::exception &) {
      TRG_MSG_ERROR("Connector name " << connName << " unknown");
      throw;
   }
}

uint32_t
GlobalDecision::overflow_field(string connName, unsigned int clock) const {
   if(clock==0) {
      // lower 32 bit
      return (uint32_t) (m_overflow.find(connName)->second & 0xffffffff);
   } else {
      // upper 32 bit
      uint64_t clock1 = m_overflow.find(connName)->second & 0xffffffff00000000;
      return (uint32_t) (clock1 >> 32);
   }
}


GlobalDecision::GlobalDecision(const std::string &name) :
   TrigConfMessaging(name)
{}

/****************************************************************
 *
 * Fill the decision bits with the pass/fail of all decision algs.
 * This is called by the TopoSteering after the connectors have
 * been executed.
 *
 ****************************************************************/

TCS::StatusCode
GlobalDecision::collectDecision(const set<DecisionConnector*> & outconn) {
   resetDecision();

   for( const DecisionConnector * conn : outconn ) {

      const Decision& dec = conn->decision();

      unsigned int pos = 0; // for multi-output algorithms pos is the output index
      for(const TrigConf::TriggerLine & trigger : conn->triggers() ) {
         
	      unsigned int position = trigger.startbit() + 32*trigger.fpga() + 16*trigger.clock();

         uint64_t & connectorDec = m_decision[trigger.connName()];
         uint64_t & connectorOvf = m_overflow[trigger.connName()];
         uint64_t mask(0x1);

         if( dec.bit(pos++) )  // bit set?
            connectorDec |= (mask << position);
         if( dec.overflow())
            connectorOvf |= (mask << position);
      }

   }
   m_valid = true;
   return TCS::StatusCode::SUCCESS;
}


TCS::StatusCode
GlobalDecision::resetDecision() {
   for(auto const& dec : m_decision)
     m_decision[dec.first] = 0;
   for(auto const& ovf : m_overflow)
     m_overflow[ovf.first] = 0;
   m_valid = false;
   return TCS::StatusCode::SUCCESS;
}


namespace TCS {


std::ostream&
operator<<(std::ostream& o, const TCS::GlobalDecision & dec) {

   if(!dec.isValid()) 
      o << "Note that the overall decision has not been calculated" << endl;

   for(auto const& itdec : dec.m_decision)
      o << "Overall decision for connector " << itdec.first << ": 0x" << right << hex << setfill('0') << setw(16) << dec.decision_field(itdec.first) << std::dec << setfill(' ') << endl;
   
   if(dec.isValid()) {
     for(const TrigConf::TriggerLine & trigger : dec.m_triggers){
	 unsigned int position = trigger.startbit() + 32*trigger.fpga() + 16*trigger.clock();
	 o << "  " << setw(30) << left << trigger.name() << "  " << (dec.passed(trigger.connName(), position) ? "pass" : "fail") << endl;}
   } else {
      for(const TrigConf::TriggerLine & trigger : dec.m_triggers)
	o << "  " << setw(30) << left << trigger.name() << "  unset" << endl;
   }
   return o;
}
//----------------------------------------------------------
void
GlobalDecision::print() const {

   if(!isValid()) 
      TRG_MSG_INFO("Note that the overall decision has not been calculated");

   for(auto const& dec : m_decision)
      TRG_MSG_INFO("Overall decision from connector " << dec.first << ": 0x" << right << hex << setfill('0') << setw(16) << decision_field(dec.first) << std::dec << setfill(' '));

   if(isValid()) {
      for(const TrigConf::TriggerLine & trigger : m_triggers){
	 unsigned int position = trigger.startbit() + 32*trigger.fpga() + 16*trigger.clock();
 	 TRG_MSG_INFO("      " << setw(30) << left << trigger.name() << "  " << (passed(trigger.connName(), position) ? "pass" : "fail") );}
   } else {
      for(const TrigConf::TriggerLine & trigger : m_triggers)
         TRG_MSG_INFO("      " << setw(30) << left << trigger.name() << "  unset" );
   }
}

}
