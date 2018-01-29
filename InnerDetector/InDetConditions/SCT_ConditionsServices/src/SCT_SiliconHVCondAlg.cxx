/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SiliconHVCondAlg.h"

#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"

#include "GaudiKernel/EventIDRange.h"

SCT_SiliconHVCondAlg::SCT_SiliconHVCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthAlgorithm(name, pSvcLocator)
  , m_readKeyState{"SCT_DCSStatCondData"}
  , m_readKeyHV{"SCT_DCSHVCondData"}
  , m_writeKey{"SCT_SiliconBiasVoltCondData"}
  , m_condSvc{"CondSvc", name}
  , m_sctDCSSvc{"InDetSCT_DCSConditionsSvc", name}
  , m_pHelper{nullptr}
{
  declareProperty("ReadKeyState", m_readKeyState, "Key of input state conditions folder");
  declareProperty("ReadKeyHV", m_readKeyHV, "Key of input HV conditions folder");
  declareProperty("WriteKey", m_writeKey, "Key of output bias voltage conditions folder");
}

SCT_SiliconHVCondAlg::~SCT_SiliconHVCondAlg() {
}

StatusCode SCT_SiliconHVCondAlg::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  // SCT DCS service
  ATH_CHECK(m_sctDCSSvc.retrieve());
  // SCT ID helper
  ATH_CHECK(detStore()->retrieve(m_pHelper, "SCT_ID"));

  // CondSvc
  ATH_CHECK(m_condSvc.retrieve());
  // Read Cond Handles
  ATH_CHECK(m_readKeyState.initialize());
  ATH_CHECK(m_readKeyHV.initialize());
  // Write Cond Handle
  ATH_CHECK(m_writeKey.initialize());
  if(m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode SCT_SiliconHVCondAlg::execute() {
  ATH_MSG_DEBUG("execute " << name());

  // Write Cond Handle
  SG::WriteCondHandle<SCT_DCSFloatCondData> writeHandle{m_writeKey};
  // Do we have a valid Write Cond Handle for current time?
  if (writeHandle.isValid()) {
    // in theory this should never be called in MT
    writeHandle.updateStore();
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << " In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order."
                  << " Forcing update of Store contents");
    return StatusCode::SUCCESS; 
  }

  // Read Cond Handle (state)
  SG::ReadCondHandle<SCT_DCSStatCondData> readHandleState{m_readKeyState};
  const SCT_DCSStatCondData* readCdoState{*readHandleState};
  if (readCdoState==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  EventIDRange rangeState;
  if (not readHandleState.range(rangeState)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandleState.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Input is " << readHandleState.fullKey() << " with the range of " << rangeState);

  // Read Cond Handle (HV)
  SG::ReadCondHandle<SCT_DCSFloatCondData> readHandleHV{m_readKeyHV};
  const SCT_DCSFloatCondData* readCdoHV{*readHandleHV};
  if (readCdoHV==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  EventIDRange rangeHV;
  if (not readHandleHV.range(rangeHV)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandleHV.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Input is " << readHandleHV.fullKey() << " with the range of " << rangeHV);

  // Combined the validity ranges of state and range
  EventIDRange rangeW{EventIDRange::intersect(rangeState, rangeHV)};
  if(rangeW.start()>rangeW.stop()) {
    ATH_MSG_FATAL("Invalid intersection range: " << rangeW);
    return StatusCode::FAILURE;
  }
  
  // Construct the output Cond Object and fill it in
  SCT_DCSFloatCondData* writeCdo{new SCT_DCSFloatCondData()};
  const SCT_ID::size_type wafer_hash_max{m_pHelper->wafer_hash_max()};
  for (SCT_ID::size_type hash{0}; hash<wafer_hash_max; hash++) {
    writeCdo->setValue(hash, m_sctDCSSvc->modHV(IdentifierHash(hash)));
  }

  // Record the output cond object
  if (writeHandle.record(rangeW, writeCdo).isFailure()) {
    ATH_MSG_FATAL("Could not record SCT_DCSFloatCondData " << writeHandle.key() 
                  << " with EventRange " << rangeW
                  << " into Conditions Store");
    delete writeCdo;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode SCT_SiliconHVCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}
