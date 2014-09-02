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

TopoCoreSimResult::TopoCoreSimResult()
{}

TopoCoreSimResult::~TopoCoreSimResult()
{}


StatusCode
TopoCoreSimResult::collectResult() {
   return m_globalDecision.collectDecision(m_outputConnectors);
}

StatusCode
TopoCoreSimResult::reset() {
   return m_globalDecision.resetDecision();
}


StatusCode
TopoCoreSimResult::setupFromMenu(const TXC::L1TopoMenu & menu, 
                                 const std::map<std::string, TCS::DecisionConnector*>& outputConnectorMap) {

   m_outputConnectorMap = outputConnectorMap;

   for(auto & x : m_outputConnectorMap)
      m_outputConnectors.insert(x.second);

   m_globalDecision.setTriggerLines(menu.getL1TopoConfigOutputList().getTriggerLines());

   return StatusCode::SUCCESS;
}



const std::vector<const TCS::TOBArray*> &
TCS::TopoCoreSimResult::output(const std::string & connName) const {

   for( const DecisionConnector * conn : m_outputConnectors ) {
      if( conn->name() != connName ) continue;
      return conn->outputData();
   }
   TCS_EXCEPTION("No connector with name '" << connName << "'")
      return (*m_outputConnectors.begin())->outputData();
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
