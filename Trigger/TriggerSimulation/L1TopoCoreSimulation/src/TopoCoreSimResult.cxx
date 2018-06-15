/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  TopoCoreSimResult.cxx
//  L1TopoCoreSimulation
//
//  Created by Joerg Stelzer on 11/29/12.
//

#include "L1TopoCoreSimulation/TopoCoreSimResult.h"
#include "L1TopoCoreSimulation/Connector.h"
#include "L1TopoCoreSimulation/ConfigurableAlg.h"
#include "L1TopoCoreSimulation/DataArray.h"
#include "L1TopoCoreSimulation/Exception.h"

#include <algorithm>

using namespace std;

TCS::TopoCoreSimResult::TopoCoreSimResult()
{}

TCS::TopoCoreSimResult::~TopoCoreSimResult()
{}


TCS::StatusCode
TCS::TopoCoreSimResult::collectResult() {
   StatusCode sc = m_globalDecision.collectDecision();

   return sc;
}

TCS::StatusCode
TCS::TopoCoreSimResult::reset() {
   StatusCode sc = m_globalDecision.resetDecision();

   return sc;
}


TCS::StatusCode
TCS::TopoCoreSimResult::setConfiguration(const std::vector<TCS::Connector*>& connectors) {
   StatusCode sc = StatusCode::SUCCESS;
   m_connectors.insert(m_connectors.begin(), connectors.begin(), connectors.end());
   // setting decision algs
   for(std::vector<TCS::Connector*>::const_iterator connIt = connectors.begin();
       connIt != connectors.end(); ++connIt)
   {
      const Connector & conn = **connIt;
      for(std::vector<TCS::ConfigurableAlg*>::const_iterator alg = conn.algorithms().begin();
          alg != conn.algorithms().end(); ++alg) {
         if((*alg)->isDecisionAlg()) {
            sc &= m_globalDecision.addDecisionAlg((DecisionAlg*) *alg);
         }
      }
   }
   return sc;
}


const TCS::DataArray*
TCS::TopoCoreSimResult::output(const std::string & connName) const {

   for(std::vector<TCS::Connector*>::const_iterator connIt = m_connectors.begin();
       connIt != m_connectors.end(); ++connIt)
   {
      if( (*connIt)->name() != connName ) continue;
      const TCS::DataArray* output = (*connIt)->outputData();
      return output;
   }
   TCS_EXCEPTION("No connector with name '" << connName << "'")
   return 0;
}


std::ostream&
operator<<(std::ostream& o, const TCS::TopoCoreSimResult & simRes) {
   o << simRes.globalDecision();
   for(std::vector<TCS::Connector*>::const_iterator connIt = simRes.m_connectors.begin();
          connIt != simRes.m_connectors.end(); ++connIt)
      {
         if( ! (*connIt)->isDecisionConnector() ) continue;

         const TCS::DataArray* output = (*connIt)->outputData();
         o << (*connIt)->name() << " produced output " << endl << *output << endl;
      }

   return o;
}
