/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  TopoSteering.h
//  L1TopoCoreSimulation
//
//  Created by Joerg Stelzer on 11/20/12.
//

#ifndef __L1TopoCoreSimulation__TopoSteering__
#define __L1TopoCoreSimulation__TopoSteering__

#include <iostream>
#include <vector>

#include "L1TopoCoreSimulation/StatusCode.h"
#include "L1TopoCoreSimulation/TopoInputEvent.h"
//#include "L1TopoCoreSimulation/GlobalDecision.h"
#include "L1TopoCoreSimulation/TopoCoreSimResult.h"

namespace TCS {

   class Connector;
   class ConfigurableAlg;
   class SortingAlg;
   class DecisionAlg;
   class ParameterSpace;
   
   class TopoSteering {
   public:
      TopoSteering();
      
      // @brief: build the execution structure and parameterspace from the configuration
      //
      // configuration describes the algorithmic sequences and the list of triggers
      // paramterspace is the list of all parameters of all parameters of all algorithms
      //
      // TODO: this will be implemented once the configuration is available
      /*StatusCode setupFromConfiguration(const Configuration &);*/
      
      // in the meantime we have a mock configuration
      StatusCode setupMockConfiguration();
      
      // run the topo simulation
      StatusCode executeEvent();
      
      // TODO: resets the inputEvent, and the connectors (status, intermediate TOBs, and decision of algs)
      StatusCode reset();
      
      TopoInputEvent & inputEvent() { return m_inputEvent; }

      //      const GlobalDecision& globalDecision() const { return m_globalDecision; }

      const TopoCoreSimResult & simulationResult() const { return m_simulationResult; }

      void printDebugInfo();

   private:
      // steering functionality

      // configuration
      StatusCode instantiateAlgorithms();

      StatusCode linkConnectors();

      StatusCode setAlgorithmParameters(const ParameterSpace & parameterSpace);

      StatusCode checkConfigurationConsistency() const;
      
      Connector * findConnector(const std::string & connectorName) const;

      // execution
      StatusCode executeConnector(Connector *conn);
      
      StatusCode executeAlgorithm(ConfigurableAlg *alg, Connector* connector);
      
      StatusCode executeSortingAlgorithm(SortingAlg *alg, const std::vector<Connector*> & inputConnectors, DataArray * & output);
      
      StatusCode executeDecisionAlgorithm(TCS::DecisionAlg *alg, const std::vector<Connector*> & inputConnectors, DataArray * & output);
      
      // steering data
      TopoInputEvent m_inputEvent;  // the current input event

      // TODO: create TopoEvent class which holds input event and intermediate and output TOBs
      
      bool m_isConfigured;
      
      std::vector<std::string> m_output; // names of output connectors
      
      // TODO: also build a hash map from name to connector for fast lookup (see TrigDecisionTool)
      std::vector<TCS::Connector*> m_connectors; // list of connectors

      //      GlobalDecision m_globalDecision;

      TopoCoreSimResult m_simulationResult;
   };
   
   
} // end of namespace TCS

#endif /* defined(__L1TopoCoreSimulation__TopoSteering__) */
