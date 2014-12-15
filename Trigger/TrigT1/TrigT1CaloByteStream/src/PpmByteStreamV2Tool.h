/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_PPMBYTESTREAMV2TOOL_H
#define TRIGT1CALOBYTESTREAM_PPMBYTESTREAMV2TOOL2_H
// ===========================================================================
// Includes
// ===========================================================================
// STD:
// ===========================================================================
#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <sstream>
// ===========================================================================
// Atheana:
// ===========================================================================
#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "DataModel/DataVector.h"
#include "eformat/SourceIdentifier.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"


#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

// ===========================================================================
// Forward declarations
// ===========================================================================
class IInterface;
class InterfaceID;
class SegMemSvc;
template <class T> class FullEventAssembler;

namespace xAOD {
  class TriggerTower_v2;
}

namespace LVL1 {
  class IL1CaloMappingTool;
}

// ===========================================================================
namespace LVL1BS {
// ===========================================================================
class L1CaloErrorByteStreamTool;
class L1CaloSrcIdMap;
class PpmSubBlockV2;

/** Tool to perform ROB fragments to trigger towers and trigger towers
 *  to raw data conversions.
 *
 *
 * @author alexander.mazurov@cern.ch 
 * @author Peter Faulkner
 */

class PpmByteStreamV2Tool: public AthAlgTool {

public:
  PpmByteStreamV2Tool(const std::string& type, const std::string& name,
      const IInterface* parent);
  virtual ~PpmByteStreamV2Tool();

  /// AlgTool InterfaceID
  static const InterfaceID& interfaceID();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  /// Convert ROB fragments to trigger towers
  StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
      xAOD::TriggerTowerContainer* ttCollection);

  /// Convert trigger towers to bytestream
  StatusCode convert(const xAOD::TriggerTowerContainer* ttCollection,
      RawEventWrite* re);

  /// Return reference to vector with all possible Source Identifiers
  const std::vector<uint32_t>& sourceIDs(const std::string& sgKey);

private:
  void reserveMemory();
  void collectTriggerTowers(const IROBDataProviderSvc::VROBFRAG& robFrags);
  uint_least32_t coolId(int crate, int module, int channel) const;
 /// Add compression stats to totals
 void addCompStats(const std::vector<uint32_t>& stats);
 /// Print compression stats
 void printCompStats() const;
 /// Print a vector
 template <typename T>
   std::string vectorToString(const std::vector<T>& vec) const;

private:
 // Private functions for encoding
 void setupSourceTowers(const xAOD::TriggerTowerContainer* ttCollection);

private:
  // typedef DataVector<LVL1::TriggerTower2> TriggerTowerCollection;
  typedef std::vector<xAOD::TriggerTower*> TriggerTowerVector;
  typedef std::map<unsigned int, int> TriggerTowerMap;
  typedef std::vector<uint32_t> ChannelBitVector;

  typedef IROBDataProviderSvc::VROBFRAG::const_iterator ROBIterator;
  typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      ROBPointer;
  typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      RODPointer;

  typedef enum {
    Data, Spare, Muon
  } ChannelsType;
  /// Services
  ServiceHandle<SegMemSvc> m_sms;
  /// Source ID converter
  L1CaloSrcIdMap* m_srcIdMap;
  /// Channel mapping tool
  ToolHandle<LVL1::IL1CaloMappingTool> m_ppmMaps;
  // Error collection tool
  ToolHandle<LVL1BS::L1CaloErrorByteStreamTool> m_errorTool;
  /// Current error block
  PpmSubBlockV2* m_errorBlock;
  /// Vector for current PPM sub-blocks
  DataVector<PpmSubBlockV2> m_ppmBlocks;
  /// Vector for compression statistics
  std::vector<uint32_t> m_compStats;


  /// Sub-detector type
  const eformat::SubDetector m_subDetector;
  int m_crates;
  int m_modules;
  int m_channels;
  int m_dataSize;
  int m_maxSlinks;

  ChannelsType m_channelsType;
  ChannelBitVector m_dataChan;
  ChannelBitVector m_dataMod;
  ChannelBitVector m_chanLayer;
  ChannelBitVector m_foundChan;


  /// ROB source IDs
  std::vector<uint32_t> m_sourceIDs;
  std::vector<uint32_t> m_sourceIDsSpare;
  std::vector<uint32_t> m_sourceIDsMuon;


  /// TriggerTower pool vectors
  TriggerTowerVector m_ttData;
  // TriggerTowerVector m_ttSpare;
  // TriggerTowerVector m_ttMuon;
  std::vector<int> m_ttPos;


  /// Compression statistics print flag
  int m_printCompStats;
  /// Pedestal value
  int m_pedestal;
  /// FADC baseline lower bound
  int m_fadcBaseline;

private:
  // For writing to bytestream
  /// ROD status map
  std::map<uint32_t, std::vector<uint32_t>* > m_rodStatusMap;
  // Event assembler
  FullEventAssembler<L1CaloSrcIdMap>* m_fea;

  TriggerTowerVector m_source_towers;

  short m_dataFormat;
  short m_subheaderVersion;

};
// ===========================================================================
template<typename T>
std::string PpmByteStreamV2Tool::vectorToString(const std::vector<T>& vec) const
{
  std::stringstream res;
  for (auto pos = vec.cbegin(); pos != vec.cend(); ++pos) {
    if (pos != vec.begin()) 
      res << ",";
    res << int(*pos);
  }
  res << "/" ;
  return res.str();
}
// ===========================================================================
}// end namespace
// ===========================================================================
#endif
