/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_JEPROIBYTESTREAMV2TOOL_H
#define TRIGT1CALOBYTESTREAM_JEPROIBYTESTREAMV2TOOL_H

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "AthContainers/DataVector.h"
#include "eformat/SourceIdentifier.h"
#include "GaudiKernel/ToolHandle.h"
#include "CmxEnergySubBlock.h"

class IInterface;
class InterfaceID;
class StatusCode;

template <class T> class FullEventAssembler;

namespace LVL1 {
  class CMXEtSums;
  class CMXRoI;
  class JEMTobRoI;
  class JEPRoIBSCollectionV2;
}

namespace LVL1BS {

class JemRoiSubBlockV2;
class L1CaloErrorByteStreamTool;
class L1CaloSrcIdMap;

/** Tool to perform ROB fragments to JEM RoI and CMX RoI,
 *  and JEP RoI container to raw data conversions.
 *
 *  Based on ROD document version X_xxx.                                 <<== CHECK
 *
 *  @author Peter Faulkner
 */

class JepRoiByteStreamV2Tool : public AthAlgTool {

 public:
   JepRoiByteStreamV2Tool(const std::string& type, const std::string& name,
                          const IInterface* parent);
   virtual ~JepRoiByteStreamV2Tool();

   /// AlgTool InterfaceID
   static const InterfaceID& interfaceID();

   virtual StatusCode initialize() override;
   virtual StatusCode finalize() override;

   /// Convert ROB fragments to JEM RoIs
   StatusCode convert(const std::string& name,
                      DataVector<LVL1::JEMTobRoI>* jeCollection) const;
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::JEMTobRoI>* jeCollection) const;
   /// Convert ROB fragments to CMX RoIs
   StatusCode convert(const std::string& name,
                      LVL1::CMXRoI* cmCollection) const;
   
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      LVL1::CMXRoI* cmCollection) const;

   /// Convert JEP RoI Container to bytestream
   StatusCode convert(const LVL1::JEPRoIBSCollectionV2* jep) const;

   /// Return reference to vector with all possible Source Identifiers
   const std::vector<uint32_t>& sourceIDs(const std::string& sgKey) const;

 private:
   ServiceHandle<ByteStreamCnvSvc> m_byteStreamCnvSvc
   { this, "ByteStreamCnvSvc", "ByteStreamCnvSvc" };

   typedef DataVector<LVL1::JEMTobRoI>                   JemRoiCollection;
   typedef DataVector<LVL1::CMXEtSums>                   CmxSumsCollection;
   typedef std::map<uint32_t, const LVL1::JEMTobRoI*>    JemRoiMap;
   typedef std::map<int, const LVL1::CMXEtSums*>         CmxSumsMap;
   typedef IROBDataProviderSvc::VROBFRAG::const_iterator ROBIterator;
   typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      ROBPointer;
   typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      RODPointer;

   /// Convert bytestream to given container type
   StatusCode convertBs(const IROBDataProviderSvc::VROBFRAG& robFrags,
                        DataVector<LVL1::JEMTobRoI>* jeCollection,
                        LVL1::CMXRoI* cmCollection) const;

   ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
   /// Property: Error collection tool
   ToolHandle<LVL1BS::L1CaloErrorByteStreamTool> m_errorTool;

   /// Find CMX energy sums for given crate, source
   const LVL1::CMXEtSums*  findCmxSums(int crate, int source,
                                       const CmxSumsMap& cmxEtMap) const;

   std::vector<uint32_t> makeSourceIDs (bool roiDaq) const;

   /// Set up JEM RoIs map
   void setupJemRoiMap(const JemRoiCollection* jeCollection,
                       JemRoiMap&  roiMap) const;
   /// Set up CMX energy sums map
   void setupCmxEtMap(const CmxSumsCollection* enCollection,
                      CmxSumsMap& cmxEtMap) const;

   /// Get energy subBlock types from CMXEtSums source type
   void energySubBlockTypes(int source,
                            CmxEnergySubBlock::SourceType& srcType,
			    CmxEnergySubBlock::SumType&    sumType,
			    CmxEnergySubBlock::HitsType&   hitType) const;

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
   /// Number of RoI frames
   const int m_frames;
   /// Number of CMX energy RoI words
   const int m_maxRoiWords;
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
