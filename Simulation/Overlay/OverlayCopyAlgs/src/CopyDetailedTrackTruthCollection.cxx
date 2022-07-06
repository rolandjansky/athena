/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CopyDetailedTrackTruthCollection.h"


CopyDetailedTrackTruthCollection::CopyDetailedTrackTruthCollection(const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) { }

StatusCode CopyDetailedTrackTruthCollection::initialize()
{
  ATH_MSG_DEBUG("Initializing...");

  // Check and initialize keys
  ATH_CHECK( m_inputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_inputKey);
  ATH_CHECK( m_outputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputKey);

  return StatusCode::SUCCESS;
}

StatusCode CopyDetailedTrackTruthCollection::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("execute() begin");
  // Reading the input container
  ATH_MSG_VERBOSE("Retrieving input container");

  SG::ReadHandle<DetailedTrackTruthCollection> inputContainer(m_inputKey, ctx);
  if (!inputContainer.isValid()) {
    ATH_MSG_ERROR("Could not get pileup DetailedTrackTruthCollection " << inputContainer.name() << " from store " << inputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found pileup DetailedTrackTruthCollection " << inputContainer.name() << " in store " << inputContainer.store());

  // Creating output RDO container
  SG::WriteHandle<DetailedTrackTruthCollection> outputContainer(m_outputKey, ctx);
  ATH_CHECK(outputContainer.record(std::make_unique<DetailedTrackTruthCollection>(inputContainer->trackCollectionLink())));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output DetailedTrackTruthCollection " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output DetailedTrackTruthCollection container " << outputContainer.name() << " in store " << outputContainer.store());

  for(const std::pair<const Trk::TrackTruthKey, DetailedTrackTruth>& element : *inputContainer){
    outputContainer->insert(std::make_pair(element.first,element.second));
  }
  return StatusCode::SUCCESS;
}
