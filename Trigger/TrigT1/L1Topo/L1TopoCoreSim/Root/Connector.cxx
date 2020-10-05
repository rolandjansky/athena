// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#include "L1TopoCoreSim/Connector.h"

#include "L1TopoEvent/TopoInputEvent.h"
#include "L1TopoInterfaces/DecisionAlg.h"
#include "L1TopoInterfaces/ConfigurableAlg.h"
#include "L1TopoCommon/Exception.h"

#include "L1TopoCoreSim/DecisionConnector.h"
#include "L1TopoCoreSim/SortingConnector.h"
#include "L1TopoCoreSim/InputConnector.h"

#include "boost/foreach.hpp"

using namespace std;
using namespace TCS;
 
TCS::Connector::Connector( const std::string & name,
                           const std::vector<std::string> & inputs,
                           const std::string & algorithm,
                           const std::vector<std::string> & outputs,
                           ConnectorType conntype ) :
   m_name(name),
   m_algorithmName(algorithm),
   m_inputNames(inputs),
   m_outputNames(outputs),
   m_conntype(conntype)
{}



TCS::Connector::Connector( const std::string & name,
                           ConnectorType conntype ) :
   m_name(name),
   m_conntype(conntype)
{}


TCS::Connector::~Connector() {}


void
TCS::Connector::setAlgorithm(TCS::ConfigurableAlg* alg) {  // alg ownership is with the factory
   m_algorithm = alg;
}


TCS::StatusCode
TCS::Connector::reset() {
   StatusCode sc(StatusCode::SUCCESS);

   // clear the output data
   sc &= clearOutput();
   m_hasInputOverflow = false;

   // reset the algorithm
   if(m_algorithm)
      sc &= m_algorithm->reset();

   // reset the execution flags
   m_isExecuted = false;
   m_executionStatusCode = StatusCode::SUCCESS;

   return sc;
}

unsigned int
TCS::Connector::numberOutputBits() const {
   if(isDecisionConnector()) {
      return ((DecisionAlg*) algorithm())->numberOutputBits();
   } else {
      return 0;
   }
}

void TCS::Connector::toggleInputOverflow(const bool value)
{
    m_hasInputOverflow = value;
}


ostream &
operator<<(ostream &o, const TCS::Connector & conn) {
   try {
      o << dynamic_cast<const TCS::DecisionConnector&>(conn);
      return o;
   } catch(...) {}
   try {
      o << dynamic_cast<const TCS::SortingConnector&>(conn);
      return o;
   } catch(...) {}
   try {
      o << dynamic_cast<const TCS::InputConnector&>(conn);
      return o;
   } catch(...) {}

   return o;
}
