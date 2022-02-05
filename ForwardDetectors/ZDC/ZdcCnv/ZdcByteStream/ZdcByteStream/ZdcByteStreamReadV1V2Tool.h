/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_ZDCBYTESTREAMREADV1V2TOOL_H
#define TRIGT1CALOBYTESTREAM_ZDCBYTESTREAMREADV1V2TOOL_H
// ===========================================================================
// Includes
// ===========================================================================
// STD:
// ===========================================================================
#include <stdint.h>
#include <vector>

// ===========================================================================
// Athena:
// ===========================================================================
#include "AsgTools/AsgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

//#include "xAODForward/ZdcModule.h"
//#include "xAODForward/ZdcModuleContainer.h"

#include "ZdcByteStream/ZdcCaloUserHeader.h"
#include "ZdcByteStream/ZdcSubBlockHeader.h"
#include "ZdcByteStream/ZdcSubBlockStatus.h"
#include "ZdcEvent/ZdcDigitsCollection.h"

//#include "TrigT1CaloByteStream/L1CaloErrorByteStreamTool.h"
#include "ZdcByteStream/ZdcL1CaloErrorByteStreamTool.h"

// ===========================================================================
// Forward declarations
// ===========================================================================


// ===========================================================================
class ZdcSrcIdMap;
class ZdcID;
class ZdcL1CaloErrorByteStreamTool;
namespace LVL1BS {
// Forward declarations
//class L1CaloErrorByteStreamTool;
// ===========================================================================
}// end namespace

// instead let's use their stupid namespace
//using namespace LVL1BS;
// ===========================================================================

// ===========================================================================

/** Tool to perform ROB fragments to trigger towers and trigger towers
 *  to raw data conversions.
 *
 *
 * @author alexander.mazurov@cern.ch
 */

class ZdcByteStreamReadV1V2Tool: public asg::AsgTool
{
  ASG_TOOL_INTERFACE(ZdcByteStreamReadV1V2Tool)
  ASG_TOOL_CLASS0(ZdcByteStreamReadV1V2Tool)
public:
  ZdcByteStreamReadV1V2Tool(const std::string& name);
  virtual ~ZdcByteStreamReadV1V2Tool() {};

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  // =========================================================================
  /// Convert ROB fragments to trigger towers
  StatusCode convert(
    const IROBDataProviderSvc::VROBFRAG& robFrags,
    xAOD::TriggerTowerContainer* const ttCollection
  ) const;

  StatusCode convert(	const IROBDataProviderSvc::VROBFRAG& robFrags,
			ZdcDigitsCollection* zdcCollection) const;
  
  StatusCode convert(xAOD::TriggerTowerContainer* const ttCollection) const;
  StatusCode convert(const std::string& sgKey, xAOD::TriggerTowerContainer* const ttCollection) const;

  ZdcDigitsCollection* convertTT2ZD(xAOD::TriggerTowerContainer* const ttCollection) const;
  //xAOD::ZdcModuleContainer* convertTT2ZM(xAOD::TriggerTowerContainer* const ttCollection);


  // =========================================================================
  /// Return reference to vector with all possible Source Identifiers
  const std::vector<uint32_t>& ppmSourceIDs(const std::string& sgKey) const;

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


  struct State
  {
    std::set<uint32_t> m_coolIds;
    xAOD::TriggerTowerContainer* m_triggerTowers = nullptr;

    ZdcCaloUserHeader m_caloUserHeader;
    ZdcSubBlockHeader m_subBlockHeader;

    uint8_t m_subDetectorID = 0;
    uint32_t m_rodSourceId = 0;
    uint32_t m_robSourceId = 0;
    uint8_t m_verCode = 0;

    // For RUN2
    std::vector<uint32_t> m_ppBlock;
    // For RUN1
    std::map<uint8_t, std::vector<uint16_t>> m_ppFadcs;
    std::map<uint8_t, std::vector<uint16_t>> m_ppLuts;
  };


private:
  StatusCode processRobFragment_(State& state,
                                 const ROBIterator& robFrag,
                                 const RequestType& requestedType) const;

  // ==========================================================================
  // PPM
  // ==========================================================================
  StatusCode processPpmWord_(State& state, uint32_t word, int indata) const;
  StatusCode processPpmBlock_(State& state) const;

  StatusCode processPpmBlockR4V1_(State& state) const;
  StatusCode processPpmBlockR3V1_(State& state) const;
  StatusCode processPpmStandardR4V1_(State& state) const;
  StatusCode processPpmStandardR3V1_(State& state) const;
  StatusCode processPpmStandardR3V1_(State& state, uint32_t word, int indata) const;
  StatusCode processPpmCompressedR3V1_(State& state) const;
  std::vector<uint16_t> getPpmAdcSamplesR3_(State& state, BitReader& br, uint8_t format, uint8_t minIndex) const;
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
    const std::vector<uint8_t>& lcpVal,
    const std::vector<uint8_t>& lcpBcidVec,

    const std::vector<uint8_t>& ljeVal,
    const std::vector<uint8_t>& ljeSat80Vec,

    const std::vector<uint16_t>& adcVal,
    const std::vector<uint8_t>& adcExt,
    const std::vector<int16_t>& pedCor,
    const std::vector<uint8_t>& pedEn) const;

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
    const std::vector<uint8_t>& luts,
    const std::vector<uint8_t>& lcpBcidVec,
    const std::vector<uint16_t>& fadc,
    const std::vector<uint8_t>& bcidExt
  ) const;

  void initSourceIDs();

private:
  //ToolHandle<LVL1BS::L1CaloErrorByteStreamTool> m_errorTool;
  ToolHandle<ZdcL1CaloErrorByteStreamTool> m_errorTool;
  /// Channel mapping tool
  //ToolHandle<LVL1::IL1CaloMappingTool> m_ppmMaps;
  /// Service for reading bytestream
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;

private:
  std::vector<uint32_t> m_ppmSourceIDs;
  std::vector<uint32_t> m_ppmSourceIDsMuon;
  std::vector<uint32_t> m_ppmSourceIDsSpare;
  ZdcSrcIdMap* m_srcIdMap;
  const ZdcID* m_zdcID;
};


// PAS 8/14 used to be here
// ===========================================================================
//}// end namespace
// ===========================================================================
#endif
