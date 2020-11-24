/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_PPMBYTESTREAMV1TOOL_H
#define TRIGT1CALOBYTESTREAM_PPMBYTESTREAMV1TOOL_H

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <atomic>
#include <mutex>

#include "L1CaloSrcIdMap.h"
#include "PpmSubBlockV1.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"
#include "AthContainers/DataVector.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "eformat/SourceIdentifier.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "CxxUtils/checker_macros.h"

class IInterface;
class InterfaceID;
class StatusCode;
class SegMemSvc;

template <class T> class FullEventAssembler;

namespace LVL1 {
  class IL1CaloMappingTool;
  class TriggerTower;
  class TriggerTowerKey;
}

namespace LVL1BS {

class L1CaloErrorByteStreamTool;
class L1CaloSrcIdMap;
class PpmSubBlockV1;

/** Tool to perform ROB fragments to trigger towers and trigger towers
 *  to raw data conversions.
 *
 *  Based on ROD document version 1_09h.
 *
 *  @author Peter Faulkner
 */

class PpmByteStreamV1Tool : public AthAlgTool {

 public:
   PpmByteStreamV1Tool(const std::string& type, const std::string& name,
                     const IInterface* parent);
   virtual ~PpmByteStreamV1Tool();

   /// AlgTool InterfaceID
   static const InterfaceID& interfaceID();

   virtual StatusCode initialize() override;
   virtual StatusCode finalize() override;

   /// Convert ROB fragments to trigger towers
   StatusCode convert(const std::string& sgKey,
                      const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::TriggerTower>* ttCollection) const;

   /// Convert trigger towers to bytestream
   StatusCode convert(const DataVector<LVL1::TriggerTower>* ttCollection,
                      RawEventWrite* re) const;

   /// Return reference to vector with all possible Source Identifiers
   const std::vector<uint32_t>& sourceIDs(const std::string& sgKey) const;

 private:

   typedef DataVector<LVL1::TriggerTower>                TriggerTowerCollection;
   typedef std::vector<LVL1::TriggerTower*>              TriggerTowerVector;
   typedef std::map<unsigned int, int>                   TriggerTowerMap;
   typedef std::map<unsigned int, const LVL1::TriggerTower*>
                                                         TriggerTowerMapConst;
   typedef std::vector<uint32_t>                         ChannelBitVector;
   typedef IROBDataProviderSvc::VROBFRAG::const_iterator ROBIterator;
   typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      ROBPointer;
   typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      RODPointer;

   /// Add compression stats to totals
   void addCompStats(const std::vector<uint32_t>& stats) const;
   /// Print compression stats
   void printCompStats() const;

   /// Find a trigger tower using separate layer maps
   const LVL1::TriggerTower* findLayerTriggerTower(double eta, double phi,
                                                   int layer,
                                                   const TriggerTowerMapConst& ttEmMap,
                                                   const TriggerTowerMapConst& ttHadMap,
                                                   LVL1::TriggerTowerKey& towerKey) const;
   /// Set up separate Em and Had trigger tower maps
   void setupTTMaps(const TriggerTowerCollection* ttCollection,
                    TriggerTowerMapConst& ttEmMap,
                    TriggerTowerMapConst& ttHadMap,
                    LVL1::TriggerTowerKey& towerKey) const;

   /// Get number of slices and triggered slice offsets for next slink
   bool slinkSlices(int crate, int module, int modulesPerSlink,
        int& slicesLut, int& slicesFadc, int& trigLut, int& trigFadc,
        const TriggerTowerMapConst& ttEmMap,
        const TriggerTowerMapConst& ttHadMap,
        LVL1::TriggerTowerKey& towerKey) const;

   std::vector<uint32_t>
   makeSourceIDs (bool spareChannels, bool muonChannels) const;

   /// Print a vector
   void printVec(const std::vector<int>& vec) const;

   struct SlotData
   {
     std::mutex m_mutex;
     /// TriggerTower pool vectors
     TriggerTowerVector m_ttData;
     TriggerTowerVector m_ttSpare;
     TriggerTowerVector m_ttMuon;
     std::vector<int> m_ttPos;
     /// Mapping vectors
     ChannelBitVector m_chanLayer;
     ChannelBitVector m_dataChan;
     ChannelBitVector m_spareChan;
     ChannelBitVector m_muonChan;
     ChannelBitVector m_dataMod;
     ChannelBitVector m_spareMod;
     ChannelBitVector m_muonMod;
     ChannelBitVector m_foundChan;
     /// Vector for current PPM sub-blocks
     DataVector<PpmSubBlockV1> m_ppmBlocks;
     /// Current error block
     PpmSubBlockV1 m_errorBlock;
     /// Trigger tower key provider
     LVL1::TriggerTowerKey m_towerKey;
   };
   mutable SG::SlotSpecificObj<SlotData> m_slotData ATLAS_THREAD_SAFE;


   /// Property: Channel mapping tool
   ToolHandle<LVL1::IL1CaloMappingTool> m_ppmMaps;
   /// Property: Error collection tool
   ToolHandle<LVL1BS::L1CaloErrorByteStreamTool> m_errorTool;
   /// Memory pool service
   ServiceHandle<SegMemSvc> m_sms;

   /// Sub_block header version
   const int m_version;
   /// Property: Data compression format
   int m_dataFormat;
   /// Compression version
   const int m_compVers;
   /// Property: Compression statistics print flag
   int m_printCompStats;
   /// Property: Number of slinks per crate when writing out bytestream
   int m_slinks;
   /// Property: Default number of LUT slices in simulation
   int m_dfltSlicesLut;
   /// Property: Default number of FADC slices in simulation
   int m_dfltSlicesFadc;
   /// Property: Force number of LUT slices in bytestream
   int m_forceSlicesLut;
   /// Property: Force number of FADC slices in bytestream
   int m_forceSlicesFadc;
   /// Property: Minimum crate number when writing out bytestream
   int m_crateMin;
   /// Property: Maximum crate number when writing out bytestream
   int m_crateMax;
   /// Property: Pedestal value
   int m_pedestal;
   /// Property: FADC baseline lower bound
   int m_fadcBaseline;
   /// Property: FADC threshold for super-compressed format
   int m_fadcThreshold;
   /// Property: Zero suppression on input
   int m_zeroSuppress;
   /// Property: ROB source IDs
   std::vector<uint32_t> m_sourceIDsProp;
   /// Sub-detector type
   const eformat::SubDetector m_subDetector;
   /// Source ID converter
   const L1CaloSrcIdMap m_srcIdMap;
   /// Vector for compression statistics
   static constexpr unsigned int MAXFORMATS = 10;
   std::atomic<uint32_t> m_compStats[MAXFORMATS];

   static const int s_crates   = 8;
   static const int s_modules  = 16;
   static const int s_channels = 64;
   static const int s_dataSize = 3584;

};

} // end namespace

#endif
