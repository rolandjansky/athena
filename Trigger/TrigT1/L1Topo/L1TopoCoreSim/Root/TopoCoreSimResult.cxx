/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  TopoCoreSimResult.cxx
//  L1TopoEvent
//  @authors
//  Christian Ohm <cohm@cern.ch>
//  Joerg Stelzer <stelzer@cern.ch>

#include "L1TopoCoreSim/TopoCoreSimResult.h"
#include "L1TopoCoreSim/DecisionConnector.h"
#include "L1TopoConfig/L1TopoMenu.h"
#include "L1TopoInterfaces/ConfigurableAlg.h"
#include "L1TopoEvent/DataArray.h"

#include "L1TopoCommon/Exception.h"

#include <algorithm>

using namespace std;
using namespace TCS;

TopoCoreSimResult::TopoCoreSimResult() :
   TrigConfMessaging("TopoCoreSimResult")
{}

TopoCoreSimResult::~TopoCoreSimResult()
{}


bool
TopoCoreSimResult::triggerDecision(const std::string & triggerName) const {
   auto x = m_triggerLocation.find(triggerName);
   if( x == end(m_triggerLocation) )
      TCS_EXCEPTION("No trigger with name '" << triggerName << "' defined");
   return x->second->decision(triggerName);
}

const TCS::TOBArray*
TopoCoreSimResult::triggerOutput(const std::string & triggerName) const {
   auto x = m_triggerLocation.find(triggerName);
   if( x == end(m_triggerLocation) )
      TCS_EXCEPTION("No trigger with name '" << triggerName << "' defined");
   return x->second->output(triggerName);
}



StatusCode
TopoCoreSimResult::collectResult(TCS::DecisionConnector* outputConn) {
   StatusCode sc = StatusCode::SUCCESS;
   if (outputConn == nullptr ) {
      sc = m_globalDecision.collectDecision(m_outputConnectors);
   } else {
      set<DecisionConnector*> c = { outputConn };
      sc = m_globalDecision.collectDecision(c);
   }
   return sc;
}


StatusCode
TopoCoreSimResult::reset() {
   return m_globalDecision.resetDecision();
}


StatusCode
TopoCoreSimResult::setupFromMenu(const TXC::L1TopoMenu & menu, 
                                 const std::map<std::string, TCS::DecisionConnector*>& outputConnectorMap) {

   m_outputConnectorMap = outputConnectorMap;

   for(auto & x : m_outputConnectorMap) {
      // fill the set
      m_outputConnectors.insert(x.second);

      // fill the trigger line map (trigger name --> (TCS::DecisionConnector*,unsigned int index) )
      for( const TXC::TriggerLine & trigger : x.second->triggers() ) {
         m_triggerLocation[trigger.name()] = x.second;
      }
   }

   m_globalDecision.setTriggerLines(menu.getL1TopoConfigOutputList().getTriggerLines());

   return StatusCode::SUCCESS;
}



const std::vector<const TCS::TOBArray*> &
TCS::TopoCoreSimResult::output(const std::string & connName) const {

   for( const DecisionConnector * conn : m_outputConnectors ) {
      if( conn->name() != connName ) continue;
      return conn->outputData();
   }

   // returns first matching connector as there should only be one, otherwise non match, arrive here and throw an exception.
   TCS_EXCEPTION("No connector with name '" << connName << "'");

   //return (*m_outputConnectors.begin())->outputData();
}




std::ostream&
operator<<(std::ostream& o, const TCS::TopoCoreSimResult & simRes) {

   o << simRes.globalDecision();

   for( const DecisionConnector * conn : simRes.m_outputConnectors ) {
      o << conn->name() << endl;
      for(const TXC::TriggerLine & trigger : conn->triggers()) {
         o << "  " << trigger << endl;
      }
      for(const TCS::TOBArray* output : conn->outputData())
         o << "  output " << output << endl;
   }

   return o;
}


void
TopoCoreSimResult::setMsgLevel( TrigConf::MSGTC::Level lvl ) {
   msg().setLevel( lvl );

   m_globalDecision.msg().setLevel( lvl );
}
