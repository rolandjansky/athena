/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_PPMBYTESTREAMV1TOOL_H
#define TRIGT1CALOBYTESTREAM_PPMBYTESTREAMV1TOOL_H

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "DataModel/DataVector.h"
#include "eformat/SourceIdentifier.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

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

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /// Convert ROB fragments to trigger towers
   StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                      DataVector<LVL1::TriggerTower>* ttCollection);

   /// Convert trigger towers to bytestream
   StatusCode convert(const DataVector<LVL1::TriggerTower>* ttCollection,
                      RawEventWrite* re);

   /// Return reference to vector with all possible Source Identifiers
   const std::vector<uint32_t>& sourceIDs(const std::string& sgKey);

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
   void addCompStats(const std::vector<uint32_t>& stats);
   /// Print compression stats
   void printCompStats() const;

   /// Find a trigger tower using separate layer maps
   const LVL1::TriggerTower* findLayerTriggerTower(double eta, double phi,
                                                               int layer);
   /// Set up separate Em and Had trigger tower maps
   void setupTTMaps(const TriggerTowerCollection* ttCollection);

   /// Get number of slices and triggered slice offsets for next slink
   bool slinkSlices(int crate, int module, int modulesPerSlink,
        int& slicesLut, int& slicesFadc, int& trigLut, int& trigFadc);

   /// Print a vector
   void printVec(const std::vector<int>& vec) const;

   /// Channel mapping tool
   ToolHandle<LVL1::IL1CaloMappingTool> m_ppmMaps;
   /// Error collection tool
   ToolHandle<LVL1BS::L1CaloErrorByteStreamTool> m_errorTool;
   /// Memory pool service
   ServiceHandle<SegMemSvc> m_sms;

   /// Sub_block header version
   int m_version;
   /// Data compression format
   int m_dataFormat;
   /// Compression version
   int m_compVers;
   /// Compression statistics print flag
   int m_printCompStats;
   /// Number of slinks per crate when writing out bytestream
   int m_slinks;
   /// Default number of LUT slices in simulation
   int m_dfltSlicesLut;
   /// Default number of FADC slices in simulation
   int m_dfltSlicesFadc;
   /// Force number of LUT slices in bytestream
   int m_forceSlicesLut;
   /// Force number of FADC slices in bytestream
   int m_forceSlicesFadc;
   /// Minimum crate number when writing out bytestream
   int m_crateMin;
   /// Maximum crate number when writing out bytestream
   int m_crateMax;
   /// Pedestal value
   int m_pedestal;
   /// FADC baseline lower bound
   int m_fadcBaseline;
   /// FADC threshold for super-compressed format
   int m_fadcThreshold;
   /// Zero suppression on input
   int m_zeroSuppress;
   /// Data channel flag
   bool m_dataChannels;
   /// Spare channel flag
   bool m_spareChannels;
   /// Tile Muon channel flag
   bool m_muonChannels;
   /// ROB source IDs
   std::vector<uint32_t> m_sourceIDs;
   std::vector<uint32_t> m_sourceIDsSpare;
   std::vector<uint32_t> m_sourceIDsMuon;
   /// Sub-detector type
   eformat::SubDetector m_subDetector;
   /// Source ID converter
   L1CaloSrcIdMap* m_srcIdMap;
   /// Trigger tower key provider
   LVL1::TriggerTowerKey* m_towerKey;
   /// Current error block
   PpmSubBlockV1* m_errorBlock;
   /// Vector for current PPM sub-blocks
   DataVector<PpmSubBlockV1> m_ppmBlocks;
   /// Vector for compression statistics
   std::vector<uint32_t> m_compStats;
   /// Trigger tower map for conversion from bytestream
   TriggerTowerMap m_ttMap;
   /// Trigger tower map for conversion EM to bytestream
   TriggerTowerMapConst m_ttEmMap;
   /// Trigger tower map for conversion Had to bytestream
   TriggerTowerMapConst m_ttHadMap;
   /// ROD Status words
   std::vector<uint32_t>* m_rodStatus;
   /// ROD status map
   std::map<uint32_t, std::vector<uint32_t>* > m_rodStatusMap;
   /// Event assembler
   FullEventAssembler<L1CaloSrcIdMap>* m_fea;
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

   static const int s_crates   = 8;
   static const int s_modules  = 16;
   static const int s_channels = 64;
   static const int s_dataSize = 3584;

};

} // end namespace

#endif
