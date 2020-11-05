/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_JEPROIBYTESTREAMV1TOOL_H
#define TRIGT1CALOBYTESTREAM_JEPROIBYTESTREAMV1TOOL_H

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include "L1CaloSrcIdMap.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "AthContainers/DataVector.h"
#include "eformat/SourceIdentifier.h"
#include "GaudiKernel/ToolHandle.h"

class IInterface;
class InterfaceID;
class StatusCode;

template <class T> class FullEventAssembler;

namespace LVL1 {
  class CMMJetHits;
  class CMMEtSums;
  class CMMRoI;
  class JEMRoI;
  class JEPRoIBSCollectionV1;
}

namespace LVL1BS {

class CmmEnergySubBlock;
class CmmJetSubBlock;
class JemRoiSubBlockV1;
class L1CaloErrorByteStreamTool;
class L1CaloSrcIdMap;

/** Tool to perform ROB fragments to JEM RoI and CMM RoI,
 *  and JEP RoI container to raw data conversions.
 *
 *  Based on ROD document version 1_09h.
 *
 *  @author Peter Faulkner
 */

class JepRoiByteStreamV1Tool : public AthAlgTool {

 public:
   JepRoiByteStreamV1Tool(const std::string& type, const std::string& name,
                          const IInterface* parent);
   virtual ~JepRoiByteStreamV1Tool();

   /// AlgTool InterfaceID
   static const InterfaceID& interfaceID();

   virtual StatusCode initialize() override;
   virtual StatusCode finalize() override;

   /// Convert ROB fragments to JEM RoIs
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::JEMRoI>* jeCollection) const;
   /// Convert ROB fragments to CMM RoIs
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      LVL1::CMMRoI* cmCollection) const;

   /// Convert JEP RoI Container to bytestream
   StatusCode convert(const LVL1::JEPRoIBSCollectionV1* jep) const;

   /// Return reference to vector with all possible Source Identifiers
   const std::vector<uint32_t>& sourceIDs(const std::string& sgKey) const;

 private:
   ServiceHandle<ByteStreamCnvSvc> m_byteStreamCnvSvc
   { this, "ByteStreamCnvSvc", "ByteStreamCnvSvc" };

   typedef DataVector<LVL1::JEMRoI>                      JemRoiCollection;
   typedef DataVector<LVL1::CMMJetHits>                  CmmHitsCollection;
   typedef DataVector<LVL1::CMMEtSums>                   CmmSumsCollection;
   typedef std::map<uint32_t, const LVL1::JEMRoI*>       JemRoiMap;
   typedef std::map<int, const LVL1::CMMJetHits*>        CmmHitsMap;
   typedef std::map<int, const LVL1::CMMEtSums*>         CmmSumsMap;
   typedef IROBDataProviderSvc::VROBFRAG::const_iterator ROBIterator;
   typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      ROBPointer;
   typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      RODPointer;

   /// Convert bytestream to given container type
   StatusCode convertBs(const IROBDataProviderSvc::VROBFRAG& robFrags,
                        DataVector<LVL1::JEMRoI>* jeCollection,
                        LVL1::CMMRoI* cmCollection) const;

   /// Property: Error collection tool
   ToolHandle<LVL1BS::L1CaloErrorByteStreamTool> m_errorTool;

   /// Find CMM hits for given crate, data ID
   const LVL1::CMMJetHits* findCmmHits(int crate, int dataID,
                                       const CmmHitsMap& cmmHitsMap) const;
   /// Find CMM energy sums for given crate, data ID
   const LVL1::CMMEtSums*  findCmmSums(int crate, int dataID,
                                       const CmmSumsMap& cmmEtMap) const;

   std::vector<uint32_t> makeSourceIDs (bool roiDaq) const;

   /// Set up JEM RoIs map
   void setupJemRoiMap(const JemRoiCollection* jeCollection,
                       JemRoiMap& roiMap) const;
   /// Set up CMM hits map
   void setupCmmHitsMap(const CmmHitsCollection* hitCollection,
                        CmmHitsMap& cmmHitsMap) const;
   /// Set up CMM energy sums map
   void setupCmmEtMap(const CmmSumsCollection* enCollection,
                      CmmSumsMap& cmmEtMap) const;

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
   /// Number of JEM modules per crate
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
};

} // end namespace

#endif
