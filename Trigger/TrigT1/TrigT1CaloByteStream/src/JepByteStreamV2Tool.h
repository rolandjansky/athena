/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_JEPBYTESTREAMV2TOOL_H
#define TRIGT1CALOBYTESTREAM_JEPBYTESTREAMV2TOOL_H

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

#include "CmxEnergySubBlock.h"

class IInterface;
class InterfaceID;
class StatusCode;

template <class T> class FullEventAssembler;

namespace LVL1 {
  class CMXJetHits;
  class CMXJetTob;
  class CMXEtSums;
  class IL1CaloMappingTool;
  class JEMEtSums;
  class JEPBSCollectionV2;
  class JetElement;
  class JetElementKey;
}

namespace LVL1BS {

class CmxJetSubBlock;
class JemSubBlockV2;
class L1CaloErrorByteStreamTool;
class L1CaloSrcIdMap;

/** Tool to perform ROB fragments to jet elements, jet hits and energy sums,
 *  and JEP container to raw data conversions.
 *
 *  Based on ROD document version X_xxx.                                     <<== CHECK
 *
 *  @author Peter Faulkner
 */

class JepByteStreamV2Tool : public AthAlgTool {

 public:
   JepByteStreamV2Tool(const std::string& type, const std::string& name,
                       const IInterface* parent);
   virtual ~JepByteStreamV2Tool();

