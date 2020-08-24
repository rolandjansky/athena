/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CopyMcEventCollection.h"


CopyMcEventCollection::CopyMcEventCollection(const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode CopyMcEventCollection::initialize()
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

StatusCode CopyMcEventCollection::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("execute() begin");

  // Reading the input containers
  ATH_MSG_VERBOSE("Retrieving input containers");

  const McEventCollection *bkgContainerPtr = nullptr;
  if (!m_bkgInputKey.key().empty()) {
    SG::ReadHandle<McEventCollection> bkgContainer(m_bkgInputKey, ctx);
    if (!bkgContainer.isValid()) {
      ATH_MSG_ERROR("Could not get background McEventCollection container " << bkgContainer.name() << " from store " << bkgContainer.store());
      return StatusCode::FAILURE;
    }
    bkgContainerPtr = bkgContainer.cptr();

    ATH_MSG_DEBUG("Found background McEventCollection container " << bkgContainer.name() << " in store " << bkgContainer.store());
  }

  SG::ReadHandle<McEventCollection> signalContainer(m_signalInputKey, ctx);
  if (!signalContainer.isValid()) {
    ATH_MSG_ERROR("Could not get signal McEventCollection container " << signalContainer.name() << " from store " << signalContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found signal McEventCollection container " << signalContainer.name() << " in store " << signalContainer.store());

  // Creating output RDO container
  SG::WriteHandle<McEventCollection> outputContainer(m_outputKey, ctx);
  ATH_CHECK(outputContainer.record(std::make_unique<McEventCollection>()));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output McEventCollection container " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output McEventCollection container " << outputContainer.name() << " in store " << outputContainer.store());

  // Copy signal GenEvents
  for (McEventCollection::const_iterator it = signalContainer->begin(); it != signalContainer->end(); ++it) {
    outputContainer->push_back(new HepMC::GenEvent(**it));
  }

  // Copy background GenEvents if configured
  if (!m_bkgInputKey.key().empty()) {
    McEventCollection::const_iterator it = bkgContainerPtr->begin();
    if (m_removeBkgHardScatterTruth.value()) {
      ++it;
    }
    for ( ; it != bkgContainerPtr->end(); ++it) {
      outputContainer->push_back(new HepMC::GenEvent(**it));
    }
  }

  // dump McEventCollection in debug mode to confirm everything is as expected
  if (msgLvl(MSG::DEBUG)) {
    if (!outputContainer->empty()) {
      ATH_MSG_DEBUG("McEventCollection contents:");
      for (const HepMC::GenEvent *event : *outputContainer) {
        ATH_MSG_DEBUG("  GenEvent #" << event->event_number() << ", signal_process_id=" << event->signal_process_id());
      }
    }
  }

  ATH_MSG_DEBUG("execute() end");
  return StatusCode::SUCCESS;
}
