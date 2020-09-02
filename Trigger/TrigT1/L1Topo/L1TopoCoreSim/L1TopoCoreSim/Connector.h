// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoCoreSim_Connector
#define L1TopoCoreSim_Connector

#include <iostream>
#include <vector>
#include <string>

#include "L1TopoCommon/StatusCode.h"
#include "L1TopoCommon/Types.h"


namespace TCS {

   class ConfigurableAlg;

   //    std::vector<std::string> vectorize(const std::string& str);   
   
   //    std::vector<std::string> vectorize2(const std::string &str1, const std::string &str2);
   
   class Connector {
   public:

      enum ConnectorType { NONE=0, INPUT, SORT, DECISION };
      
      // standard connector
      Connector( const std::string & name,
                 ConnectorType conntype );

      Connector( const std::string & name,
                 const std::vector<std::string> & inputnames,
                 const std::string & algorithm,
                 const std::vector<std::string> & outputnames,
                 ConnectorType conntype );
      
      virtual ~Connector();

      bool isInputConnector() const    { return m_conntype == INPUT; }
      bool isSortingConnector() const  { return m_conntype == SORT; }
      bool isDecisionConnector() const { return m_conntype == DECISION; }

      const std::string &              name() const { return m_name; }
      const std::string &              algorithmName() const {  return m_algorithmName; }
      const std::vector<std::string> & inputNames() const { return m_inputNames; }
      const std::vector<std::string> & outputNames() const { return m_outputNames; }

      const std::vector<TCS::Connector*> & inputConnectors() const { return m_inputConnectors; }
      const TCS::ConfigurableAlg* algorithm() const {  return m_algorithm; }

      virtual void setAlgorithm(TCS::ConfigurableAlg*);

      virtual StatusCode clearOutput() = 0;

      StatusCode reset();

      // accessors
      bool isExecuted() const { return m_isExecuted; }

      StatusCode executionStatusCode() const { return m_executionStatusCode; }

      unsigned int numberOutputBits() const;

      /**
         @brief to be toggled after 'attachOutputData' is called.
         This flag is reset with reset().
       */
      void toggleInputOverflow(const bool value);
      bool hasInputOverflow() const { return m_hasInputOverflow; }

   protected: // functions

      friend class TopoSteering;
      friend class TopoSteeringStructure;

      // setters
      void setIsExecuted(bool isExecuted) { m_isExecuted = isExecuted; }

      void setExecutionStatusCode(StatusCode statusCode) { m_executionStatusCode = statusCode; }

      std::vector<TCS::Connector*> & inputConnectors() { return m_inputConnectors; }

      TCS::ConfigurableAlg* algorithm() { return m_algorithm; }

   protected: // data

      // configuration of the connector: name, input, algorithms
      std::string              m_name{""};
      std::string              m_algorithmName{""};
      std::vector<std::string> m_inputNames;
      std::vector<std::string> m_outputNames;

      // the instances for input and algorithms (algorithms are owned by AlgFactory, connectors by the steering)
      std::vector<TCS::Connector*> m_inputConnectors;

      /** @brief the algorithm that belongs to this connector
       */
      TCS::ConfigurableAlg* m_algorithm { nullptr };

      // status information
      bool m_isExecuted {false};             // true if connector alg(s) have been executed on current event (must be reset at begin of event

      StatusCode m_executionStatusCode {StatusCode::SUCCESS}; // StatusCode of last execution (for cache)
      /**
         @brief whether the input data came with an overflow bit
       */
      bool m_hasInputOverflow;

   private:

      ConnectorType   m_conntype { NONE };

   };
   
}

std::ostream & operator<<(std::ostream &, const TCS::Connector&);

#endif
