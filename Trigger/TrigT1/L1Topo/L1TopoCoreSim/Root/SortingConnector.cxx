// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#include "L1TopoCoreSim/SortingConnector.h"

#include "L1TopoCoreSim/InputConnector.h"

#include "L1TopoCommon/StringUtils.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/SortingAlg.h"

using namespace std;
using namespace TCS;

SortingConnector::SortingConnector(const string & input,
                                   const string & algorithm,
                                   const string & output) :
   Connector(output, {input}, algorithm, {output}, SORT)
{}


SortingConnector::~SortingConnector() {
   clearOutput();
}


TCS::StatusCode
TCS::SortingConnector::clearOutput() {
   delete m_outputData;
   m_outputData = 0;
   return StatusCode::SUCCESS;
}


InputConnector* 
SortingConnector::inputConnector() const {
   return dynamic_cast<InputConnector*>(m_inputConnectors[0]);
}


void
SortingConnector::setAlgorithm(TCS::ConfigurableAlg* alg) {
   Connector::setAlgorithm(alg);
   m_sortingAlgorithm = dynamic_cast<SortingAlg*>(alg); // set a local copy
   if( ! m_sortingAlgorithm ) {
      TCS_EXCEPTION("Error: algorithm" << alg->name() << " is not a sorting alg");
   }
}


// attaches the sorted data to the connector (does take ownership)
void
SortingConnector::attachOutputData(const TOBArray* data) {
   if(m_outputData != 0) {
      TCS_EXCEPTION("Trying to attach data to input connector '" << name() << "' which has already data attached");
   }
   m_outputData = data;
}


namespace TCS {


ostream &
operator<<(ostream &o, const TCS::SortingConnector & conn) {
   o << conn.inputNames()[0] << " ---[ " << conn.algorithmName() << " ]---> " << conn.outputNames()[0];
   return o;
}


}
