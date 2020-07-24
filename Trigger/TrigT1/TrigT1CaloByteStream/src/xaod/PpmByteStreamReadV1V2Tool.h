/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_PPMBYTESTREAMREADV1V2TOOL_H
#define TRIGT1CALOBYTESTREAM_PPMBYTESTREAMREADV1V2TOOL_H
// ===========================================================================
// Includes
// ===========================================================================
// STD:
// ===========================================================================
#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <atomic>


// ===========================================================================
// Athena:
// ===========================================================================
#include "AsgTools/AsgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "TrigT1CaloToolInterfaces/IL1CaloMappingTool.h"

#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

#include "CaloUserHeader.h"
#include "SubBlockHeader.h"
#include "SubBlockStatus.h"

#include "../L1CaloErrorByteStreamTool.h"

// ===========================================================================
// Forward declarations
// ===========================================================================


// ===========================================================================
namespace LVL1BS {

// Forward declarations
class L1CaloSrcIdMap;
class L1CaloErrorByteStreamTool;
// ===========================================================================

/** Tool to perform ROB fragments to trigger towers and trigger towers
 *  to raw data conversions.
 *
 *
 * @author alexander.mazurov@cern.ch
 */

class PpmByteStreamReadV1V2Tool: public asg::AsgTool
{
  ASG_TOOL_INTERFACE(PpmByteStreamReadV1V2Tool)
  ASG_TOOL_CLASS0(PpmByteStreamReadV1V2Tool)
public:
  PpmByteStreamReadV1V2Tool(const std::string& name);
  virtual ~PpmByteStreamReadV1V2Tool() {};

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  // =========================================================================
  /// Convert ROB fragments to trigger towers
  StatusCode convert(xAOD::TriggerTowerContainer* const ttCollection) const;
  StatusCode convert(const std::string& sgKey, xAOD::TriggerTowerContainer* const ttCollection) const;
  // =========================================================================

private:
  enum class RequestType { PPM, CPM, CMX };
  typedef IROBDataProviderSvc::VROBFRAG::const_iterator ROBIterator;
  typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      ROBPointer;
  typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      RODPointer;

  class BitReader
  {
  public:
    BitReader (const std::vector<uint32_t>& ppBlock)
      : m_ppPointer (0),
        m_ppMaxBit (31 * ppBlock.size()),
        m_ppBlock (ppBlock)
    {
    }

    uint32_t getField (const uint8_t numBits);

  private:
    uint32_t m_ppPointer;
    uint32_t m_ppMaxBit;
    const std::vector<uint32_t>& m_ppBlock;
  };


  typedef std::map<uint8_t, std::vector<uint16_t>> LutsMap;
  typedef std::map<uint8_t, std::vector<uint16_t>> FadcsMap;

  struct State
  {
    uint32_t m_rodSourceId = 0;
    uint32_t m_robSourceId = 0;
    uint8_t m_verCode = 0;
    uint8_t m_subDetectorID = 0;
    bool m_ppmIsRetMuon = false;
    bool m_ppmIsRetSpare = false;
    CaloUserHeader m_caloUserHeader;
    SubBlockHeader m_subBlockHeader;
    xAOD::TriggerTowerContainer* m_triggerTowers;
    LutsMap m_ppLuts;
    FadcsMap m_ppFadcs;
    std::vector<uint32_t> m_ppBlock;
  };


private:
  StatusCode convert(
    State& state,
    const IROBDataProviderSvc::VROBFRAG& robFrags) const;


  void initSourceIDs();

  /// Return reference to vector with all possible Source Identifiers
  const std::vector<uint32_t>& ppmSourceIDs(State& state,
                                            const std::string& sgKey) const;

  StatusCode processRobFragment_(State& state,
                                 const ROBIterator& robFrag,
                                 const RequestType& requestedType) const;

  // ==========================================================================
  // PPM
  // ==========================================================================
  StatusCode processPpmWord_(State& state,
                             uint32_t word, int indata) const;
  StatusCode processPpmBlock_(State& state) const;

  StatusCode processPpmBlockR4V1_(State& state) const;
  StatusCode processPpmBlockR3V1_(State& state) const;
  StatusCode processPpmStandardR4V1_(State& state) const;
  StatusCode processPpmStandardR3V1_(State& state) const;
  StatusCode processPpmStandardR3V1_(State& state,
                                     uint32_t word, int indata) const;
  StatusCode processPpmCompressedR3V1_(State& state) const;
  std::vector<uint16_t> getPpmAdcSamplesR3_(State& state,
                                            BitReader& br, uint8_t format, uint8_t minIndex) const;
  StatusCode processPpmCompressedR4V1_(State& state) const;
  void interpretPpmHeaderR4V1_(BitReader& br,
                               uint8_t numAdc, int8_t& encoding,
                               int8_t& minIndex) const;
  std::vector<uint16_t> getPpmAdcSamplesR4_(State& state,
                                            BitReader& br,
                                            uint8_t encoding, uint8_t minIndex) const;
  StatusCode processPpmNeutral_(State& state) const;

  StatusCode addTriggerTowerV2_(
    State& state,
    uint8_t crate,
    uint8_t module,
    uint8_t channel,
    std::vector<uint8_t>&& lcpVal,
    std::vector<uint8_t>&& lcpBcidVec,

    std::vector<uint8_t>&& ljeVal,
    std::vector<uint8_t>&& ljeSat80Vec,

    std::vector<uint16_t>&& adcVal,
    std::vector<uint8_t>&& adcExt,
    std::vector<int16_t>&& pedCor,
    std::vector<uint8_t>&& pedEn) const;

  StatusCode addTriggerTowerV1_(
    State& state,
    uint8_t crate,
    uint8_t module,
    uint8_t channel,
    const std::vector<uint16_t>& luts,
    const std::vector<uint16_t>& fadc
  ) const;

  StatusCode addTriggerTowerV1_(
    State& state,
    uint8_t crate,
    uint8_t module,
    uint8_t channel,
    std::vector<uint8_t>&& luts,
    std::vector<uint8_t>&& lcpBcidVec,
    std::vector<uint16_t>&& fadc,
    std::vector<uint8_t>&& bcidExt
  ) const;

  void processSubBlockStatus_(State& state,
                              uint8_t crate, uint8_t module, uint32_t word) const;

private:
  ToolHandle<LVL1BS::L1CaloErrorByteStreamTool> m_errorTool;
  /// Channel mapping tool
  ToolHandle<LVL1::IL1CaloMappingTool> m_ppmMaps;
  /// Service for reading bytestream
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;

private:
  std::vector<uint32_t> m_ppmSourceIDs;
  std::vector<uint32_t> m_ppmSourceIDsMuon;
  std::vector<uint32_t> m_ppmSourceIDsSpare;
  L1CaloSrcIdMap* m_srcIdMap;

  mutable std::atomic<size_t> m_maxSizeSeen;

// ==========================================================================
private:
   static const uint8_t s_crates   = 8;
   static const uint8_t s_modules  = 16;
   static const uint8_t s_channels = 64;
   static const uint16_t s_maxtowers = s_crates * s_modules * s_channels;
};

// ===========================================================================
}// end namespace
// ===========================================================================
#endif
