/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_ZDCL1CALOERRORBYTESTREAMTOOL_H
#define TRIGT1CALOBYTESTREAM_ZDCL1CALOERRORBYTESTREAMTOOL_H

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <mutex>

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "CxxUtils/checker_macros.h"

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

   /// AlgTool InterfaceID
   static const InterfaceID& interfaceID();

   /// Set ROB status error
   void robError(uint32_t robid, unsigned int err) const;
   /// Set ROD unpacking error
   void rodError(uint32_t robid, unsigned int err) const;
   /// Fill vector with accumulated errors and reset
   StatusCode errors(std::vector<unsigned int>* errColl) const;

 private:

   // FIXME: do this in a sane way...
   typedef std::map<uint32_t, unsigned int> ErrorMap;
   struct ErrorMaps {
     // Maps of accumulated errors
     ErrorMap m_robMap;
     ErrorMap m_rodMap;
     std::mutex m_mutex;
   };
   mutable SG::SlotSpecificObj<ErrorMaps> m_maps ATLAS_THREAD_SAFE;
};

//} // end namespace

#endif
