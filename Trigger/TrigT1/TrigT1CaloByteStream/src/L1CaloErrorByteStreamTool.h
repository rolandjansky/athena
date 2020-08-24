/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_L1CALOERRORBYTESTREAMTOOL_H
#define TRIGT1CALOBYTESTREAM_L1CALOERRORBYTESTREAMTOOL_H

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/SlotSpecificObj.h"

class IInterface;
class InterfaceID;
class StatusCode;

namespace LVL1BS {

/** Tool to accumulate ROB/ROD unpacking errors.
 *
 *  @author Peter Faulkner
 */

class L1CaloErrorByteStreamTool : public AthAlgTool {

 public:
   L1CaloErrorByteStreamTool(const std::string& type, const std::string& name,
                             const IInterface* parent);
   virtual ~L1CaloErrorByteStreamTool();

   /// AlgTool InterfaceID
   static const InterfaceID& interfaceID();

   virtual StatusCode initialize() override;
   virtual StatusCode finalize() override;

   /// Set ROB status error
   void robError(uint32_t robid, unsigned int err) const;
   /// Set ROD unpacking error
   void rodError(uint32_t robid, unsigned int err) const;
   /// Fill vector with accumulated errors and reset
   StatusCode errors(std::vector<unsigned int>* errColl);

 private:

   // FIXME: do this in a sane way...
   typedef std::map<uint32_t, unsigned int> ErrorMap;
   struct ErrorMaps {
     // Maps of accumulated errors
     ErrorMap m_robMap;
     ErrorMap m_rodMap;
     std::mutex m_mutex;
   };
   mutable SG::SlotSpecificObj<ErrorMaps> m_maps;
};

} // end namespace

#endif
