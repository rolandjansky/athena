/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CopyPRD_MultiTruthCollection.h"


CopyPRD_MultiTruthCollection::CopyPRD_MultiTruthCollection(const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) { }

StatusCode CopyPRD_MultiTruthCollection::initialize()
{
  ATH_MSG_DEBUG("Initializing...");

  // Check and initialize keys
  ATH_CHECK( m_inputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_inputKey);
  ATH_CHECK( m_outputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputKey);

  return StatusCode::SUCCESS;
}

StatusCode CopyPRD_MultiTruthCollection::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("execute() begin");
  // Reading the input container
  ATH_MSG_VERBOSE("Retrieving input container");

  SG::ReadHandle<PRD_MultiTruthCollection> inputContainer(m_inputKey, ctx);
  if (!inputContainer.isValid()) {
    ATH_MSG_ERROR("Could not get pileup PRD_MultiTruthCollection " << inputContainer.name() << " from store " << inputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found pileup PRD_MultiTruthCollection " << inputContainer.name() << " in store " << inputContainer.store());

  // Creating output RDO container
  SG::WriteHandle<PRD_MultiTruthCollection> outputContainer(m_outputKey, ctx);
  ATH_CHECK(outputContainer.record(std::make_unique<PRD_MultiTruthCollection>()));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output PRD_MultiTruthCollection " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output PRD_MultiTruthCollection container " << outputContainer.name() << " in store " << outputContainer.store());

  for(const std::pair<const Identifier, HepMcParticleLink>& element : *inputContainer){
    outputContainer->insert(std::make_pair(element.first,element.second));
  }
  return StatusCode::SUCCESS;
}
