/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_JEPROIBYTESTREAMTOOL_H
#define TRIGT1CALOBYTESTREAM_JEPROIBYTESTREAMTOOL_H

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
  class CMMRoI;
  class JEMRoI;
  class JEPRoIBSCollection;
}

namespace LVL1BS {

class CmmEnergySubBlock;
class CmmJetSubBlock;
class JemRoiSubBlock;
class L1CaloErrorByteStreamTool;
class L1CaloSrcIdMap;

/** Tool to perform ROB fragments to JEM RoI and CMM RoI,
 *  and JEP RoI container to raw data conversions.
 *
 *  Based on ROD document version 1_09h.
 *
 *  @author Peter Faulkner
 */

class JepRoiByteStreamTool : public AthAlgTool {

 public:
   JepRoiByteStreamTool(const std::string& type, const std::string& name,
                        const IInterface* parent);
   virtual ~JepRoiByteStreamTool();

   /// AlgTool InterfaceID
   static const InterfaceID& interfaceID();

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /// Convert ROB fragments to JEM RoIs
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::JEMRoI>* jeCollection);
   /// Convert ROB fragments to CMM RoIs
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      LVL1::CMMRoI* cmCollection);

   /// Convert JEP RoI Container to bytestream
   StatusCode convert(const LVL1::JEPRoIBSCollection* jep, RawEventWrite* re);

   /// Return reference to vector with all possible Source Identifiers
   const std::vector<uint32_t>& sourceIDs(const std::string& sgKey);

 private:
   enum CollectionType { JEM_ROI, CMM_ROI };

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
                        CollectionType collection);

   /// Error collection tool
   ToolHandle<LVL1BS::L1CaloErrorByteStreamTool> m_errorTool;

   /// Find CMM hits for given crate, data ID
   const LVL1::CMMJetHits* findCmmHits(int crate, int dataID);
   /// Find CMM energy sums for given crate, data ID
   const LVL1::CMMEtSums*  findCmmSums(int crate, int dataID);

   /// Set up JEM RoIs map
   void setupJemRoiMap(const JemRoiCollection* jeCollection);
   /// Set up CMM hits map
   void setupCmmHitsMap(const CmmHitsCollection* hitCollection);
   /// Set up CMM energy sums map
   void setupCmmEtMap(const CmmSumsCollection* enCollection);

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
   /// Number of JEM modules per crate
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
   JemRoiSubBlock*  m_subBlock;
   /// Current JEM RoI collection
   JemRoiCollection* m_jeCollection;
   /// Current CMM RoI collection
   LVL1::CMMRoI*     m_cmCollection;
   /// JEM RoI map
   JemRoiMap  m_roiMap;
   /// CMM hits map
   CmmHitsMap m_cmmHitsMap;
   /// CMM energy sums map
   CmmSumsMap m_cmmEtMap;
   /// ROD Status words
   std::vector<uint32_t>* m_rodStatus;
   /// ROD status map
   std::map<uint32_t, std::vector<uint32_t>* > m_rodStatusMap;
   /// Event assembler
   FullEventAssembler<L1CaloSrcIdMap>* m_fea;

};

} // end namespace

#endif
