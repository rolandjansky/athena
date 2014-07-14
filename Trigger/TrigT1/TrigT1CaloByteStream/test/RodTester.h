/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_RODTESTER_H
#define TRIGT1CALOBYTESTREAM_RODTESTER_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "DataModel/DataVector.h"

class ISvcLocator;
class StatusCode;

namespace LVL1 {
  class RODHeader;
}

namespace LVL1BS {

/** Algorithm to test ROD header bytestream conversions.
 *
 *  Just prints out the contents of the RODHeader objects
 *
 *  @author Peter Faulkner
 */

class RodTester : public AthAlgorithm {

 public:
   RodTester(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~RodTester();

   virtual StatusCode initialize();
   virtual StatusCode execute();
   virtual StatusCode finalize();

 private:
   typedef DataVector<LVL1::RODHeader> RodHeaderCollection;

   /// RODHeader container StoreGate key
   std::string m_rodHeaderLocation;
   /// Variant collection flags
   std::vector<std::string> m_flags;

};

} // end namespace

#endif
