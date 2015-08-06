/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CPMROIBYTESTREAMV2TOOL_H
#define TRIGT1CALOBYTESTREAM_CPMROIBYTESTREAMV2TOOL_H

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "DataModel/DataVector.h"
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

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /// Convert ROB fragments to CPM RoIs
   StatusCode convert(const std::string& robFrags,
                      DataVector<LVL1::CPMTobRoI>* roiCollection);
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CPMTobRoI>* roiCollection);

   /// Convert CPM RoI to bytestream
   StatusCode convert(const DataVector<LVL1::CPMTobRoI>* roiCollection,
                      RawEventWrite* re);

   /// Return reference to vector with all possible Source Identifiers
   const std::vector<uint32_t>& sourceIDs(const std::string& sgKey);

 private:

   typedef DataVector<LVL1::CPMTobRoI>                   CpmRoiCollection;
   typedef std::map<uint32_t, const LVL1::CPMTobRoI*>    CpmRoiMap;
   typedef IROBDataProviderSvc::VROBFRAG::const_iterator ROBIterator;
   typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      ROBPointer;
   typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      RODPointer;

   /// Set up CPM RoI map
   void setupCpmRoiMap(const CpmRoiCollection* roiCollection);

   ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
   /// Error collection tool
   ToolHandle<LVL1BS::L1CaloErrorByteStreamTool> m_errorTool;

   /// Hardware crate number offset
   int m_crateOffsetHw;
   /// Software crate number offset
   int m_crateOffsetSw;
   /// Sub_block header version
   int m_version;
   /// Data compression format
   int m_dataFormat;
   /// Number of crates
   int m_crates;
   /// Number of CPM modules per crate
   int m_modules;
   /// Number of slinks per crate when writing out bytestream
   int m_slinks;
   /// Minimum crate number when writing out bytestream
   int m_crateMin;
   /// Maximum crate number when writing out bytestream
   int m_crateMax;
   /// ROB source IDs
   std::vector<uint32_t> m_sourceIDs;
   /// ROB source IDs for RoIB
   std::vector<uint32_t> m_sourceIDsRoIB;
   /// Sub-detector type
   eformat::SubDetector m_subDetector;
   /// Source ID converter
   L1CaloSrcIdMap* m_srcIdMap;
   /// Sub-block for neutral format
   CpmRoiSubBlockV2* m_subBlock;
   /// CPM RoI map
   CpmRoiMap m_roiMap;
   /// ROD Status words
   std::vector<uint32_t>* m_rodStatus;
   /// ROD status map
   std::map<uint32_t, std::vector<uint32_t>* > m_rodStatusMap;
   /// Event assembler
   FullEventAssembler<L1CaloSrcIdMap>* m_fea;

   // M7 format follows old specification, so we have two zeros 
   // as most significant bits instead of 0xa
   bool m_isM7Format;

};

} // end namespace

#endif
