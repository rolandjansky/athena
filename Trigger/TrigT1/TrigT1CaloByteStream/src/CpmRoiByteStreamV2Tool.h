/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CPMROIBYTESTREAMV2TOOL_H
#define TRIGT1CALOBYTESTREAM_CPMROIBYTESTREAMV2TOOL_H

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include "L1CaloSrcIdMap.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "AthContainers/DataVector.h"
#include "eformat/SourceIdentifier.h"
#include "GaudiKernel/ToolHandle.h"

class IInterface;
class InterfaceID;
class StatusCode;

template <typename> class FullEventAssembler;

namespace LVL1 {
  class CPMTobRoI;
}

namespace LVL1BS {

class CpmRoiSubBlockV2;
class L1CaloErrorByteStreamTool;
class L1CaloSrcIdMap;

/** Tool to perform ROB fragments to CPM TOB RoI and CPM TOB RoI to raw data
 *  conversions.
 *
 *  Based on ROD document version X_xxx.                                          <<== CHECK
 *
 *  @author Peter Faulkner
 */

class CpmRoiByteStreamV2Tool : public AthAlgTool {

 public:
   CpmRoiByteStreamV2Tool(const std::string& type, const std::string& name,
                          const IInterface* parent);
   virtual ~CpmRoiByteStreamV2Tool();

   /// AlgTool InterfaceID
   static const InterfaceID& interfaceID();

   virtual StatusCode initialize() override;

   /// Convert ROB fragments to CPM RoIs
   StatusCode convert(const std::string& robFrags,
                      DataVector<LVL1::CPMTobRoI>* roiCollection) const;
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CPMTobRoI>* roiCollection) const;

   /// Convert CPM RoI to bytestream
   StatusCode convert(const DataVector<LVL1::CPMTobRoI>* roiCollection,
                      RawEventWrite* re) const;

   /// Return reference to vector with all possible Source Identifiers
   const std::vector<uint32_t>& sourceIDs(const std::string& sgKey) const;

 private:

   typedef DataVector<LVL1::CPMTobRoI>                   CpmRoiCollection;
   typedef std::map<uint32_t, const LVL1::CPMTobRoI*>    CpmRoiMap;
   typedef IROBDataProviderSvc::VROBFRAG::const_iterator ROBIterator;
   typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      ROBPointer;
   typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      RODPointer;

   std::vector<uint32_t> makeSourceIDs (bool roiDaq) const;

   /// Set up CPM RoI map
   void setupCpmRoiMap(const CpmRoiCollection* roiCollection,
                       CpmRoiMap& roiMap) const;

   ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
   /// Property: Error collection tool
   ToolHandle<LVL1BS::L1CaloErrorByteStreamTool> m_errorTool;

   /// Property: Hardware crate number offset
   int m_crateOffsetHw;
   /// Property: Software crate number offset
   int m_crateOffsetSw;
   /// Property: Sub_block header version
   int m_version;
   /// Property: Data compression format
   int m_dataFormat;
   /// Number of crates
   const int m_crates;
   /// Number of CPM modules per crate
   const int m_modules;
   /// Property: Number of slinks per crate when writing out bytestream
   int m_slinks;
   /// Property: Minimum crate number when writing out bytestream
   int m_crateMin;
   /// Property: Maximum crate number when writing out bytestream
   int m_crateMax;
   /// Property: ROB source IDs
   std::vector<uint32_t> m_sourceIDsProp;
   /// Property: ROB source IDs for RoIB
   std::vector<uint32_t> m_sourceIDsRoIBProp;
   /// Sub-detector type
   const eformat::SubDetector m_subDetector;
   /// Source ID converter
   const L1CaloSrcIdMap m_srcIdMap;

   // Property: M7 format follows old specification, so we have two zeros 
   // as most significant bits instead of 0xa
   bool m_isM7Format;
};

} // end namespace

#endif
