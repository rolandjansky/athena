/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CopyTrackCollection.h"


CopyTrackCollection::CopyTrackCollection(const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) { }

StatusCode CopyTrackCollection::initialize()
{
  ATH_MSG_DEBUG("Initializing...");

  // Check and initialize keys
  ATH_CHECK( m_inputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_inputKey);
  ATH_CHECK( m_outputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputKey);

  return StatusCode::SUCCESS;
}

StatusCode CopyTrackCollection::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("execute() begin");
  // Reading the input container
  ATH_MSG_VERBOSE("Retrieving input container");

  SG::ReadHandle<TrackCollection> inputContainer(m_inputKey, ctx);
  if (!inputContainer.isValid()) {
    ATH_MSG_ERROR("Could not get pileup TrackCollection " << inputContainer.name() << " from store " << inputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found pileup TrackCollection " << inputContainer.name() << " in store " << inputContainer.store());
  
  // Creating output container
  SG::WriteHandle<TrackCollection> outputContainer(m_outputKey, ctx);
  ATH_CHECK(outputContainer.record(std::make_unique<TrackCollection>()));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output TrackCollection " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output TrackCollection container " << outputContainer.name() << " in store " << outputContainer.store());

  // Copy pileup tracks
  for (const Trk::Track* track : *inputContainer) {
    outputContainer->push_back(new Trk::Track(*track));
  }

  ATH_MSG_DEBUG("execute() end");
  return StatusCode::SUCCESS;
}
