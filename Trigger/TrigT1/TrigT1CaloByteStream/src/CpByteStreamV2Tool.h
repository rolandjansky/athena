/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CPBYTESTREAMV2TOOL_H
#define TRIGT1CALOBYTESTREAM_CPBYTESTREAMV2TOOL_H

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include "TrigT1CaloUtils/TriggerTowerKey.h"
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
  class CMXCPHits;
  class CMXCPTob;
  class CPMTower;
  class CPBSCollectionV2;
  class IL1CaloMappingTool;
  class TriggerTowerKey;
}

namespace LVL1BS {

class CmxCpSubBlock;
class CpmSubBlockV2;
class L1CaloErrorByteStreamTool;
class L1CaloSrcIdMap;

/** Tool to perform ROB fragments to CPM towers, CMX-CP TOBs and CMX-CP hits,
 *  and CP container to raw data conversions.
 *
 *  Based on ROD document version X_xxx.
 *
 *  @author Peter Faulkner
 */

class CpByteStreamV2Tool : public AthAlgTool {

 public:
   CpByteStreamV2Tool(const std::string& type, const std::string& name,
                                               const IInterface* parent);
   virtual ~CpByteStreamV2Tool();

   /// AlgTool InterfaceID
   static const InterfaceID& interfaceID();

   virtual StatusCode initialize() override;
   virtual StatusCode finalize() override;

   /// Convert ROB fragments to CPM towers
   StatusCode convert(const std::string& sgKey, DataVector<LVL1::CPMTower>* ttCollection) const;
   StatusCode convert(const std::string& sgKey,
                      const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CPMTower>* ttCollection) const;
   /// Convert ROB fragments to CMX-CP TOBs
   StatusCode convert(const std::string& sgKey,
                      DataVector<LVL1::CMXCPTob>* tobCollection) const;
   StatusCode convert(const std::string& sgKey,
                      const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CMXCPTob>* tobCollection) const;
   /// Convert ROB fragments to CMX-CP hits
   StatusCode convert(const std::string& sgKey,
                      DataVector<LVL1::CMXCPHits>* hitCollection) const;
   StatusCode convert(const std::string& sgKey,
                      const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CMXCPHits>* hitCollection) const;

   /// Convert CP Container to bytestream
   StatusCode convert(const LVL1::CPBSCollectionV2* cp, RawEventWrite* re) const;

   /// Return reference to vector with all possible Source Identifiers
   const std::vector<uint32_t>& sourceIDs() const;

 private:
   struct LocalData
   {
     /// Tower channels to accept (1=Core, 2=Overlap)
     int coreOverlap = 0;
     /// Unpacking error code
     unsigned int rodErr = 0;
     /// Energy vector for unpacking
     std::vector<int> energyVec;
     /// Isolation vector for unpacking
     std::vector<int> isolVec;
     /// TOB error vector for unpacking
     std::vector<int> errorVec;
     /// Presence map vector for unpacking
     std::vector<unsigned int> presenceMapVec;
     /// Hits0 vector for unpacking
     std::vector<unsigned int> hitsVec0;
     /// Hits1 vector for unpacking
     std::vector<unsigned int> hitsVec1;
     /// Error0 vector for unpacking
     std::vector<int> errVec0;
     /// Error1 vector for unpacking
     std::vector<int> errVec1;
     /// EM data vector for unpacking
     std::vector<int> emVec;
     /// Had data vector for unpacking
     std::vector<int> hadVec;
     /// EM error data vector for unpacking
     std::vector<int> emErrVec;
     /// Had error data vector for unpacking
     std::vector<int> hadErrVec;
     /// Trigger tower key provider
     LVL1::TriggerTowerKey towerKey;
   };

   enum CollectionType { CPM_TOWERS, CMX_CP_TOBS, CMX_CP_HITS };

   typedef DataVector<LVL1::CPMTower>                    CpmTowerCollection;
   typedef DataVector<LVL1::CMXCPTob>                    CmxCpTobCollection;
   typedef DataVector<LVL1::CMXCPHits>                   CmxCpHitsCollection;
   typedef std::map<unsigned int, LVL1::CPMTower*>       CpmTowerMap;
   typedef std::map<unsigned int, const LVL1::CPMTower*> ConstCpmTowerMap;
   typedef std::map<int, LVL1::CMXCPTob*>                CmxCpTobMap;
   typedef std::map<int, const LVL1::CMXCPTob*>          ConstCmxCpTobMap;
   typedef std::map<int, LVL1::CMXCPHits*>               CmxCpHitsMap;
   typedef std::map<int, const LVL1::CMXCPHits*>         ConstCmxCpHitsMap;
   typedef IROBDataProviderSvc::VROBFRAG::const_iterator ROBIterator;
   typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      ROBPointer;
   typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      RODPointer;

