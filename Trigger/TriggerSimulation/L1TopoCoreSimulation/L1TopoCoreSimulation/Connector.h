/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  Connector.h
//  L1TopoCoreSimulation
//
//  Created by Joerg Stelzer on 11/20/12.
//

#ifndef __L1TopoCoreSimulation__Connector__
#define __L1TopoCoreSimulation__Connector__

#include <iostream>
#include <vector>
#include <string>

#include "L1TopoCoreSimulation/StatusCode.h"
#include "L1TopoCoreSimulation/Types.h"

namespace TCS {
   
   class ConfigurableAlg;
   class TopoInputEvent;
   class DataArray;
   
   class Connector {
   public:
      
      enum ctype_t { INPUT=0, SORTED=1, DECISION=2 };

      // standard connector
      Connector(const std::string & name, ctype_t connectorType,
                const std::vector<std::string> & algorithms,
                const std::vector<std::string> & inputConnectors);
      
      // special connector for topo input
      Connector(const std::string & name,
                inputTOBType_t inputType);
      

      static std::vector<std::string> vectorize(const std::string& str);
      
      const std::string & name() const { return m_name; }
      const std::vector<std::string> & inputNames() const { return m_inputNames; }
      const std::vector<std::string> & algorithmNames() const {  return m_algorithmNames; }

      const std::vector<TCS::Connector*> & inputConnectors() const { return m_inputConnectors; }
      const std::vector<TCS::ConfigurableAlg*> & algorithms() const {  return m_algorithms; }

      StatusCode reset();

      // accessors
      bool isExecuted() const { return m_isExecuted; }
      StatusCode executionStatusCode() const { return m_executionStatusCode; }
      ctype_t connectorType() const { return m_connectorType; }
      bool isInputConnector() const { return connectorType() == INPUT; }
      bool isSortedConnector() const { return connectorType() == SORTED; }
      bool isDecisionConnector() const { return connectorType() == DECISION; }
      
      // setters
      void setIsExecuted(bool isExecuted) { m_isExecuted = isExecuted; }
      void setExecutionStatusCode(StatusCode statusCode) { m_executionStatusCode = statusCode; }

      // output data
      void attachOutputData(const DataArray*);
      const DataArray* outputData() const { return m_outputData; }
      
      inputTOBType_t inputTOBType() const { return m_inputTOBType; }
      
   private:
      friend class TopoSteering;
      // private accessors
      std::vector<TCS::Connector*> & inputConnectors() { return m_inputConnectors; }
      std::vector<TCS::ConfigurableAlg*> & algorithms() { return m_algorithms; }
      
      // configuration of the connector: name, input, algorithms
      std::string m_name;
      std::vector<std::string> m_inputNames;
      std::vector<std::string> m_algorithmNames;

      ctype_t m_connectorType;          // defines which type of connector we have (input, sorted, decision)

      inputTOBType_t m_inputTOBType;  // for input connectors it holds the type of inputData
      
      // the instances for input and algorithms (algorithms are owned by AlgFactory, connectors by the steering)
      std::vector<TCS::Connector*> m_inputConnectors;
      std::vector<TCS::ConfigurableAlg*> m_algorithms;

      // attached output data vector
      DataArray const * m_outputData;
      
      // status information
      bool m_isExecuted;                // true if connector alg(s) have been executed on current event (must be reset at begin of event
      StatusCode m_executionStatusCode; // StatusCode of last execution (for cache)
   };
   
}

#endif /* defined(__L1TopoCoreSimulation__Connector__) */
