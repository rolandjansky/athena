// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoCoreSim_DecisionConnector
#define L1TopoCoreSim_DecisionConnector

#include "L1TopoCoreSim/Connector.h"

#include "L1TopoInterfaces/Decision.h"
#include "L1TopoConfig/L1TopoConfigOutputList.h"

#include <vector>
#include <string>

namespace TCS {

   class TOBArray;
   class DecisionAlg;

   // special connector for Decision algorithms
   class DecisionConnector : public Connector {
   public:
      
      DecisionConnector(const std::string & name,
                        const std::vector<std::string> & inputs,
                        const std::string & algorithm,
                        const std::vector<std::string> & outputs);

      virtual ~DecisionConnector();

      virtual bool isDecisionConnector() const { return true; }

      TCS::DecisionAlg* decisionAlgorithm() const {  return m_decisionAlgorithm; }

      virtual StatusCode clearOutput();

      const Decision & decision() const { return m_decision; }

      bool decision(const std::string & trigger) const;

      const std::vector<TXC::TriggerLine> & triggers() const { return m_triggers; }

      // output data
      void attachOutputData(const std::vector<TOBArray *>&);

      TOBArray const * output(const std::string & trigger) const;
      
      const std::vector<TOBArray const *> & outputData() const { return m_outputData; }
      
   private: // functions

      friend class TopoSteering;
      friend class TopoSteeringStructure;

      void setAlgorithm(TCS::ConfigurableAlg* alg);

      void setFirstOutputBit(unsigned int firstOutputBit);

   private: // data

      // the decision bit array
      Decision m_decision;

      TCS::DecisionAlg* m_decisionAlgorithm;

      std::vector<TXC::TriggerLine> m_triggers;

      // attached output data
      std::vector<TOBArray const *>  m_outputData;

   };

std::ostream & operator<<(std::ostream &, const TCS::DecisionConnector&);

}

#endif