   struct CpByteStreamToolData
   {
     CpByteStreamToolData (const CollectionType collection)
       : m_collection(collection){}
     const CollectionType m_collection;
   };
   struct CpmTowerData : public CpByteStreamToolData
   {
     CpmTowerData (CpmTowerCollection* const ttCollection)
       : CpByteStreamToolData (CPM_TOWERS), m_ttCollection (ttCollection) {}
     CpmTowerCollection* const m_ttCollection;
     CpmTowerMap  m_ttMap;
   };
   struct CmxCpTobData : public CpByteStreamToolData
   {
     CmxCpTobData (CmxCpTobCollection* const tobCollection)
       : CpByteStreamToolData (CMX_CP_TOBS), m_tobCollection (tobCollection) {}
     CmxCpTobCollection* const m_tobCollection;
     CmxCpTobMap  m_tobMap;
   };
   struct CmxCpHitsData : public CpByteStreamToolData
   {
     CmxCpHitsData (CmxCpHitsCollection* const hitCollection)
       : CpByteStreamToolData (CMX_CP_HITS), m_hitCollection (hitCollection) {}
     CmxCpHitsCollection* const m_hitCollection;
     CmxCpHitsMap m_hitsMap;
   };

   /// Create list of all source IDs.
   std::vector<uint32_t> makeSourceIDs() const;

   /// Convert bytestream to given container type
   StatusCode convertBs(const std::string& sgKey,
                        const IROBDataProviderSvc::VROBFRAG& robFrags,
                        CpByteStreamToolData& data) const;
   /// Unpack CMX-CP sub-block
   void decodeCmxCp(CmxCpSubBlock* subBlock, int trigCpm, CpByteStreamToolData& data,
                    LocalData& ld) const;
   /// Unpack CPM sub-block
   void decodeCpm(CpmSubBlockV2* subBlock, int trigCpm, CpmTowerData& data,
                  LocalData& ld) const;

   /// Find a CPM tower for given key
   const LVL1::CPMTower*  findCpmTower(unsigned int key,
                                       const ConstCpmTowerMap& ttMap) const;
   LVL1::CPMTower*  findCpmTower(const CpmTowerData& data, unsigned int key) const;
   /// Find CMX-CP TOB for given key
   const LVL1::CMXCPTob*  findCmxCpTob(int key,
                                       const ConstCmxCpTobMap& tobMap) const;
   LVL1::CMXCPTob*  findCmxCpTob(const CmxCpTobData& data, int key) const;
   /// Find CMX-CP hits for given key
   const LVL1::CMXCPHits* findCmxCpHits(int key,
                                        const ConstCmxCpHitsMap& hitsMap) const;
   LVL1::CMXCPHits* findCmxCpHits(const CmxCpHitsData& data, int key) const;

   /// Set up CPM tower map
   void setupCpmTowerMap(const CpmTowerCollection* ttCollection,
                         ConstCpmTowerMap& ttMap,
                         LVL1::TriggerTowerKey& towerKey) const;
   /// Set up CMX-CP TOB map
   void setupCmxCpTobMap(const CmxCpTobCollection* tobCollection,
                         ConstCmxCpTobMap& tobMap) const;
   /// Set up CMX-CP hits map
   void setupCmxCpHitsMap(const CmxCpHitsCollection* hitCollection,
                          ConstCmxCpHitsMap& hitsMap) const;

   /// Key for TOBs
   int tobKey(int crate, int cmx, int cpm, int chip, int loc) const;
   /// Key for Hits
   int hitsKey(int crate, int cmx, int source) const;

   /// Get number of slices and triggered slice offset for next slink
   bool slinkSlices(int crate, int module, int modulesPerSlink,
                    int& timeslices, int& trigJem,
                    const ConstCpmTowerMap& ttMap,
                    const ConstCmxCpTobMap& tobMap,
                    const ConstCmxCpHitsMap& hitsMap,
                    LVL1::TriggerTowerKey& towerKey) const;

   /// Channel mapping tool
   ToolHandle<LVL1::IL1CaloMappingTool> m_cpmMaps;
   /// Error collection tool
   ToolHandle<LVL1BS::L1CaloErrorByteStreamTool> m_errorTool;
   ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
   /// Property: Hardware crate number offset
   int m_crateOffsetHw;
   /// Property: Software crate number offset
   int m_crateOffsetSw;
   /// Property: Sub_block header version
   int m_version;
   /// Property: Data compression format
   int m_dataFormat;
   /// Number of channels per module
   const int m_channels;
   /// Property: Number of crates
   int m_crates;
   /// Number of CPM modules per crate
   const int m_modules;
   /// Number of CMXs per crate
   const int m_cmxs;
   /// Maximum number of TOBS per module
   const int m_maxTobs;
   /// Number of chips
   const int m_chips;
   /// Number of Local coordinates
   const int m_locs;
   /// Property: Number of slinks per crate when writing out bytestream
   int m_slinks;
   /// Property: Default number of slices in simulation
   int m_dfltSlices;
   /// Property: Force number of slices in bytestream
   int m_forceSlices;
   /// Property: Minimum crate number when writing out bytestream
   int m_crateMin;
   /// Property: Maximum crate number when writing out bytestream
   int m_crateMax;
   /// Sub-detector type
   const eformat::SubDetector m_subDetector;
   /// Source ID converter
   const L1CaloSrcIdMap m_srcIdMap;
   /// Property: ROB source IDs
   std::vector<uint32_t> m_sourceIDsProp;
};

} // end namespace

#endif
