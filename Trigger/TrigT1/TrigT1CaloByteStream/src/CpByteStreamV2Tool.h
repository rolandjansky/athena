/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CPBYTESTREAMV2TOOL_H
#define TRIGT1CALOBYTESTREAM_CPBYTESTREAMV2TOOL_H

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

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

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /// Convert ROB fragments to CPM towers
   StatusCode convert(const std::string& sgKey, DataVector<LVL1::CPMTower>* ttCollection);
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CPMTower>* ttCollection);
   /// Convert ROB fragments to CMX-CP TOBs
   StatusCode convert(const std::string& sgKey,
                      DataVector<LVL1::CMXCPTob>* tobCollection);
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CMXCPTob>* tobCollection);
   /// Convert ROB fragments to CMX-CP hits
   StatusCode convert(const std::string& sgKey,
                      DataVector<LVL1::CMXCPHits>* hitCollection);
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CMXCPHits>* hitCollection);

   /// Convert CP Container to bytestream
   StatusCode convert(const LVL1::CPBSCollectionV2* cp, RawEventWrite* re);

   /// Return reference to vector with all possible Source Identifiers
   const std::vector<uint32_t>& sourceIDs(const std::string& sgKey);

 private:

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

   /// Convert bytestream to given container type
   StatusCode convertBs(const IROBDataProviderSvc::VROBFRAG& robFrags,
                        CpByteStreamToolData& data);
   /// Unpack CMX-CP sub-block
   void decodeCmxCp(CmxCpSubBlock* subBlock, int trigCpm, CpByteStreamToolData& data);
   /// Unpack CPM sub-block
   void decodeCpm(CpmSubBlockV2* subBlock, int trigCpm, CpmTowerData& data);

   /// Find a CPM tower for given key
   const LVL1::CPMTower*  findCpmTower(unsigned int key) const;
   LVL1::CPMTower*  findCpmTower(const CpmTowerData& data, unsigned int key) const;
   /// Find CMX-CP TOB for given key
   const LVL1::CMXCPTob*  findCmxCpTob(int key) const;
   LVL1::CMXCPTob*  findCmxCpTob(const CmxCpTobData& data, int key) const;
   /// Find CMX-CP hits for given key
   const LVL1::CMXCPHits* findCmxCpHits(int key) const;
   LVL1::CMXCPHits* findCmxCpHits(const CmxCpHitsData& data, int key) const;

   /// Set up CPM tower map
   void setupCpmTowerMap(const CpmTowerCollection* ttCollection);
   /// Set up CMX-CP TOB map
   void setupCmxCpTobMap(const CmxCpTobCollection* tobCollection);
   /// Set up CMX-CP hits map
   void setupCmxCpHitsMap(const CmxCpHitsCollection* hitCollection);

   /// Key for TOBs
   int tobKey(int crate, int cmx, int cpm, int chip, int loc) const;
   /// Key for Hits
   int hitsKey(int crate, int cmx, int source) const;

   /// Get number of slices and triggered slice offset for next slink
   bool slinkSlices(int crate, int module, int modulesPerSlink,
                    int& timeslices, int& trigJem);

   /// Channel mapping tool
   ToolHandle<LVL1::IL1CaloMappingTool> m_cpmMaps;
   /// Error collection tool
   ToolHandle<LVL1BS::L1CaloErrorByteStreamTool> m_errorTool;
   ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
   /// Hardware crate number offset
   int m_crateOffsetHw;
   /// Software crate number offset
   int m_crateOffsetSw;
   /// Sub_block header version
   int m_version;
   /// Data compression format
   int m_dataFormat;
   /// Number of channels per module
   int m_channels;
   /// Number of crates
   int m_crates;
   /// Number of CPM modules per crate
   int m_modules;
   /// Number of CMXs per crate
   int m_cmxs;
   /// Maximum number of TOBS per module
   int m_maxTobs;
   /// Number of chips
   int m_chips;
   /// Number of Local coordinates
   int m_locs;
   /// Number of slinks per crate when writing out bytestream
   int m_slinks;
   /// Default number of slices in simulation
   int m_dfltSlices;
   /// Force number of slices in bytestream
   int m_forceSlices;
   /// Minimum crate number when writing out bytestream
   int m_crateMin;
   /// Maximum crate number when writing out bytestream
   int m_crateMax;
   /// Tower channels to accept (1=Core, 2=Overlap)
   int m_coreOverlap;
   /// Unpacking error code
   unsigned int m_rodErr;
   /// ROB source IDs
   std::vector<uint32_t> m_sourceIDs;
   /// Sub-detector type
   eformat::SubDetector m_subDetector;
   /// Source ID converter
   L1CaloSrcIdMap* m_srcIdMap;
   /// Trigger tower key provider
   LVL1::TriggerTowerKey* m_towerKey;
   /// CPM sub-block for unpacking
   CpmSubBlockV2* m_cpmSubBlock;
   /// CMX-CP sub-block for unpacking
   CmxCpSubBlock* m_cmxCpSubBlock;
   /// Energy vector for unpacking
   std::vector<int> m_energyVec;
   /// Isolation vector for unpacking
   std::vector<int> m_isolVec;
   /// TOB error vector for unpacking
   std::vector<int> m_errorVec;
   /// Presence map vector for unpacking
   std::vector<unsigned int> m_presenceMapVec;
   /// Hits0 vector for unpacking
   std::vector<unsigned int> m_hitsVec0;
   /// Hits1 vector for unpacking
   std::vector<unsigned int> m_hitsVec1;
   /// Error0 vector for unpacking
   std::vector<int> m_errVec0;
   /// Error1 vector for unpacking
   std::vector<int> m_errVec1;
   /// EM data vector for unpacking
   std::vector<int> m_emVec;
   /// Had data vector for unpacking
   std::vector<int> m_hadVec;
   /// EM error data vector for unpacking
   std::vector<int> m_emErrVec;
   /// Had error data vector for unpacking
   std::vector<int> m_hadErrVec;
   /// Vector for current CPM sub-blocks
   DataVector<CpmSubBlockV2> m_cpmBlocks;
   /// Vector for current CMX-CP sub-blocks
   DataVector<CmxCpSubBlock> m_cmxBlocks;
   /// CPM tower map
   ConstCpmTowerMap  m_ttMap;
   /// CMX-CP TOB map
   ConstCmxCpTobMap  m_tobMap;
   /// CMX-CP hits map
   ConstCmxCpHitsMap m_hitsMap;
   /// ROD Status words
   std::vector<uint32_t>* m_rodStatus;
   /// ROD status map
   std::map<uint32_t, std::vector<uint32_t>* > m_rodStatusMap;
   /// Event assembler
   FullEventAssembler<L1CaloSrcIdMap>* m_fea;
};

} // end namespace

#endif
