/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_RODHEADERBYTESTREAMTOOL_H
#define TRIGT1CALOBYTESTREAM_RODHEADERBYTESTREAMTOOL_H

#include <stdint.h>

#include <string>
#include <vector>

#include "eformat/SourceIdentifier.h"
#include "GaudiKernel/ToolHandle.h"

#include "L1CaloSrcIdMap.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "AthContainers/DataVector.h"

class IInterface;
class InterfaceID;
class StatusCode;
class IROBDataProviderSvc;

namespace LVL1 {
  class RODHeader;
}

namespace LVL1BS {

class L1CaloErrorByteStreamTool;
class L1CaloSrcIdMap;

/** Tool to perform ROB fragments to ROD Header conversions.
 *
 *  Based on ROD document version 1_09h.
 *
 *  @author Peter Faulkner
 */

class RodHeaderByteStreamTool : public AthAlgTool {

 public:
   RodHeaderByteStreamTool(const std::string& type, const std::string& name,
                           const IInterface* parent);
   virtual ~RodHeaderByteStreamTool();

   /// AlgTool InterfaceID
   static const InterfaceID& interfaceID();

   virtual StatusCode initialize() override;
   virtual StatusCode finalize() override;

   /// Convert ROB fragments to RODHeaders
   StatusCode convert(const std::string& name,
                      DataVector<LVL1::RODHeader>* rhCollection) const;

   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::RODHeader>* rhCollection) const;

   /// Return reference to vector with all possible Source Identifiers
   const std::vector<uint32_t>& sourceIDs(const std::string& sgKey) const;

 private:
   typedef DataVector<LVL1::RODHeader>                   RodHeaderCollection;
   typedef IROBDataProviderSvc::VROBFRAG::const_iterator ROBIterator;
   typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      ROBPointer;
   typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      RODPointer;

   /// Fill vector with ROB IDs for given sub-detector
   std::vector<uint32_t> makeAllRobIds() const;
   std::vector<uint32_t>
   makeRobIds(int numCrates, int crateOffset,
              const std::vector<int>& slinks, int daqOrRoi,
              eformat::SubDetector subdet,
              const std::vector<uint32_t>& prop) const;

   /// Return true if StoreGate key ends in given string
   bool isAppended(const std::string& sgKey, const std::string& flag) const;

  /// Service for reading bytestream
   ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
   /// Error collection tool
   ToolHandle<LVL1BS::L1CaloErrorByteStreamTool> m_errorTool;

   /// ROB source IDs
   std::vector<uint32_t> m_sourceIDsProp;
   std::vector<uint32_t> m_sourceIDsPPProp;
   std::vector<uint32_t> m_sourceIDsCPProp;
   std::vector<uint32_t> m_sourceIDsJEPProp;
   std::vector<uint32_t> m_sourceIDsCPRoIProp;
   std::vector<uint32_t> m_sourceIDsJEPRoIProp;
   std::vector<uint32_t> m_sourceIDsCPRoIBProp;
   std::vector<uint32_t> m_sourceIDsJEPRoIBProp;
   /// Source ID converter
   const L1CaloSrcIdMap m_srcIdMap;
};

} // end namespace

#endif
