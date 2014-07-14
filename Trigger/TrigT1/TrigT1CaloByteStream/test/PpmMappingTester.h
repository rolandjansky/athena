/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_PPMMAPPINGTESTER_H
#define TRIGT1CALOBYTESTREAM_PPMMAPPINGTESTER_H

#include <string>
#include <vector>

#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "DataModel/DataVector.h"

class ISvcLocator;
class StatusCode;

namespace LVL1 {
  class IL1CaloMappingTool;
}

namespace LVL1BS {

/** Algorithm to compare Trigger tower mapping tools.
 *
 *  @author Peter Faulkner
 */

class PpmMappingTester : public AthAlgorithm {

 public:
   PpmMappingTester(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~PpmMappingTester();

   virtual StatusCode initialize();
   virtual StatusCode execute();
   virtual StatusCode finalize();

 private:
   void etaPhiToChannel(ToolHandle<LVL1::IL1CaloMappingTool>& tool,
                                   std::vector<unsigned int>& chanIds);
   void channelToEtaPhi(ToolHandle<LVL1::IL1CaloMappingTool>& tool,
                        std::vector<double>& etas, std::vector<double>& phis,
			std::vector<int>& layers);
   bool notEqual(double item1, double item2);

   /// Mapping tool 1
   ToolHandle<LVL1::IL1CaloMappingTool> m_tool1;
   /// Mapping tool 2
   ToolHandle<LVL1::IL1CaloMappingTool> m_tool2;

   int m_timing;
   std::vector<double> m_etaBins;

};

} // end namespace

#endif