   /// AlgTool InterfaceID
   static const InterfaceID& interfaceID();

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /// Convert ROB fragments to jet elements
   StatusCode convert(const std::string& name,
                      DataVector<LVL1::JetElement>* jeCollection);
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::JetElement>* jeCollection);
   /// Convert ROB fragments to energy sums
   StatusCode convert(const std::string& name,
                      DataVector<LVL1::JEMEtSums>* etCollection);
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::JEMEtSums>* etCollection);
   /// Convert ROB fragments to CMX TOBs
   StatusCode convert(const std::string& name,
                      DataVector<LVL1::CMXJetTob>* tobCollection);
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CMXJetTob>* tobCollection);
   /// Convert ROB fragments to CMX jet hits
   StatusCode convert(const std::string& name,
                      DataVector<LVL1::CMXJetHits>* hitCollection);
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CMXJetHits>* hitCollection);
   /// Convert ROB fragments to CMX energy sums
   StatusCode convert(const std::string& name,
                      DataVector<LVL1::CMXEtSums>* etCollection);
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CMXEtSums>* etCollection);

   /// Convert JEP Container to bytestream
   StatusCode convert(const LVL1::JEPBSCollectionV2* jep, RawEventWrite* re);

   /// Return reference to vector with all possible Source Identifiers
   const std::vector<uint32_t>& sourceIDs(const std::string& sgKey);

 private:
   enum CollectionType { JET_ELEMENTS, ENERGY_SUMS, CMX_TOBS,
                                       CMX_HITS, CMX_SUMS };

   typedef DataVector<LVL1::JetElement>                  JetElementCollection;
   typedef DataVector<LVL1::JEMEtSums>                   EnergySumsCollection;
   typedef DataVector<LVL1::CMXJetTob>                   CmxTobCollection;
   typedef DataVector<LVL1::CMXJetHits>                  CmxHitsCollection;
   typedef DataVector<LVL1::CMXEtSums>                   CmxSumsCollection;
   typedef std::map<unsigned int, LVL1::JetElement*>     JetElementMap;
   typedef std::map<unsigned int, const LVL1::JetElement*> ConstJetElementMap;
   typedef std::map<int, LVL1::JEMEtSums*>               EnergySumsMap;
   typedef std::map<int, const LVL1::JEMEtSums*>         ConstEnergySumsMap;
   typedef std::map<int, LVL1::CMXJetTob*>               CmxTobMap;
   typedef std::map<int, const LVL1::CMXJetTob*>         ConstCmxTobMap;
   typedef std::map<int, LVL1::CMXJetHits*>              CmxHitsMap;
   typedef std::map<int, const LVL1::CMXJetHits*>        ConstCmxHitsMap;
   typedef std::map<int, LVL1::CMXEtSums*>               CmxSumsMap;
   typedef std::map<int, const LVL1::CMXEtSums*>         ConstCmxSumsMap;

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
   struct EnergySumsData : public JepByteStreamToolData
   {
     EnergySumsData (EnergySumsCollection* const etCollection)
       : JepByteStreamToolData (ENERGY_SUMS), m_etCollection (etCollection) {}
     EnergySumsCollection* const m_etCollection;
     EnergySumsMap m_etMap;
   };
   struct CmxTobData : public JepByteStreamToolData
   {
     CmxTobData (CmxTobCollection* const tobCollection)
       : JepByteStreamToolData (CMX_TOBS), m_cmxTobCollection (tobCollection) {}
     CmxTobCollection* const m_cmxTobCollection;
     CmxTobMap     m_cmxTobMap;
   };
   struct CmxHitsData : public JepByteStreamToolData
   {
     CmxHitsData (CmxHitsCollection* const hitCollection)
       : JepByteStreamToolData (CMX_HITS), m_cmxHitCollection (hitCollection) {}
     CmxHitsCollection* const m_cmxHitCollection;
     CmxHitsMap    m_cmxHitsMap;
   };
   struct CmxSumsData : public JepByteStreamToolData
   {
     CmxSumsData (CmxSumsCollection* const etCollection)
       : JepByteStreamToolData (CMX_SUMS), m_cmxEtCollection (etCollection) {}
     CmxSumsCollection* const m_cmxEtCollection;
     CmxSumsMap    m_cmxEtMap;
   };

   /// Convert bytestream to given container type
   StatusCode convertBs(const IROBDataProviderSvc::VROBFRAG& robFrags,
                        JepByteStreamToolData& data);
   /// Unpack CMX-Energy sub-block
   void decodeCmxEnergy(CmxEnergySubBlock* subBlock, int trigJem, CmxSumsData& data);
   /// Unpack CMX-Jet sub-block
   void decodeCmxJet(CmxJetSubBlock* subBlock, int trigJem,
                     JepByteStreamToolData& data);
   /// Unpack JEM sub-block
   void decodeJem(JemSubBlockV2* subBlock, int trigJem,
                  JepByteStreamToolData& data);

   /// Find TOB map key for given crate, jem, frame, loc
   int tobKey(int crate, int jem, int frame, int loc);
   /// Find a jet element given eta, phi
   const LVL1::JetElement* findJetElement(double eta, double phi) const;
   LVL1::JetElement* findJetElement(const JetElementData& data, double eta, double phi) const;
   /// Find energy sums for given crate, module
   const LVL1::JEMEtSums*  findEnergySums(int crate, int module) const;
   LVL1::JEMEtSums*  findEnergySums(const EnergySumsData& data, int crate, int module) const;
   /// Find CMX TOB for given key
   const LVL1::CMXJetTob*  findCmxTob(int key) const;
   LVL1::CMXJetTob*  findCmxTob(const CmxTobData& data, int key) const;
   /// Find CMX hits for given crate, source
   const LVL1::CMXJetHits* findCmxHits(int crate, int source) const;
   LVL1::CMXJetHits* findCmxHits(const CmxHitsData& data, int crate, int source) const;
   /// Find CMX energy sums for given crate, source
   const LVL1::CMXEtSums*  findCmxSums(int crate, int source) const;
   LVL1::CMXEtSums*  findCmxSums(const CmxSumsData& data, int crate, int source) const;

   /// Set up jet element map
   void setupJeMap(const JetElementCollection* jeCollection);
   /// Set up energy sums map
   void setupEtMap(const EnergySumsCollection* enCollection);
   /// Set up CMX TOB map
   void setupCmxTobMap(const CmxTobCollection* tobCollection);
   /// Set up CMX hits map
   void setupCmxHitsMap(const CmxHitsCollection* hitCollection);
   /// Set up CMX energy sums map
   void setupCmxEtMap(const CmxSumsCollection* enCollection);

   /// Get number of slices and triggered slice offset for next slink
   bool slinkSlices(int crate, int module, int modulesPerSlink,
                    int& timeslices, int& trigJem);
   /// Get energy subBlock types from CMXEtSums source type
   void energySubBlockTypes(int source,
                            CmxEnergySubBlock::SourceType& srcType,
			    CmxEnergySubBlock::SumType&    sumType,
			    CmxEnergySubBlock::HitsType&   hitType);
   /// Get jet hits subBlock source ID from CMXJetHits source type
   int jetSubBlockSourceId(int source);

   ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
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
   /// Number of RoI frames
   int m_frames;
   /// Number of RoI locations
   int m_locations;
   /// Maximum number of TOBs per module
   int m_maxTobs;
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
   JemSubBlockV2* m_jemSubBlock;
   /// CmxEnergySubBlock for unpacking
   CmxEnergySubBlock* m_cmxEnergySubBlock;
   /// CmxJetSubBlock for unpacking
   CmxJetSubBlock* m_cmxJetSubBlock;
   /// Unsigned int unpacking vector 0
   std::vector<unsigned int> m_uintVec0;
   /// Unsigned int unpacking vector 1
   std::vector<unsigned int> m_uintVec1;
   /// Unsigned int unpacking vector 2
   std::vector<unsigned int> m_uintVec2;
   /// Int unpacking vector 0
   std::vector<int> m_intVec0;
   /// Int unpacking vector 1
   std::vector<int> m_intVec1;
   /// Int unpacking vector 2
   std::vector<int> m_intVec2;
   /// Vector for current JEM sub-blocks
   DataVector<JemSubBlockV2> m_jemBlocks;
   /// Vector for current CMX-Energy sub-blocks
   DataVector<CmxEnergySubBlock> m_cmxEnergyBlocks;
   /// Vector for current CMX-Jet sub-blocks
   DataVector<CmxJetSubBlock> m_cmxJetBlocks;
   /// Jet element map
   ConstJetElementMap m_jeMap;
   /// Energy sums map
   ConstEnergySumsMap m_etMap;
   /// CMX TOB map
   ConstCmxTobMap     m_cmxTobMap;
   /// CMX hits map
   ConstCmxHitsMap    m_cmxHitsMap;
   /// CMX energy sums map
   ConstCmxSumsMap    m_cmxEtMap;
   /// ROD Status words
   std::vector<uint32_t>* m_rodStatus;
   /// ROD status map
   std::map<uint32_t, std::vector<uint32_t>* > m_rodStatusMap;
   /// Event assembler
   FullEventAssembler<L1CaloSrcIdMap>* m_fea;

};

} // end namespace

#endif
