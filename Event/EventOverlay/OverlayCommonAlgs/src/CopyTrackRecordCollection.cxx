/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CopyTrackRecordCollection.h"


CopyTrackRecordCollection::CopyTrackRecordCollection(const std::string &name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator) { }

StatusCode CopyTrackRecordCollection::initialize()
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

StatusCode CopyTrackRecordCollection::execute()
{
  ATH_MSG_DEBUG("execute() begin");

  // Reading the input containers
  ATH_MSG_VERBOSE("Retrieving input containers");

  SG::ReadHandle<TrackRecordCollection> signalContainer(m_signalInputKey);
  if (!signalContainer.isValid()) {
    ATH_MSG_ERROR("Could not get signal TrackRecordCollection container " << signalContainer.name() << " from store " << signalContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found signal TrackRecordCollection container " << signalContainer.name() << " in store " << signalContainer.store());

  // Creating output RDO container
  SG::WriteHandle<TrackRecordCollection> outputContainer(m_outputKey);
  ATH_CHECK(outputContainer.record(std::make_unique<TrackRecordCollection>(m_collectionName)));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output TrackRecordCollection container " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output TrackRecordCollection container " << outputContainer.name() << " in store " << outputContainer.store());

  // Copy signal TrackRecords
  for (const TrackRecord &record : *signalContainer) {
    outputContainer->push_back(TrackRecord(record));
  }

  ATH_MSG_DEBUG("execute() end");
  return StatusCode::SUCCESS;
}
