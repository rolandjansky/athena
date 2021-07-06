/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_JEPBYTESTREAMV2TOOL_H
#define TRIGT1CALOBYTESTREAM_JEPBYTESTREAMV2TOOL_H

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

   virtual StatusCode initialize() override;
   virtual StatusCode finalize() override;

   /// Convert ROB fragments to jet elements
   StatusCode convert(const std::string& name,
                      DataVector<LVL1::JetElement>* jeCollection) const;
   StatusCode convert(const std::string& sgKey,
                      const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::JetElement>* jeCollection) const;
   /// Convert ROB fragments to energy sums
   StatusCode convert(const std::string& name,
                      DataVector<LVL1::JEMEtSums>* etCollection) const;
   StatusCode convert(const std::string& sgKey,
                      const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::JEMEtSums>* etCollection) const;
   /// Convert ROB fragments to CMX TOBs
   StatusCode convert(const std::string& name,
                      DataVector<LVL1::CMXJetTob>* tobCollection) const;
   StatusCode convert(const std::string& sgKey,
                      const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CMXJetTob>* tobCollection) const;
   /// Convert ROB fragments to CMX jet hits
   StatusCode convert(const std::string& name,
                      DataVector<LVL1::CMXJetHits>* hitCollection) const;
   StatusCode convert(const std::string& sgKey,
                      const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CMXJetHits>* hitCollection) const;
   /// Convert ROB fragments to CMX energy sums
   StatusCode convert(const std::string& name,
                      DataVector<LVL1::CMXEtSums>* etCollection) const;
   StatusCode convert(const std::string& sgKey,
                      const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::CMXEtSums>* etCollection) const;

   /// Convert JEP Container to bytestream
   StatusCode convert(const LVL1::JEPBSCollectionV2* jep) const;

   /// Return reference to vector with all possible Source Identifiers
   const std::vector<uint32_t>& sourceIDs() const;

 private:
   // Need option to disable dependency on offline ByteStreamCnvSvc to be able to run
   // decoding in online HLT framework (which uses TrigByteStreamCnvSvc)
   Gaudi::Property<bool> m_enableEncoding{
     this, "enableEncoding", true, "Enable conversion from RDO to ByteStream"};
   SmartIF<ByteStreamCnvSvc> m_byteStreamCnvSvc;

  struct LocalData
  {
    /// Jet elements to accept (0=Core, 1=Overlap)
    int coreOverlap = 0;
    // Unpacking error code
    unsigned int rodErr = 0;
    /// Unsigned int unpacking vector 0
    std::vector<unsigned int> uintVec0;
    /// Unsigned int unpacking vector 1
    std::vector<unsigned int> uintVec1;
    /// Unsigned int unpacking vector 2
    std::vector<unsigned int> uintVec2;
    /// Int unpacking vector 0
    std::vector<int> intVec0;
    /// Int unpacking vector 1
    std::vector<int> intVec1;
    /// Int unpacking vector 2
    std::vector<int> intVec2;
    // Jet element key provider
    LVL1::JetElementKey elementKey;
  };

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
   StatusCode convertBs(const std::string& sgKey,
                        const IROBDataProviderSvc::VROBFRAG& robFrags,
                        JepByteStreamToolData& data) const;
   /// Unpack CMX-Energy sub-block
   void decodeCmxEnergy(CmxEnergySubBlock* subBlock, int trigJem, CmxSumsData& data,
                        LocalData& ld) const;
   /// Unpack CMX-Jet sub-block
   void decodeCmxJet(CmxJetSubBlock* subBlock, int trigJem,
                     JepByteStreamToolData& data,
                     LocalData& ld) const;
   /// Unpack JEM sub-block
   void decodeJem(JemSubBlockV2* subBlock, int trigJem,
                  JepByteStreamToolData& data,
                  LocalData& ld) const;

   /// Find TOB map key for given crate, jem, frame, loc
   int tobKey(int crate, int jem, int frame, int loc) const;
   /// Find a jet element given eta, phi
   const LVL1::JetElement* findJetElement(double eta, double phi,
                                          const ConstJetElementMap& jeMap,
                                          LVL1::JetElementKey& elementKey) const;
   LVL1::JetElement* findJetElement(const JetElementData& data, double eta, double phi,
                                    LVL1::JetElementKey& elementKey) const;
   /// Find energy sums for given crate, module
   const LVL1::JEMEtSums*  findEnergySums(int crate, int module,
                                          const ConstEnergySumsMap& etMap) const;
   LVL1::JEMEtSums*  findEnergySums(const EnergySumsData& data, int crate, int module) const;
   /// Find CMX TOB for given key
   const LVL1::CMXJetTob*  findCmxTob(int key,
                                      const ConstCmxTobMap& cmxTobMap) const;
   LVL1::CMXJetTob*  findCmxTob(const CmxTobData& data, int key) const;
   /// Find CMX hits for given crate, source
   const LVL1::CMXJetHits* findCmxHits(int crate, int source,
                                       const ConstCmxHitsMap& cmxHitsMap) const;
   LVL1::CMXJetHits* findCmxHits(const CmxHitsData& data, int crate, int source) const;
   /// Find CMX energy sums for given crate, source
   const LVL1::CMXEtSums*  findCmxSums(int crate, int source,
                                       const ConstCmxSumsMap& cmxEtMap) const;
   LVL1::CMXEtSums*  findCmxSums(const CmxSumsData& data, int crate, int source) const;

   std::vector<uint32_t> makeSourceIDs() const;

   /// Set up jet element map
   void setupJeMap(const JetElementCollection* jeCollection,
                   ConstJetElementMap& jeMap,
                   LVL1::JetElementKey& elementKey) const;
   /// Set up energy sums map
   void setupEtMap(const EnergySumsCollection* enCollection,
                   ConstEnergySumsMap& etMap) const;
   /// Set up CMX TOB map
   void setupCmxTobMap(const CmxTobCollection* tobCollection,
                       ConstCmxTobMap& cmxTobMap) const;
   /// Set up CMX hits map
   void setupCmxHitsMap(const CmxHitsCollection* hitCollection,
                        ConstCmxHitsMap& cmxHitsMap) const;
   /// Set up CMX energy sums map
   void setupCmxEtMap(const CmxSumsCollection* enCollection,
                      ConstCmxSumsMap& cmxEtMap) const;

   /// Get number of slices and triggered slice offset for next slink
   bool slinkSlices(int crate, int module, int modulesPerSlink,
                    int& timeslices, int& trigJem,
                    const ConstJetElementMap& jeMap,
                    const ConstEnergySumsMap& etMap,
                    const ConstCmxTobMap& cmxTobMap,
                    const ConstCmxHitsMap& cmxHitsMap,
                    const ConstCmxSumsMap& cmxEtMap,
                    LVL1::JetElementKey& elementKey) const;
   /// Get energy subBlock types from CMXEtSums source type
   void energySubBlockTypes(int source,
                            CmxEnergySubBlock::SourceType& srcType,
			    CmxEnergySubBlock::SumType&    sumType,
			    CmxEnergySubBlock::HitsType&   hitType) const;
   /// Get jet hits subBlock source ID from CMXJetHits source type
   int jetSubBlockSourceId(int source) const;

   ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
   /// Property: Channel mapping tool
   ToolHandle<LVL1::IL1CaloMappingTool> m_jemMaps;
   /// Property:Error collection tool
   ToolHandle<LVL1BS::L1CaloErrorByteStreamTool> m_errorTool;

   /// Property:Hardware crate number offset
   int m_crateOffsetHw;
   /// Property:Software crate number offset
   int m_crateOffsetSw;
   /// Property:Sub_block header version
   int m_version;
   /// Property:Data compression format
   int m_dataFormat;
   /// Number of channels per module
   const int m_channels;
   /// Number of crates
   const int m_crates;
   /// Number of JEM modules per crate
   const int m_modules;
   /// Number of RoI frames
   const int m_frames;
   /// Number of RoI locations
   const int m_locations;
   /// Maximum number of TOBs per module
   const int m_maxTobs;
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
   /// Property:ROB source IDs
   std::vector<uint32_t> m_sourceIDsProp;
   /// Sub-detector type
   const eformat::SubDetector m_subDetector;
   /// Source ID converter
   const L1CaloSrcIdMap m_srcIdMap;
};

} // end namespace

#endif
