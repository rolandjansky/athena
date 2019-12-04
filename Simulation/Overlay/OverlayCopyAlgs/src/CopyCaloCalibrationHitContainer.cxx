/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CopyCaloCalibrationHitContainer.h"


CopyCaloCalibrationHitContainer::CopyCaloCalibrationHitContainer(const std::string &name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator) { }

StatusCode CopyCaloCalibrationHitContainer::initialize()
{
  ATH_MSG_DEBUG("Initializing...");

  // Check that collection name is not empty
  if (m_collectionName.empty()) {
    ATH_MSG_ERROR("CaloCalibrationHitContainer collection name should not be empty");
    return StatusCode::FAILURE;
  }

  // Check and initialize keys
  ATH_CHECK( m_signalInputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_signalInputKey);
  ATH_CHECK( m_outputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputKey);

  return StatusCode::SUCCESS;
}

StatusCode CopyCaloCalibrationHitContainer::execute()
{
  ATH_MSG_DEBUG("execute() begin");

  // Reading the input containers
  ATH_MSG_VERBOSE("Retrieving input containers");

  SG::ReadHandle<CaloCalibrationHitContainer> signalContainer(m_signalInputKey);
  if (!signalContainer.isValid()) {
    ATH_MSG_ERROR("Could not get signal CaloCalibrationHitContainer container " << signalContainer.name() << " from store " << signalContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found signal CaloCalibrationHitContainer container " << signalContainer.name() << " in store " << signalContainer.store());

  // Creating output RDO container
  SG::WriteHandle<CaloCalibrationHitContainer> outputContainer(m_outputKey);
  ATH_CHECK(outputContainer.record(std::make_unique<CaloCalibrationHitContainer>(m_collectionName)));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output CaloCalibrationHitContainer container " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output CaloCalibrationHitContainer container " << outputContainer.name() << " in store " << outputContainer.store());

  // Copy signal CaloHits
  for (const CaloCalibrationHit *record : *signalContainer) {
    outputContainer->push_back(new CaloCalibrationHit(*record));
  }

  ATH_MSG_DEBUG("execute() end");
  return StatusCode::SUCCESS;
}
