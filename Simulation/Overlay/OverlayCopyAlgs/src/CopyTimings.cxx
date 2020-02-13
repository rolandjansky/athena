/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CopyTimings.h"

CopyTimings::CopyTimings(const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode CopyTimings::initialize()
{
  ATH_MSG_DEBUG("Initializing...");

  // Check and initialize keys
  ATH_CHECK( m_bkgInputKey.initialize(!m_bkgInputKey.key().empty()) );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_bkgInputKey);
  ATH_CHECK( m_signalInputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_signalInputKey);
  ATH_CHECK( m_outputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputKey);

  return StatusCode::SUCCESS;
}

StatusCode CopyTimings::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("execute() begin");

  // Reading the input timings
  ATH_MSG_VERBOSE("Retrieving input timing containers");

  const RecoTimingObj *bkgContainerPtr = nullptr;
  if (!m_bkgInputKey.key().empty()) {
    SG::ReadHandle<RecoTimingObj> bkgContainer(m_bkgInputKey, ctx);
    if (!bkgContainer.isValid()) {
      ATH_MSG_ERROR("Could not get background timings container " << bkgContainer.name() << " from store " << bkgContainer.store());
      return StatusCode::FAILURE;
    }
    bkgContainerPtr = bkgContainer.cptr();

    ATH_MSG_DEBUG("Found background timings container " << bkgContainer.name() << " in store " << bkgContainer.store());
  }

  SG::ReadHandle<RecoTimingObj> signalContainer(m_signalInputKey, ctx);
  if (!signalContainer.isValid()) {
    ATH_MSG_ERROR("Could not get signal timings container " << signalContainer.name() << " from store " << signalContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found signal timings container " << signalContainer.name() << " in store " << signalContainer.store());

  // Creating output timings container
  SG::WriteHandle<RecoTimingObj> outputContainer(m_outputKey, ctx);
  ATH_CHECK(outputContainer.record(std::make_unique<RecoTimingObj>()));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output timings container " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output timings container " << outputContainer.name() << " in store " << outputContainer.store());

  if (!m_bkgInputKey.key().empty()) {
    outputContainer->insert(outputContainer->end(), bkgContainerPtr->begin(), bkgContainerPtr->end());
  }
  outputContainer->insert(outputContainer->end(), signalContainer->begin(), signalContainer->end());

  ATH_MSG_DEBUG("execute() end");
  return StatusCode::SUCCESS;
}
