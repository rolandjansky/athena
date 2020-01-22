/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TriggerResultMaker.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

// =============================================================================
// Standard constructor
// =============================================================================
L1TriggerResultMaker::L1TriggerResultMaker(const std::string& name, ISvcLocator* svcLoc)
: AthReentrantAlgorithm(name, svcLoc) {}

// =============================================================================
// Implementation of AthReentrantAlgorithm::initialize
// =============================================================================
StatusCode L1TriggerResultMaker::initialize() {
  ATH_MSG_DEBUG("Initialising " << name());
  ATH_CHECK(m_l1TriggerResultWHKey.initialize());
  ATH_CHECK(m_decoderTools.retrieve());
  ATH_CHECK(m_robDataProviderSvc.retrieve());
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of AthReentrantAlgorithm::finalize
// =============================================================================
StatusCode L1TriggerResultMaker::finalize() {
  ATH_MSG_DEBUG("Finalising " << name());
  ATH_CHECK(m_robDataProviderSvc.release());
  ATH_CHECK(m_decoderTools.release());
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of AthReentrantAlgorithm::execute
// =============================================================================
StatusCode L1TriggerResultMaker::execute(const EventContext& eventContext) const {
  ATH_MSG_DEBUG("Executing " << name());

  // Create and record the L1TriggerResult container
  SG::WriteHandle<xAOD::TrigCompositeContainer> handle(m_l1TriggerResultWHKey, eventContext);
  auto cont = std::make_unique<xAOD::TrigCompositeContainer>();
  auto auxcont = std::make_unique<xAOD::TrigCompositeAuxContainer>();
  cont->setStore(auxcont.get());
  ATH_CHECK(handle.record(std::move(cont), std::move(auxcont)));
  ATH_MSG_DEBUG("Recorded L1TriggerResult with key " << m_l1TriggerResultWHKey.key());

  // Retrieve the BS data and fill a new L1TriggerResult
  IROBDataProviderSvc::VROBFRAG vrobf;
  handle->push_back(new xAOD::TrigComposite);
  for (const auto& decoderTool : m_decoderTools) {
    vrobf.clear();
    m_robDataProviderSvc->getROBData(eventContext, decoderTool->robIds(), vrobf, name());
    ATH_CHECK(decoderTool->convert(vrobf, *(handle->back()), eventContext));
    // Verify if the link was created
    if (!handle->back()->hasObjectLink(decoderTool->linkName())) {
      ATH_MSG_ERROR("Decoder tool " << decoderTool.name() << " failed to link the xAOD RoI object to L1TriggerResult"
                    << " with the link name " << decoderTool->linkName());
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}
