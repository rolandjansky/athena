/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CPMROIBYTESTREAMTOOL_H
#define TRIGT1CALOBYTESTREAM_CPMROIBYTESTREAMTOOL_H

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
  class CPMRoI;
}

namespace LVL1BS {

class CpmRoiSubBlock;
class L1CaloErrorByteStreamTool;
class L1CaloSrcIdMap;

/** Tool to perform ROB fragments to CPM RoI and CPM RoI to raw data
 *  conversions.
 *
 *  Based on ROD document version 1_09h.
 *
 *  @author Peter Faulkner
 */

class CpmRoiByteStreamTool : public AthAlgTool {

 public:
   CpmRoiByteStreamTool(const std::string& type, const std::string& name,
                        const IInterface* parent);
   virtual ~CpmRoiByteStreamTool();

   /// AlgTool InterfaceID
   static const InterfaceID& interfaceID();

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /// Convert ROB fragments to CPM RoIs
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CPMRoI>* roiCollection);

   /// Convert CPM RoI to bytestream
   StatusCode convert(const DataVector<LVL1::CPMRoI>* roiCollection,
                      RawEventWrite* re);

   /// Return reference to vector with all possible Source Identifiers
   const std::vector<uint32_t>& sourceIDs(const std::string& sgKey);

 private:

   typedef DataVector<LVL1::CPMRoI>                      CpmRoiCollection;
   typedef std::map<uint32_t, const LVL1::CPMRoI*>       CpmRoiMap;
   typedef IROBDataProviderSvc::VROBFRAG::const_iterator ROBIterator;
   typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      ROBPointer;
   typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      RODPointer;

   /// Set up CPM RoI map
   void setupCpmRoiMap(const CpmRoiCollection* roiCollection);

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
   /// ROB source IDs
   std::vector<uint32_t> m_sourceIDs;
   /// ROB source IDs for RoIB
   std::vector<uint32_t> m_sourceIDsRoIB;
   /// Sub-detector type
   eformat::SubDetector m_subDetector;
   /// Source ID converter
   L1CaloSrcIdMap* m_srcIdMap;
   /// Sub-block for neutral format
   CpmRoiSubBlock* m_subBlock;
   /// CPM RoI map
   CpmRoiMap m_roiMap;
   /// ROD Status words
   std::vector<uint32_t>* m_rodStatus;
   /// ROD status map
   std::map<uint32_t, std::vector<uint32_t>* > m_rodStatusMap;
   /// Event assembler
   FullEventAssembler<L1CaloSrcIdMap>* m_fea;

};

} // end namespace

#endif
