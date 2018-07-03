/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  Connector.cxx
//  L1TopoCoreSimulation
//
//  Created by Joerg Stelzer on 11/20/12.
//

#include "L1TopoCoreSimulation/Connector.h"
#include "L1TopoCoreSimulation/TopoInputEvent.h"
#include "L1TopoCoreSimulation/Exception.h"
#include "L1TopoCoreSimulation/ConfigurableAlg.h"

using namespace std;

std::vector<std::string>
TCS::Connector::vectorize(const std::string& str) {
   std::vector<std::string> s;
   s.push_back(str);
   return s;
}

TCS::Connector::Connector(const std::string & name,
                          inputTOBType_t inputType) :
   m_name(name)
   , m_connectorType(INPUT)
   , m_inputTOBType(inputType)
   , m_isExecuted(false)
   , m_executionStatusCode(StatusCode::SUCCESS)
{}

TCS::Connector::Connector(const std::string & name,
                          ctype_t connectorType,
                          const std::vector<std::string> & algorithms,
                          const std::vector<std::string> & inputConnectors) :
   m_name(name)
   , m_inputNames(inputConnectors)
   , m_algorithmNames(algorithms)
   , m_connectorType(connectorType)
   , m_isExecuted(false)
   , m_executionStatusCode(StatusCode::SUCCESS)
{}

// attaches the data produced by the connector alg or input
// does not take ownership
void
TCS::Connector::attachOutputData(const TCS::DataArray* data) {
   if(m_outputData != 0) {
      TCS_EXCEPTION("Trying to attach data to connector " << name() << " which has already output data attached")
   }
   m_outputData = data;
}


TCS::StatusCode
TCS::Connector::reset() {
   StatusCode sc(StatusCode::SUCCESS);

   m_outputData = 0;
   m_isExecuted = false;
   m_executionStatusCode = StatusCode::SUCCESS;
   for(std::vector<TCS::ConfigurableAlg*>::iterator alg = m_algorithms.begin();
       alg != m_algorithms.end(); ++alg)
   {
      sc &= (*alg)->reset();
   }
   return sc;
}
