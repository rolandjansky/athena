// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#include "L1TopoCoreSim/DecisionConnector.h"

#include "L1TopoCommon/StringUtils.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/DecisionAlg.h"

using namespace std;
using namespace TCS;

DecisionConnector::DecisionConnector(const std::string & name,
                                     const vector<string> & inputs,
                                     const string & algorithm,
                                     const vector<string> & outputs) :
   Connector(name, inputs, algorithm, outputs, DECISION),
   m_decisionAlgorithm(0)
{}

DecisionConnector::~DecisionConnector() {
   clearOutput();
}

bool
TCS::DecisionConnector::decision(const std::string & trigger) const {
   unsigned int index(0);
   for(const TXC::TriggerLine & tl : m_triggers) {
      if(tl.name() == trigger)
         return m_decision.bit(index);
      ++index;
   }
   TCS_EXCEPTION("Decision connector '" << name() << "' has no output trigger '" << trigger << "'");
   return false;
}


TCS::TOBArray const *
TCS::DecisionConnector::output(const std::string & trigger) const {
   unsigned int index(0);
   for(const TXC::TriggerLine & tl : m_triggers) {
      if(tl.name() == trigger)
         return m_outputData[index];
      ++index;
   }
   TCS_EXCEPTION("Decision connector '" << name() << "' has no output trigger '" << trigger << "'");
   return nullptr;
}


TCS::StatusCode
TCS::DecisionConnector::clearOutput() {
   for(TOBArray const * x : m_outputData) {
      delete x;
   }
   m_outputData.clear();

   // set decision to 0
   m_decision.reset();
   
   return StatusCode::SUCCESS;
}

void
DecisionConnector::setAlgorithm(TCS::ConfigurableAlg* alg) {
   Connector::setAlgorithm(alg);
   m_decisionAlgorithm = dynamic_cast<DecisionAlg*>(alg);
   if(! m_decisionAlgorithm ) {
      cerr << "Error: algorithm" << alg->name() << " is not a DecisionAlg" << endl;
   }
}


// attaches the sorted data to the connector (does take ownership)
void
DecisionConnector::attachOutputData(const std::vector<TOBArray *>& data) {
   if(m_outputData.size() != 0) {
      TCS_EXCEPTION("Trying to attach data to decision connector '" << name() << "' which has already data attached");
   }

   //std::vector<TOBArray const *>  m_outputData;
   for(TOBArray * x : data)
      m_outputData.push_back( (TOBArray const *) x );
}


void
DecisionConnector::setFirstOutputBit(unsigned int firstOutputBit) {
   m_decision.setFirstBit(firstOutputBit);
}


namespace TCS {


ostream &
operator<<(ostream &o, const TCS::DecisionConnector & conn) {
   o << "(" << join(conn.inputNames()) << ") ---[ " << conn.algorithmName() << " ]---> (" << join(conn.outputNames()) << ")";
   return o;
}


}
