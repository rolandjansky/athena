/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoCoreSim_CountingConnector
#define L1TopoCoreSim_CountingConnector

#include "L1TopoCoreSim/Connector.h"

#include "L1TopoInterfaces/Count.h"
#include "L1TopoConfig/L1TopoConfigOutputList.h"
#include "L1TopoCommon/StatusCode.h"
#include "L1TopoCommon/Types.h"
#include "TrigConfData/L1Connector.h"

#include <vector>
#include <string>

namespace TCS {

   class TOBArray;
   class CountingAlg;
   class InputConnector;

   //special connector for Multiplicity Algorithms
   class CountingConnector : public Connector {
   public:

      CountingConnector(const std::string & name,
			               const std::string & input,
			               const std::string & algorithm,
			               const std::string & output);

      virtual ~CountingConnector();
      
      InputConnector* inputConnector() const;

      virtual bool isCountingConnector() const {  return true; }

      TCS::CountingAlg* countingAlgorithm() const { return m_countingAlgorithm; }
      
      virtual StatusCode clearOutput();

      const Count & count() const { return m_count; }

      const std::vector<TrigConf::TriggerLine> & triggers() const { return m_triggers; }

      //output data
      void attachOutputData(const std::vector<TOBArray *>&);

      TOBArray const * output(const std::string & trigger) const;

      const std::vector<TOBArray const *> & outputData() const { return m_outputData; }

   private: // functions

      friend class TopoSteering;
      friend class TopoSteeringStructure;

      void setAlgorithm(TCS::ConfigurableAlg* alg);
      
      void setFirstOutputBit(unsigned int firstOutputBit);

   private: // data

      // the count bit array
      Count m_count;

      TCS::CountingAlg* m_countingAlgorithm;

      std::vector<TrigConf::TriggerLine> m_triggers;

      // attached output data
      std::vector<TOBArray const *> m_outputData;

   };

   std::ostream & operator<<(std::ostream &, const TCS::CountingConnector&);

}

#endif
