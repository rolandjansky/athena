/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_ERRORTESTER_H
#define TRIGT1CALOBYTESTREAM_ERRORTESTER_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"

class ISvcLocator;
class StatusCode;

namespace LVL1BS {

/** Algorithm to test unpacking error bytestream conversions.
 *
 *  Just prints out the contents of the Error vector
 *
 *  @author Peter Faulkner
 */

class ErrorTester : public AthAlgorithm {

 public:
   ErrorTester(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~ErrorTester();

   virtual StatusCode initialize();
   virtual StatusCode execute();
   virtual StatusCode finalize();

 private:
   typedef std::vector<unsigned int> ErrorCollection;

   /// Error vector StoreGate key
   std::string m_errorVectorLocation;

};

} // end namespace

#endif
