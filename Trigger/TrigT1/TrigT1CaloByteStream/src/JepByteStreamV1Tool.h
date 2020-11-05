/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_JEPBYTESTREAMV1TOOL_H
#define TRIGT1CALOBYTESTREAM_JEPBYTESTREAMV1TOOL_H

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include "L1CaloSrcIdMap.h"
#include "TrigT1CaloUtils/JetElementKey.h"
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
  class IL1CaloMappingTool;
  class JEMHits;
  class JEMEtSums;
  class JEPBSCollectionV1;
  class JetElement;
  class JetElementKey;
}

namespace LVL1BS {

class CmmEnergySubBlock;
class CmmJetSubBlock;
class JemSubBlockV1;
class L1CaloErrorByteStreamTool;
class L1CaloSrcIdMap;

/** Tool to perform ROB fragments to jet elements, jet hits and energy sums,
 *  and JEP container to raw data conversions.
 *
 *  Based on ROD document version 1_09h.
 *
 *  @author Peter Faulkner
 */

class JepByteStreamV1Tool : public AthAlgTool {

 public:
   JepByteStreamV1Tool(const std::string& type, const std::string& name,
                       const IInterface* parent);
   virtual ~JepByteStreamV1Tool();

   /// AlgTool InterfaceID
   static const InterfaceID& interfaceID();

   virtual StatusCode initialize() override;
   virtual StatusCode finalize() override;

   /// Convert ROB fragments to jet elements
   StatusCode convert(const std::string& sgKey,
                      const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::JetElement>* jeCollection) const;
   /// Convert ROB fragments to jet hits
   StatusCode convert(const std::string& sgKey,
                      const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::JEMHits>* hitCollection) const;
   /// Convert ROB fragments to energy sums
   StatusCode convert(const std::string& sgKey,
                      const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::JEMEtSums>* etCollection) const;
   /// Convert ROB fragments to CMM jet hits
   StatusCode convert(const std::string& sgKey,
                      const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CMMJetHits>* hitCollection) const;
   /// Convert ROB fragments to CMM energy sums
   StatusCode convert(const std::string& sgKey,
                      const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CMMEtSums>* etCollection) const;

   /// Convert JEP Container to bytestream
   StatusCode convert(const LVL1::JEPBSCollectionV1* jep) const;

   /// Return reference to vector with all possible Source Identifiers
   const std::vector<uint32_t>& sourceIDs() const;

 private:
   ServiceHandle<ByteStreamCnvSvc> m_byteStreamCnvSvc
   { this, "ByteStreamCnvSvc", "ByteStreamCnvSvc" };

  struct LocalData
   {
     /// Jet elements to accept (0=Core, 1=Overlap)
     int coreOverlap = 0;
     /// Unpacking error code
     unsigned int rodErr = 0;
     /// Ex vector for unpacking
     std::vector<unsigned int> exVec;
     /// Ey vector for unpacking
     std::vector<unsigned int> eyVec;
     /// Et vector for unpacking
     std::vector<unsigned int> etVec;
     /// Ex error vector for unpacking
     std::vector<int> exErrVec;
     /// Ex error vector for unpacking
     std::vector<int> eyErrVec;
     /// Ex error vector for unpacking
     std::vector<int> etErrVec;
     /// Hits vector for unpacking
     std::vector<unsigned int> hitsVec;
     /// Error vector for unpacking
     std::vector<int> errVec;
     /// Jet element key provider
     LVL1::JetElementKey elementKey;
   };

   enum CollectionType { JET_ELEMENTS, JET_HITS, ENERGY_SUMS,
                                       CMM_HITS, CMM_SUMS };

   typedef DataVector<LVL1::JetElement>                  JetElementCollection;
   typedef DataVector<LVL1::JEMHits>                     JetHitsCollection;
   typedef DataVector<LVL1::JEMEtSums>                   EnergySumsCollection;
   typedef DataVector<LVL1::CMMJetHits>                  CmmHitsCollection;
   typedef DataVector<LVL1::CMMEtSums>                   CmmSumsCollection;
   typedef std::map<unsigned int, LVL1::JetElement*>     JetElementMap;
   typedef std::map<unsigned int, const LVL1::JetElement*> ConstJetElementMap;
   typedef std::map<int, LVL1::JEMHits*>                 JetHitsMap;
   typedef std::map<int, const LVL1::JEMHits*>           ConstJetHitsMap;
   typedef std::map<int, LVL1::JEMEtSums*>               EnergySumsMap;
   typedef std::map<int, const LVL1::JEMEtSums*>         ConstEnergySumsMap;
   typedef std::map<int, LVL1::CMMJetHits*>              CmmHitsMap;
   typedef std::map<int, const LVL1::CMMJetHits*>        ConstCmmHitsMap;
   typedef std::map<int, LVL1::CMMEtSums*>               CmmSumsMap;
   typedef std::map<int, const LVL1::CMMEtSums*>         ConstCmmSumsMap;
   typedef IROBDataProviderSvc::VROBFRAG::const_iterator ROBIterator;
   typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      ROBPointer;
   typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      RODPointer;

