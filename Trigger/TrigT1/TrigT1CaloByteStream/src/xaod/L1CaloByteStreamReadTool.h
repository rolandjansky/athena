/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_L1CALOBYTESTREAMREADTOOL_H
#define TRIGT1CALOBYTESTREAM_L1CALOBYTESTREAMREADTOOL_H
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
#include "TrigT1CaloMappingToolInterfaces/IL1CaloMappingTool.h"

#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

#include "xAODTrigL1Calo/CPMTower.h"
#include "xAODTrigL1Calo/CPMTowerContainer.h"

#include "CaloUserHeader.h"
#include "SubBlockHeader.h"
#include "SubBlockStatus.h"
#include "CpmWord.h"

#include "../L1CaloErrorByteStreamTool.h"

// ===========================================================================
// Forward declarations
// ===========================================================================


// ===========================================================================
namespace LVL1BS {

// Forward declarations
class L1CaloSrcIdMap;
class CpmWord;
class L1CaloErrorByteStreamTool;
// ===========================================================================

/** Tool to perform ROB fragments to trigger towers and trigger towers
 *  to raw data conversions.
 *
 *
 * @author alexander.mazurov@cern.ch
 */

class L1CaloByteStreamReadTool: public asg::AsgTool, virtual public IIncidentListener {
	ASG_TOOL_INTERFACE(L1CaloByteStreamReadTool)
	ASG_TOOL_CLASS0(L1CaloByteStreamReadTool)
public:
  L1CaloByteStreamReadTool(const std::string& name);
  virtual ~L1CaloByteStreamReadTool(){};

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
  StatusCode convert(
      const IROBDataProviderSvc::VROBFRAG& robFrags,
      xAOD::CPMTowerContainer* const cpmCollection
    );
  StatusCode convert(xAOD::CPMTowerContainer* const cpmCollection);
  StatusCode convert(const std::string& sgKey,
    xAOD::CPMTowerContainer* const cpmCollection);
  // =========================================================================
  /// Return reference to vector with all possible Source Identifiers
  const std::vector<uint32_t>& ppmSourceIDs(const std::string& sgKey);
  const std::vector<uint32_t>& cpSourceIDs();

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
      const std::vector<uint8_t>& lcpVal,
      const std::vector<uint8_t>& lcpBcidVec,

      const std::vector<uint8_t>& ljeVal,
      const std::vector<uint8_t>& ljeSat80Vec,

      const std::vector<uint16_t>& adcVal,
      const std::vector<uint8_t>& adcExt,
      const std::vector<int16_t>& pedCor,
      const std::vector<uint8_t>& pedEn);

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
    const std::vector<uint8_t>& luts,
    const std::vector<uint8_t>& lcpBcidVec,
    const std::vector<uint16_t>& fadc,
    const std::vector<uint8_t>& bcidExt
  );

  // ==========================================================================
  // CPM
  // ==========================================================================
  StatusCode processCpWord_(uint32_t word);
  StatusCode processCpmWordR4V1_(uint32_t word);
  // ==========================================================================

  StatusCode addCpmTower_(uint8_t crate, uint8_t module, const CpmWord& word);
private:
  ServiceHandle<SegMemSvc> m_sms;
  ToolHandle<LVL1BS::L1CaloErrorByteStreamTool> m_errorTool;
  /// Channel mapping tool
  ToolHandle<LVL1::IL1CaloMappingTool> m_ppmMaps;
  ToolHandle<LVL1::IL1CaloMappingTool> m_cpmMaps;
  /// Service for reading bytestream
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;

private:
  CaloUserHeader m_caloUserHeader;
  SubBlockHeader m_subBlockHeader;
  SubBlockStatus m_subBlockStatus;

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

  // ==========================================================================
  // PPM related structures
  // ==========================================================================
  // For RUN2
  std::vector<uint32_t> m_ppBlock;
  uint32_t m_ppPointer;
  uint32_t m_ppMaxBit;
  // For RUN1
  std::map<uint8_t, std::vector<uint16_t>> m_ppLuts;
  std::map<uint8_t, std::vector<uint16_t>> m_ppFadcs;
// ==========================================================================
private:
  xAOD::TriggerTowerContainer* m_triggerTowers;
  xAOD::CPMTowerContainer* m_cpmTowers;
};

// ===========================================================================
}// end namespace
// ===========================================================================
#endif
