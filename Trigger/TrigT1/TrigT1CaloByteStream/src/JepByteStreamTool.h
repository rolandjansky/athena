/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_JEPBYTESTREAMTOOL_H
#define TRIGT1CALOBYTESTREAM_JEPBYTESTREAMTOOL_H

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

template <class T> class FullEventAssembler;

namespace LVL1 {
  class CMMJetHits;
  class CMMEtSums;
  class IL1CaloMappingTool;
  class JEMHits;
  class JEMEtSums;
  class JEPBSCollection;
  class JetElement;
  class JetElementKey;
}

namespace LVL1BS {

class CmmEnergySubBlock;
class CmmJetSubBlock;
class JemSubBlock;
class L1CaloErrorByteStreamTool;
class L1CaloSrcIdMap;

/** Tool to perform ROB fragments to jet elements, jet hits and energy sums,
 *  and JEP container to raw data conversions.
 *
 *  Based on ROD document version 1_09h.
 *
 *  @author Peter Faulkner
 */

class JepByteStreamTool : public AthAlgTool {

 public:
   JepByteStreamTool(const std::string& type, const std::string& name,
                     const IInterface* parent);
   virtual ~JepByteStreamTool();

   /// AlgTool InterfaceID
   static const InterfaceID& interfaceID();

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /// Convert ROB fragments to jet elements
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::JetElement>* jeCollection);
   /// Convert ROB fragments to jet hits
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::JEMHits>* hitCollection);
   /// Convert ROB fragments to energy sums
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::JEMEtSums>* etCollection);
   /// Convert ROB fragments to CMM jet hits
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CMMJetHits>* hitCollection);
   /// Convert ROB fragments to CMM energy sums
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CMMEtSums>* etCollection);

   /// Convert JEP Container to bytestream
   StatusCode convert(const LVL1::JEPBSCollection* jep, RawEventWrite* re);

   /// Return reference to vector with all possible Source Identifiers
   const std::vector<uint32_t>& sourceIDs(const std::string& sgKey);

 private:
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
   StatusCode convertBs(const IROBDataProviderSvc::VROBFRAG& robFrags,
                        JepByteStreamToolData& data);
   /// Unpack CMM-Energy sub-block
   void decodeCmmEnergy(CmmEnergySubBlock* subBlock, int trigCmm, CmmSumsData& data);
   /// Unpack CMM-Jet sub-block
   void decodeCmmJet(CmmJetSubBlock* subBlock, int trigCmm, CmmHitsData& data);
   /// Unpack JEM sub-block
   void decodeJem(JemSubBlock* subBlock, int trigJem,
                  JepByteStreamToolData& data);

   /// Find a jet element given eta, phi
   const LVL1::JetElement* findJetElement(double eta, double phi) const;
   LVL1::JetElement* findJetElement(const JetElementData& data, double eta, double phi) const;
   /// Find jet hits for given crate, module
   const LVL1::JEMHits*    findJetHits(int crate, int module) const;
   LVL1::JEMHits*    findJetHits(const JetHitsData& data, int crate, int module) const;
   /// Find energy sums for given crate, module
   const LVL1::JEMEtSums*  findEnergySums(int crate, int module) const;
   LVL1::JEMEtSums*  findEnergySums(const EnergySumsData& data, int crate, int module) const;
   /// Find CMM hits for given crate, data ID
   const LVL1::CMMJetHits* findCmmHits(int crate, int dataID) const;
   LVL1::CMMJetHits* findCmmHits(const CmmHitsData& data, int crate, int dataID) const;
   /// Find CMM energy sums for given crate, data ID
   const LVL1::CMMEtSums*  findCmmSums(int crate, int dataID) const;
   LVL1::CMMEtSums*  findCmmSums(const CmmSumsData& data, int crate, int dataID) const;

   /// Set up jet element map
   void setupJeMap(const JetElementCollection* jeCollection);
   /// Set up jet hits map
   void setupHitsMap(const JetHitsCollection* hitCollection);
   /// Set up energy sums map
   void setupEtMap(const EnergySumsCollection* enCollection);
   /// Set up CMM hits map
   void setupCmmHitsMap(const CmmHitsCollection* hitCollection);
   /// Set up CMM energy sums map
   void setupCmmEtMap(const CmmSumsCollection* enCollection);

   /// Get number of slices and triggered slice offset for next slink
   bool slinkSlices(int crate, int module, int modulesPerSlink,
                    int& timeslices, int& trigJem);

   /// Channel mapping tool
   ToolHandle<LVL1::IL1CaloMappingTool> m_jemMaps;
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
   /// Number of channels per module
   int m_channels;
   /// Number of crates
   int m_crates;
   /// Number of JEM modules per crate
   int m_modules;
   /// Number of slinks per crate when writing out bytestream
   int m_slinks;
   /// Default number of slices in simulation
   int m_dfltSlices;
   /// Force number of slices in bytestream
   int m_forceSlices;
   /// Jet elements to accept (0=Core, 1=Overlap)
   int m_coreOverlap;
   /// Unpacking error code
   unsigned int m_rodErr;
   /// ROB source IDs
   std::vector<uint32_t> m_sourceIDs;
   /// Sub-detector type
   eformat::SubDetector m_subDetector;
   /// Source ID converter
   L1CaloSrcIdMap* m_srcIdMap;
   /// Jet element key provider
   LVL1::JetElementKey* m_elementKey;
   /// JemSubBlock for unpacking
   JemSubBlock* m_jemSubBlock;
   /// CmmEnergySubBlock for unpacking
   CmmEnergySubBlock* m_cmmEnergySubBlock;
   /// CmmJetSubBlock for unpacking
   CmmJetSubBlock* m_cmmJetSubBlock;
   /// Ex vector for unpacking
   std::vector<unsigned int> m_exVec;
   /// Ey vector for unpacking
   std::vector<unsigned int> m_eyVec;
   /// Et vector for unpacking
   std::vector<unsigned int> m_etVec;
   /// Ex error vector for unpacking
   std::vector<int> m_exErrVec;
   /// Ex error vector for unpacking
   std::vector<int> m_eyErrVec;
   /// Ex error vector for unpacking
   std::vector<int> m_etErrVec;
   /// Hits vector for unpacking
   std::vector<unsigned int> m_hitsVec;
   /// Error vector for unpacking
   std::vector<int> m_errVec;
   /// Vector for current JEM sub-blocks
   DataVector<JemSubBlock> m_jemBlocks;
   /// Vector for current CMM-Energy sub-blocks
   DataVector<CmmEnergySubBlock> m_cmmEnergyBlocks;
   /// Vector for current CMM-Jet sub-blocks
   DataVector<CmmJetSubBlock> m_cmmJetBlocks;
   /// Jet element map
   ConstJetElementMap m_jeMap;
   /// Jet hits map
   ConstJetHitsMap    m_hitsMap;
   /// Energy sums map
   ConstEnergySumsMap m_etMap;
   /// CMM hits map
   ConstCmmHitsMap    m_cmmHitsMap;
   /// CMM energy sums map
   ConstCmmSumsMap    m_cmmEtMap;
   /// ROD Status words
   std::vector<uint32_t>* m_rodStatus;
   /// ROD status map
   std::map<uint32_t, std::vector<uint32_t>* > m_rodStatusMap;
   /// Event assembler
   FullEventAssembler<L1CaloSrcIdMap>* m_fea;

};

} // end namespace

#endif
