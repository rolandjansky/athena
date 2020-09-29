/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CPBYTESTREAMTOOL_H
#define TRIGT1CALOBYTESTREAM_CPBYTESTREAMTOOL_H

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
  class CMMCPHits;
  class CPMHits;
  class CPMTower;
  class CPBSCollection;
  class IL1CaloMappingTool;
  class TriggerTowerKey;
}

namespace LVL1BS {

class CmmCpSubBlock;
class CpmSubBlock;
class L1CaloErrorByteStreamTool;
class L1CaloSrcIdMap;

/** Tool to perform ROB fragments to CPM towers, CPM hits and CMM-CP hits,
 *  and CP container to raw data conversions.
 *
 *  Based on ROD document version 1_09h.
 *
 *  @author Peter Faulkner
 */

class CpByteStreamTool : public AthAlgTool {

 public:
   CpByteStreamTool(const std::string& type, const std::string& name,
                                             const IInterface* parent);
   virtual ~CpByteStreamTool();

   /// AlgTool InterfaceID
   static const InterfaceID& interfaceID();

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /// Convert ROB fragments to CPM towers
   StatusCode convert(const std::string& sgKey,
                      const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CPMTower>* ttCollection) const;
   /// Convert ROB fragments to CPM hits
   StatusCode convert(const std::string& sgKey,
                      const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CPMHits>* hitCollection) const;
   /// Convert ROB fragments to CMM-CP hits
   StatusCode convert(const std::string& sgKey,
                      const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CMMCPHits>* hitCollection) const;

   /// Convert CP Container to bytestream
   StatusCode convert(const LVL1::CPBSCollection* cp, RawEventWrite* re) const;

   /// Return reference to vector with all possible Source Identifiers
   const std::vector<uint32_t>& sourceIDs() const;

 private:
   struct LocalData
   {
     /// Tower channels to accept (1=Core, 2=Overlap)
     int coreOverlap;
     /// Unpacking error code
     unsigned int rodErr = 0;
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
     LVL1::TriggerTowerKey* towerKey;
   };

   enum CollectionType { CPM_TOWERS, CPM_HITS, CMM_CP_HITS };

   typedef DataVector<LVL1::CPMTower>                    CpmTowerCollection;
   typedef DataVector<LVL1::CPMHits>                     CpmHitsCollection;
   typedef DataVector<LVL1::CMMCPHits>                   CmmCpHitsCollection;
   typedef std::map<unsigned int, LVL1::CPMTower*>       CpmTowerMap;
   typedef std::map<unsigned int, const LVL1::CPMTower*> ConstCpmTowerMap;
   typedef std::map<int, LVL1::CPMHits*>                 CpmHitsMap;
   typedef std::map<int, const LVL1::CPMHits*>           ConstCpmHitsMap;
   typedef std::map<int, LVL1::CMMCPHits*>               CmmCpHitsMap;
   typedef std::map<int, const LVL1::CMMCPHits*>         ConstCmmCpHitsMap;
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
   struct CpmHitsData : public CpByteStreamToolData
   {
     CpmHitsData (CpmHitsCollection* const hitCollection)
       : CpByteStreamToolData (CPM_HITS), m_hitCollection (hitCollection) {}
     CpmHitsCollection* const m_hitCollection;
     CpmHitsMap   m_hitsMap;
   };
   struct CmmHitsData : public CpByteStreamToolData
   {
     CmmHitsData (CmmCpHitsCollection* const hitCollection)
       : CpByteStreamToolData (CMM_CP_HITS), m_cmmHitCollection (hitCollection) {}
     CmmCpHitsCollection* const m_cmmHitCollection;
     CmmCpHitsMap m_cmmHitsMap;
   };

   /// Create list of all source IDs.
   std::vector<uint32_t> makeSourceIDs() const;

   /// Convert bytestream to given container type
   StatusCode convertBs(const std::string& sgKey,
                        const IROBDataProviderSvc::VROBFRAG& robFrags,
                        CpByteStreamToolData& data) const;
   /// Unpack CMM-CP sub-block
   void decodeCmmCp(CmmCpSubBlock* subBlock, int trigCmm, CmmHitsData& data,
                    LocalData& ld) const;
   /// Unpack CPM sub-block
   void decodeCpm(CpmSubBlock* subBlock, int trigCpm, CpByteStreamToolData& data,
                  LocalData& ld) const;

   /// Find a CPM tower for given key
   const LVL1::CPMTower*  findCpmTower(unsigned int key,
                                       const ConstCpmTowerMap& ttMap) const;
   LVL1::CPMTower*  findCpmTower(const CpmTowerData& data, unsigned int key) const;
   /// Find CPM hits for given crate, module
   const LVL1::CPMHits*   findCpmHits(int crate, int module,
                                      const ConstCpmHitsMap& hitsMap) const;
   LVL1::CPMHits*   findCpmHits(const CpmHitsData& data, int crate, int module) const;
   /// Find CMM-CP hits for given crate, data ID
   const LVL1::CMMCPHits* findCmmCpHits(int crate, int dataID,
                                        const ConstCmmCpHitsMap& cmmHitsMap) const;
   LVL1::CMMCPHits* findCmmCpHits(const CmmHitsData& data,
                                  int crate, int dataID) const;

   /// Set up CPM tower map
   void setupCpmTowerMap(const CpmTowerCollection* ttCollection,
                         ConstCpmTowerMap& ttMap,
                         LVL1::TriggerTowerKey& towerKey) const;
   /// Set up CPM hits map
   void setupCpmHitsMap(const CpmHitsCollection* hitCollection,
                        ConstCpmHitsMap& hitsMap) const;
   /// Set up CMM-CP hits map
   void setupCmmCpHitsMap(const CmmCpHitsCollection* hitCollection,
                          ConstCmmCpHitsMap& cmmHitsMap) const;

   /// Get number of slices and triggered slice offset for next slink
   bool slinkSlices(int crate, int module, int modulesPerSlink,
                    int& timeslices, int& trigJem,
                    const ConstCpmTowerMap& ttMap,
                    const ConstCpmHitsMap& hitsMap,
                    const ConstCmmCpHitsMap& cmmHitsMap,
                    LVL1::TriggerTowerKey& towerKey) const;

   /// Property: Channel mapping tool
   ToolHandle<LVL1::IL1CaloMappingTool> m_cpmMaps;
   /// Error collection tool
   ToolHandle<LVL1BS::L1CaloErrorByteStreamTool> m_errorTool;

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
   /// Number of crates
   const int m_crates;
   /// Number of CPM modules per crate
   const int m_modules;
   /// Property: Number of slinks per crate when writing out bytestream
   int m_slinks;
   /// Property: Default number of slices in simulation
   int m_dfltSlices;
   /// Property: Force number of slices in bytestream
   int m_forceSlices;
   /// Property: ROB source IDs
   std::vector<uint32_t> m_sourceIDsProp;
   /// Sub-detector type
   const eformat::SubDetector m_subDetector;
   /// Source ID converter
   const L1CaloSrcIdMap m_srcIdMap;
};

} // end namespace

#endif
