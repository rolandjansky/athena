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
#include "L1TopoCoreSim/CountingConnector.h"
#include "L1TopoConfig/L1TopoMenu.h"
#include "L1TopoInterfaces/ConfigurableAlg.h"
#include "L1TopoEvent/DataArray.h"

#include "L1TopoCommon/Exception.h"

#include "TrigConfData/L1Menu.h"

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
   auto x = m_triggerLocationDec.find(triggerName);
   if( x == end(m_triggerLocationDec) )
      TCS_EXCEPTION("No trigger with name '" << triggerName << "' defined");
   return x->second->decision(triggerName);
}

const TCS::TOBArray*
TopoCoreSimResult::triggerOutput(const std::string & triggerName) const {
   auto x = m_triggerLocationDec.find(triggerName);
   if( x == end(m_triggerLocationDec) )
      TCS_EXCEPTION("No trigger with name '" << triggerName << "' defined");
   return x->second->output(triggerName);
}



TCS::StatusCode
TopoCoreSimResult::collectResult(TCS::DecisionConnector* outputConn, TCS::CountingConnector* countConn) {
   TCS::StatusCode sc = TCS::StatusCode::SUCCESS;
   if ( (outputConn == nullptr ) && (countConn == nullptr) ) {
      sc = m_globalOutput.collectOutput(m_outputConnectors, m_countConnectors);
   } else if ( !(outputConn == nullptr ) && (countConn == nullptr) ){
      set<DecisionConnector*> c = { outputConn };
      sc = m_globalOutput.collectOutput(c, m_countConnectors);
   } else if ( (outputConn == nullptr ) && !(countConn == nullptr) ){
      set<CountingConnector*> c = { countConn };
      sc = m_globalOutput.collectOutput(m_outputConnectors, c);
   } else {
      set<DecisionConnector*> c1 = { outputConn };
      set<CountingConnector*> c2 = { countConn };
      sc = m_globalOutput.collectOutput(c1, c2);
   }
   return sc;
}


TCS::StatusCode
TopoCoreSimResult::reset() {
   return m_globalOutput.resetOutput();
}


TCS::StatusCode
TopoCoreSimResult::setupFromMenu(const std::map<std::string, TCS::DecisionConnector*>& outputConnectorMap, const std::map<std::string, TCS::CountingConnector*>& countConnectorMap) {

   // Set decision trigger lines

   m_outputConnectorMap = outputConnectorMap;

   vector<TrigConf::TriggerLine> triggerLinesDec;

   for(auto & x : m_outputConnectorMap) {
      // fill the set
      m_outputConnectors.insert(x.second);

      // fill the trigger line map (trigger name --> (TCS::DecisionConnector*,unsigned int index) )
      for( const TrigConf::TriggerLine & trigger : x.second->triggers() ) {
         m_triggerLocationDec[trigger.name()] = x.second;
	 triggerLinesDec.push_back(trigger);
      }
   }

   m_globalOutput.setDecisionTriggerLines(triggerLinesDec);

   // Set multiplicity trigger lines

   m_countConnectorMap = countConnectorMap;

   vector<TrigConf::TriggerLine> triggerLinesCount;

   for(auto & x : m_countConnectorMap) {
      // fill the set
      m_countConnectors.insert(x.second);

      // fill the trigger line map (trigger name --> (TCS::CountingConnector*,unsigned int index) )
      for( const TrigConf::TriggerLine & trigger : x.second->triggers() ) {
         m_triggerLocationCount[trigger.name()] = x.second;
	      triggerLinesCount.push_back(trigger);
      }
   }

   m_globalOutput.setMultiplicityTriggerLines(triggerLinesDec);

   return TCS::StatusCode::SUCCESS;
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

   o << simRes.globalOutput();

   for( const DecisionConnector * conn : simRes.m_outputConnectors ) {
      o << conn->name() << endl;
   //   for(const TrigConf::TriggerLine & trigger : conn->triggers()) {
	//         o << "  " << trigger << endl;
   //   }
      for(const TCS::TOBArray* output : conn->outputData())
         o << "  output " << output << endl;
   }

   return o;
}


void
TopoCoreSimResult::setMsgLevel( TrigConf::MSGTC::Level lvl ) {
   msg().setLevel( lvl );

   m_globalOutput.msg().setLevel( lvl );
}
