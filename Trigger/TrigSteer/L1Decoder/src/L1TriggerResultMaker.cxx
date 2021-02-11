/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TriggerResultMaker.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

namespace {
  template<class T> void makeLink(const SG::ReadHandleKey<T>& rhk,
                                  xAOD::TrigComposite& target,
                                  const std::string& linkName,
                                  const EventContext& eventContext) {
    ElementLink<T> link = ElementLink<T>(rhk.key(), 0, eventContext);
    target.typelessSetObjectLink(linkName,
                                 link.key(),
                                 ClassID_traits<T>::ID(),
                                 /*index =*/ 0);
  }
}

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
  ATH_CHECK(m_muRoIKey.initialize());
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of AthReentrantAlgorithm::execute
// =============================================================================
StatusCode L1TriggerResultMaker::execute(const EventContext& eventContext) const {
  ATH_MSG_DEBUG("Executing " << name());

  // Create and record the L1TriggerResult container
  SG::WriteHandle<xAOD::TrigCompositeContainer> l1trHandle(m_l1TriggerResultWHKey, eventContext);
  auto cont = std::make_unique<xAOD::TrigCompositeContainer>();
  auto auxcont = std::make_unique<xAOD::TrigCompositeAuxContainer>();
  cont->setStore(auxcont.get());
  ATH_CHECK(l1trHandle.record(std::move(cont), std::move(auxcont)));
  ATH_MSG_DEBUG("Recorded L1TriggerResult with key " << m_l1TriggerResultWHKey.key());

  // Create new L1TriggerResult in the container
  l1trHandle->push_back(new xAOD::TrigComposite);

  // Retrieve the L1 xAOD containers to verify they exist
  auto muRoIHandle = SG::makeHandle(m_muRoIKey, eventContext);
  ATH_CHECK(muRoIHandle.isValid());

  // Link the L1 xAOD containers (actually their first elements) to L1TriggerResult
  ATH_MSG_DEBUG(m_muRoIKey.key() << " size: " << muRoIHandle->size());
  if (not muRoIHandle->empty()) {
    makeLink(m_muRoIKey, *(l1trHandle->back()), m_muRoIKey.key(), eventContext);
  }

  return StatusCode::SUCCESS;
}