   struct JepByteStreamToolData
   {
     JepByteStreamToolData (const CollectionType collection)
       : m_collection(collection){}
     const CollectionType m_collection;
   };
   struct JetElementData : public JepByteStreamToolData
   {
     JetElementData (JetElementCollection* const jeCollection)
       : JepByteStreamToolData (JET_ELEMENTS), m_jeCollection (jeCollection) {}
     JetElementCollection* const m_jeCollection;
     JetElementMap m_jeMap;
   };
   struct JetHitsData : public JepByteStreamToolData
   {
     JetHitsData (JetHitsCollection* const hitCollection)
       : JepByteStreamToolData (JET_HITS), m_hitCollection (hitCollection) {}
     JetHitsCollection* const m_hitCollection;
     JetHitsMap    m_hitsMap;
   };
   struct EnergySumsData : public JepByteStreamToolData
   {
     EnergySumsData (EnergySumsCollection* const etCollection)
       : JepByteStreamToolData (ENERGY_SUMS), m_etCollection (etCollection) {}
     EnergySumsCollection* const m_etCollection;
     EnergySumsMap m_etMap;
   };
   struct CmmHitsData : public JepByteStreamToolData
   {
     CmmHitsData (CmmHitsCollection* const cmmHitCollection)
       : JepByteStreamToolData (CMM_HITS), m_cmmHitCollection (cmmHitCollection) {}
     CmmHitsCollection* const m_cmmHitCollection;
     CmmHitsMap    m_cmmHitsMap;
   };
   struct CmmSumsData : public JepByteStreamToolData
   {
     CmmSumsData (CmmSumsCollection* const cmmEtCollection)
       : JepByteStreamToolData (CMM_SUMS), m_cmmEtCollection (cmmEtCollection) {}
     CmmSumsCollection* const m_cmmEtCollection;
     CmmSumsMap    m_cmmEtMap;
   };

   /// Convert bytestream to given container type
   StatusCode convertBs(const std::string& sgKey,
                        const IROBDataProviderSvc::VROBFRAG& robFrags,
                        JepByteStreamToolData& data) const;
   /// Unpack CMM-Energy sub-block
   void decodeCmmEnergy(CmmEnergySubBlock* subBlock, int trigCmm, CmmSumsData& data,
                        LocalData& ld) const;
   /// Unpack CMM-Jet sub-block
   void decodeCmmJet(CmmJetSubBlock* subBlock, int trigCmm, CmmHitsData& data,
                     LocalData& ld) const;
   /// Unpack JEM sub-block
   void decodeJem(JemSubBlockV1* subBlock, int trigJem,
                  JepByteStreamToolData& data,
                  LocalData& ld) const;

   /// Find a jet element given eta, phi
   const LVL1::JetElement* findJetElement(double eta, double phi,
                                          const ConstJetElementMap& jeMap,
                                          LVL1::JetElementKey& elementKey) const;
   LVL1::JetElement* findJetElement(const JetElementData& data, double eta, double phi,
                                    LVL1::JetElementKey& elementKey) const;
   /// Find jet hits for given crate, module
   const LVL1::JEMHits*    findJetHits(int crate, int module,
                                       const ConstJetHitsMap& hitsMap) const;
   LVL1::JEMHits*    findJetHits(const JetHitsData& data, int crate, int module) const;
   /// Find energy sums for given crate, module
   const LVL1::JEMEtSums*  findEnergySums(int crate, int module,
                                          const ConstEnergySumsMap& etMap) const;
   LVL1::JEMEtSums*  findEnergySums(const EnergySumsData& data, int crate, int module) const;
   /// Find CMM hits for given crate, data ID
   const LVL1::CMMJetHits* findCmmHits(int crate, int dataID,
                                       const ConstCmmHitsMap& cmmHitsMap) const;
   LVL1::CMMJetHits* findCmmHits(const CmmHitsData& data, int crate, int dataID) const;
   /// Find CMM energy sums for given crate, data ID
   const LVL1::CMMEtSums*  findCmmSums(int crate, int dataID,
                                       const ConstCmmSumsMap& cmmEtMap) const;
   LVL1::CMMEtSums*  findCmmSums(const CmmSumsData& data, int crate, int dataID) const;

   std::vector<uint32_t> makeSourceIDs() const;

   /// Set up jet element map
   void setupJeMap(const JetElementCollection* jeCollection,
                   ConstJetElementMap& jeMap,
                   LVL1::JetElementKey& elementKey) const;
   /// Set up jet hits map
   void setupHitsMap(const JetHitsCollection* hitCollection,
                     ConstJetHitsMap& hitsMap) const;
   /// Set up energy sums map
   void setupEtMap(const EnergySumsCollection* enCollection,
                   ConstEnergySumsMap& etMap) const;
   /// Set up CMM hits map
   void setupCmmHitsMap(const CmmHitsCollection* hitCollection,
                        ConstCmmHitsMap& cmmHitsMap) const;
   /// Set up CMM energy sums map
   void setupCmmEtMap(const CmmSumsCollection* enCollection,
                      ConstCmmSumsMap& cmmEtMap) const;

   /// Get number of slices and triggered slice offset for next slink
   bool slinkSlices(int crate, int module, int modulesPerSlink,
                    int& timeslices, int& trigJem,
                    const ConstJetElementMap& jeMap,
                    const ConstJetHitsMap& hitsMap,
                    const ConstEnergySumsMap& etMap,
                    const ConstCmmHitsMap& cmmHitsMap,
                    const ConstCmmSumsMap& cmmEtMap,
                    LVL1::JetElementKey& elementKey) const;

   /// Property: Channel mapping tool
   ToolHandle<LVL1::IL1CaloMappingTool> m_jemMaps;
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
   /// Number of channels per module
   const int m_channels;
   /// Number of crates
   const int m_crates;
   /// Number of JEM modules per crate
   const int m_modules;
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
   /// Property: ROB source IDs
   std::vector<uint32_t> m_sourceIDsProp;
   /// Sub-detector type
   const eformat::SubDetector m_subDetector;
   /// Source ID converter
   const L1CaloSrcIdMap m_srcIdMap;
};

} // end namespace

#endif
