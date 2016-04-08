/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_ZDCL1CALOERRORBYTESTREAMTOOL_H
#define TRIGT1CALOBYTESTREAM_ZDCL1CALOERRORBYTESTREAMTOOL_H

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"

class IInterface;
class InterfaceID;
class StatusCode;

//namespace LVL1BS {

/** Tool to accumulate ROB/ROD unpacking errors.
 *
 *  @author Peter Faulkner
 */

class ZdcL1CaloErrorByteStreamTool : public AthAlgTool {

 public:
   ZdcL1CaloErrorByteStreamTool(const std::string& type, const std::string& name,
                             const IInterface* parent);
   virtual ~ZdcL1CaloErrorByteStreamTool();

   /// AlgTool InterfaceID
   static const InterfaceID& interfaceID();

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /// Set ROB status error
   void robError(uint32_t robid, unsigned int err);
   /// Set ROD unpacking error
   void rodError(uint32_t robid, unsigned int err);
   /// Fill vector with accumulated errors and reset
   StatusCode errors(std::vector<unsigned int>* errColl);

 private:

   // Maps of accumulated errors
   typedef std::map<uint32_t, unsigned int> ErrorMap;
   ErrorMap m_robMap;
   ErrorMap m_rodMap;

};

//} // end namespace

#endif
