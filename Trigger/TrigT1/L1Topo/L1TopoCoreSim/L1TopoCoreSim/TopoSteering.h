//  TopoSteering.h
//  L1TopoCoreSimulation
//  Created by Joerg Stelzer on 11/20/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#ifndef __L1TopoCoreSimulation__TopoSteering__
#define __L1TopoCoreSimulation__TopoSteering__

#include <bitset>
#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include "TrigConfBase/TrigConfMessaging.h"

#include "L1TopoCommon/StatusCode.h"
#include "L1TopoEvent/TopoInputEvent.h"
#include "L1TopoInterfaces/ParameterSpace.h"
#include "L1TopoCoreSim/TopoCoreSimResult.h"
#include "L1TopoCoreSim/TopoSteeringStructure.h"

namespace TXC {
   class L1TopoMenu;
}

class IL1TopoHistSvc;

namespace TCS {

   class InputConnector;
   class SortingConnector;
   class DecisionConnector;
   class ConfigurableAlg;
   class SortingAlg;
   class Decision;
   class DecisionAlg;

   class TopoSteering : public TrigConf::TrigConfMessaging {
   public:
     
      // default constructor
      TopoSteering();
     
      // destructor
      ~TopoSteering();

      const TopoSteeringStructure & structure() const { return m_structure; }

      TopoInputEvent & inputEvent() { return m_inputEvent; }

      const TopoCoreSimResult & simulationResult() const { return m_simulationResult; }


      // @brief: build the execution structure and parameterspace from
      // the configuration
      StatusCode setupFromConfiguration(const TXC::L1TopoMenu& menu);
     
      void setUseBitwise(bool useBitwise) { m_useBitwise = useBitwise; }
 
      // @brief: call the initialize function of the algorithms
      // will be called after the parameters are set and before the event loop starts
      StatusCode initializeAlgorithms();


      // run the topo simulation
      StatusCode executeEvent();
      
      StatusCode executeTrigger(const std::string & triggerName);

      // resets the inputEvent, connectors (status, intermediate TOBs,
      // and decision of algs), and the simulation result
      StatusCode reset();
      
      void printDebugInfo();

      const TCS::ParameterSpace & parameters(const std::string & algName) const;
      
      const std::vector<TCS::Connector*> & connectors() const { return structure().connectors(); } //added

      void printConfiguration(std::ostream & o) const;

      void setMsgLevel( TrigConf::MSGTC::Level lvl );

      void setAlgMsgLevel( TrigConf::MSGTC::Level lvl );

      /**
       * @brief enables the histogramming service
       */
      StatusCode setHistSvc(std::shared_ptr<IL1TopoHistSvc> histSvc);

      StatusCode saveHist();

      static const unsigned int numberOfL1TopoBits = 128;
      /**
         @brief cache the decision/overflow bits from hardware
         These bits are propagated to the algorithms with propagateHardwareBitsToAlgos.
       */
      void setHardwareBits(const std::bitset<numberOfL1TopoBits> &triggerBits,
                           const std::bitset<numberOfL1TopoBits> &ovrflowBits);
      /**
         @brief propagate the bits from hardware to each simulated decision algo.
         They will then be used to fill the accept/reject monitoring histograms.
       */
      void propagateHardwareBitsToAlgos();
      /**
         @brief tell output algos to fill accept/reject histos based on hdw decision.

         In this case you will need to call setHardwareBits +
         propagateHardwareBitsToAlgos at each event.
       */
      void setOutputAlgosFillBasedOnHardware(const bool &value);
      /**
         @brief skip filling the histos

         When filling the histograms based on the hdw decision we want
         to skip filling them if we didn't fetch the hdw bits from the
         ROS. The flag is then toggled on/off depending on the prescaler.
       */
      void setOutputAlgosSkipHistograms(const bool &value);
   private:

      // execution
      StatusCode executeConnector(TCS::Connector *conn);

      StatusCode executeInputConnector(TCS::InputConnector *conn);

      StatusCode executeSortingConnector(TCS::SortingConnector *conn);

      StatusCode executeDecisionConnector(TCS::DecisionConnector *conn);
      
      StatusCode executeAlgorithm(ConfigurableAlg *alg, Connector* connector);
      
      StatusCode executeSortingAlgorithm(SortingAlg *alg, TCS::InputConnector* inputConnector, TOBArray * & output);
      
      StatusCode executeDecisionAlgorithm(TCS::DecisionAlg *alg, const std::vector<Connector*> & inputConnectors, const std::vector<TCS::TOBArray *> & output, Decision & decsion);
      


   private:
      bool m_useBitwise{false};                  // Using bitwise algorithms? Disabled by default. Needs a menu global flag.

      TopoInputEvent         m_inputEvent;       // the input event

      TopoCoreSimResult      m_simulationResult; // the result of the execution
      
      TopoSteeringStructure  m_structure;

      unsigned int m_evtCounter = {1};
      
      TrigConf::MSGTC::Level m_AlgMsgLvl { TrigConf::MSGTC::WARNING };

      std::shared_ptr<IL1TopoHistSvc>  m_histSvc;

      std::bitset<numberOfL1TopoBits> m_triggerHdwBits;
      std::bitset<numberOfL1TopoBits> m_ovrflowHdwBits;
   };

   
} // end of namespace TCS

#endif /* defined(__L1TopoCoreSimulation__TopoSteering__) */
