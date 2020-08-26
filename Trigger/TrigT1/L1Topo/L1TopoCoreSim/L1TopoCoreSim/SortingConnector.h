// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoCoreSim_SortingConnector
#define L1TopoCoreSim_SortingConnector

#include "L1TopoCoreSim/Connector.h"

#include <iostream>
#include <vector>
#include <string>

#include "L1TopoCommon/StatusCode.h"
#include "L1TopoCommon/Types.h"
#include "L1TopoInterfaces/Decision.h"


namespace TCS {

   class SortingAlg;
   class InputConnector;
   class TOBArray;

   // special connector for sorting algorithms
   class SortingConnector : public Connector {
   public:
      
      SortingConnector(const std::string & output,
                       const std::string & algorithms,
                       const std::string & input);

      virtual ~SortingConnector();

      InputConnector* inputConnector() const;

      std::string inputName() const { return m_inputNames.size()>0 ? m_inputNames[0] : ""; }

      std::string outputName() const { return m_outputNames.size()>0 ? m_outputNames[0] : ""; }

      void attachOutputData(const TOBArray* data);

      const TOBArray* outputData() const { return m_outputData; }

      void setAlgorithm(ConfigurableAlg* alg);

      SortingAlg* sortingAlgorithm() {  return m_sortingAlgorithm; }

      StatusCode clearOutput();

   private:

      SortingAlg* m_sortingAlgorithm;

      // attached sorted data
      TOBArray const * m_outputData { nullptr };

   };
   
std::ostream & operator<<(std::ostream &, const TCS::SortingConnector&);

}

#endif
