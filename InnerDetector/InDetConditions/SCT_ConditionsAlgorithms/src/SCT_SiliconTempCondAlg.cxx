/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SiliconTempCondAlg.h"

#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"

#include <memory>

SCT_SiliconTempCondAlg::SCT_SiliconTempCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode SCT_SiliconTempCondAlg::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  // SCT DCS tool
  ATH_CHECK(m_sctDCSTool.retrieve());
  // SCT ID helper
  ATH_CHECK(detStore()->retrieve(m_pHelper, "SCT_ID"));

  // CondSvc
  ATH_CHECK(m_condSvc.retrieve());
  // Read Cond Handle
  ATH_CHECK(m_readKeyState.initialize(m_useState));
  ATH_CHECK(m_readKeyTemp0.initialize());
  // Write Cond Handles
  ATH_CHECK(m_writeKey.initialize());
  if (m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode SCT_SiliconTempCondAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("execute " << name());

  // Write Cond Handle
  SG::WriteCondHandle<SCT_DCSFloatCondData> writeHandle{m_writeKey, ctx};
  // Do we have a valid Write Cond Handle for current time?
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  // Read Cond Handle (temperature)
  SG::ReadCondHandle<SCT_DCSFloatCondData> readHandleTemp0{m_readKeyTemp0, ctx};
  const SCT_DCSFloatCondData* readCdoTemp0{*readHandleTemp0};
  if (readCdoTemp0==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  writeHandle.addDependency(readHandleTemp0);
  ATH_MSG_INFO("Input is " << readHandleTemp0.fullKey() << " with the range of " << readHandleTemp0.getRange());

  if (m_useState) {
    // Read Cond Handle (state)
    SG::ReadCondHandle<SCT_DCSStatCondData> readHandleState{m_readKeyState, ctx};
    const SCT_DCSStatCondData* readCdoState{*readHandleState};
    if (readCdoState==nullptr) {
      ATH_MSG_FATAL("Null pointer to the read conditions object");
      return StatusCode::FAILURE;
    }
    writeHandle.addDependency(readHandleState);
    ATH_MSG_INFO("Input is " << readHandleState.fullKey() << " with the range of " << readHandleState.getRange());
  }

  // Construct the output Cond Object and fill it in
  std::unique_ptr<SCT_DCSFloatCondData> writeCdo{std::make_unique<SCT_DCSFloatCondData>()};
  const SCT_ID::size_type wafer_hash_max{m_pHelper->wafer_hash_max()};
  for (SCT_ID::size_type hash{0}; hash<wafer_hash_max; hash++) {
    writeCdo->setValue(hash, m_sctDCSTool->sensorTemperature(IdentifierHash(hash), ctx));
  }

  // Record the output cond object
  if (writeHandle.record(std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record SCT_DCSFloatCondData " << writeHandle.key()
                  << " with EventRange " << writeHandle.getRange()
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << writeHandle.getRange() << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode SCT_SiliconTempCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}
