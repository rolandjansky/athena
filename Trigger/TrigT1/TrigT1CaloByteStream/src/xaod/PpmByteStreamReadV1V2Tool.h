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


// ===========================================================================
// Athena:
// ===========================================================================
#include "AsgTools/AsgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "TrigT1CaloMappingToolInterfaces/IL1CaloMappingTool.h"

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

class PpmByteStreamReadV1V2Tool: public asg::AsgTool, virtual public IIncidentListener {
  ASG_TOOL_INTERFACE(PpmByteStreamReadV1V2Tool)
  ASG_TOOL_CLASS0(PpmByteStreamReadV1V2Tool)
public:
  PpmByteStreamReadV1V2Tool(const std::string& name);
  virtual ~PpmByteStreamReadV1V2Tool() {};

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual void handle( const Incident& );

  // =========================================================================
  /// Convert ROB fragments to trigger towers
  StatusCode convert(
    const IROBDataProviderSvc::VROBFRAG& robFrags,
    xAOD::TriggerTowerContainer* const ttCollection
  );
  StatusCode convert(xAOD::TriggerTowerContainer* const ttCollection);
  StatusCode convert(const std::string& sgKey, xAOD::TriggerTowerContainer* const ttCollection);
  // =========================================================================
  /// Return reference to vector with all possible Source Identifiers
  const std::vector<uint32_t>& ppmSourceIDs(const std::string& sgKey);

private:
  enum class RequestType { PPM, CPM, CMX };
  typedef IROBDataProviderSvc::VROBFRAG::const_iterator ROBIterator;
  typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      ROBPointer;
  typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      RODPointer;


private:
  StatusCode processRobFragment_(const ROBIterator& robFrag,
                                 const RequestType& requestedType);

  // ==========================================================================
  // PPM
  // ==========================================================================
  StatusCode processPpmWord_(uint32_t word, int indata);
  StatusCode processPpmBlock_();

  StatusCode processPpmBlockR4V1_();
  StatusCode processPpmBlockR3V1_();
  StatusCode processPpmStandardR4V1_();
  StatusCode processPpmStandardR3V1_();
  StatusCode processPpmStandardR3V1_(uint32_t word, int indata);
  StatusCode processPpmCompressedR3V1_();
  std::vector<uint16_t> getPpmAdcSamplesR3_(uint8_t format, uint8_t minIndex);
  StatusCode processPpmCompressedR4V1_();
  void interpretPpmHeaderR4V1_(uint8_t numAdc, int8_t& encoding,
                               int8_t& minIndex);
  std::vector<uint16_t> getPpmAdcSamplesR4_(uint8_t encoding, uint8_t minIndex);
  StatusCode processPpmNeutral_();
  uint32_t getPpmBytestreamField_(uint8_t numBits);

  StatusCode addTriggerTowerV2_(
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
    std::vector<uint8_t>&& pedEn);

  StatusCode addTriggerTowerV1_(
    uint8_t crate,
    uint8_t module,
    uint8_t channel,
    const std::vector<uint16_t>& luts,
    const std::vector<uint16_t>& fadc
  );

  StatusCode addTriggerTowerV1_(
    uint8_t crate,
    uint8_t module,
    uint8_t channel,
    std::vector<uint8_t>&& luts,
    std::vector<uint8_t>&& lcpBcidVec,
    std::vector<uint16_t>&& fadc,
    std::vector<uint8_t>&& bcidExt
  );

  void processSubBlockStatus_(uint8_t crate, uint8_t module, uint32_t word);

private:
  ToolHandle<LVL1BS::L1CaloErrorByteStreamTool> m_errorTool;
  /// Channel mapping tool
  ToolHandle<LVL1::IL1CaloMappingTool> m_ppmMaps;
  /// Service for reading bytestream
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;

private:
  CaloUserHeader m_caloUserHeader;
  SubBlockHeader m_subBlockHeader;
  // SubBlockStatus m_subBlockStatus;

  uint8_t m_subDetectorID;
  RequestType m_requestedType;

  std::set<uint32_t> m_coolIds;
  std::vector<uint32_t> m_ppmSourceIDs;
  bool m_ppmIsRetMuon;
  std::vector<uint32_t> m_ppmSourceIDsMuon;
  bool m_ppmIsRetSpare;
  std::vector<uint32_t> m_ppmSourceIDsSpare;
  std::vector<uint32_t> m_cpSourceIDs;
  L1CaloSrcIdMap* m_srcIdMap;

  uint32_t m_rodSourceId;
  uint32_t m_robSourceId;
  uint32_t m_rodRunNumber;
  uint16_t m_rodVer;
  uint8_t m_verCode;

  // For RUN2
  std::vector<uint32_t> m_ppBlock;
  uint32_t m_ppPointer;
  uint32_t m_ppMaxBit;
  // For RUN1
  std::map<uint8_t, std::vector<uint16_t>> m_ppLuts;
  std::map<uint8_t, std::vector<uint16_t>> m_ppFadcs;
  size_t m_maxSizeSeen;
// ==========================================================================
private:
  xAOD::TriggerTowerContainer* m_triggerTowers;

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
