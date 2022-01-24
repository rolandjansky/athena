/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "L1TopoCoreSim/CountingConnector.h"

#include "L1TopoCoreSim/InputConnector.h"

#include "L1TopoCommon/StringUtils.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/CountingAlg.h"

using namespace std;
using namespace TCS;

CountingConnector::CountingConnector(const string & name,
				     const string & input,
				     const string & algorithm,
				     const string & output) : 
   Connector(name, {input}, algorithm, {output}, COUNT),
   m_countingAlgorithm(0)
{}

CountingConnector::~CountingConnector() {
   CountingConnector::clearOutput();
}

// TO-DO: Add function to return the counts, but format? 



TCS::TOBArray const *
TCS::CountingConnector::output(const std::string & trigger) const {
   unsigned int index(0);
   for(const TrigConf::TriggerLine & tl : m_triggers) {
      if(tl.name() == trigger)
 	 return m_outputData[index];
      ++index;
   }
   TCS_EXCEPTION("Counting connector '" << name() << "' has no output trigger '" << trigger << "'");
   return nullptr;
}


TCS::StatusCode
TCS::CountingConnector::clearOutput() { 
   for(TOBArray const * x : m_outputData) {
      delete x;
   }
   m_outputData.clear();

   // set counting to 0
   m_count.reset();

   return StatusCode::SUCCESS;
}

InputConnector*
CountingConnector::inputConnector() const {
   return dynamic_cast<InputConnector*>(m_inputConnectors[0]);
}

void
CountingConnector::setAlgorithm(TCS::ConfigurableAlg* alg) {
   Connector::setAlgorithm(alg);
   m_countingAlgorithm = dynamic_cast<CountingAlg*>(alg);
   if(m_countingAlgorithm == nullptr && alg != nullptr) {
      TCS_EXCEPTION("Error: algorithm " << alg->name() << " is not a CountingAlg");
   }
}

// attaches the selected data to the connector (does take ownership)
void
CountingConnector::attachOutputData(const std::vector<TOBArray *>& data) {
   if(m_outputData.size() != 0) {
      TCS_EXCEPTION("Trying to attach data to multiplicity connector '" << name() <<"' which has already data attached");
   }

   for(TOBArray * x : data)
      m_outputData.push_back( dynamic_cast<const TOBArray*>(x) );
}


void
CountingConnector::setFirstOutputBit(unsigned int firstOutputBit) {
   m_count.setFirstBit(firstOutputBit);
}

namespace TCS {

  ostream &
  operator<<(ostream&o, const TCS::CountingConnector & conn) {
    o << "(" << join(conn.inputNames()) << ") ---[" << conn.algorithmName() << "]---> (" << join(conn.outputNames()) << ")";
    return o;

  }
}
