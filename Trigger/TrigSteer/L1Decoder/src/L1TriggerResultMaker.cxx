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
  ATH_CHECK(m_muRoIKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_eFexEMRoIKey.initialize(SG::AllowEmpty));
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

  auto retrieveAndLink = [this, &eventContext, &l1trHandle](auto key) -> StatusCode {
    // Skip disabled inputs
    if (key.empty()) {return StatusCode::SUCCESS;}
    // Retrieve the L1 xAOD container to verify it exists
    auto handle = SG::makeHandle(key, eventContext);
    ATH_CHECK(handle.isValid());
    // Link the L1 xAOD container (actually its first element) to L1TriggerResult
    ATH_MSG_DEBUG(key.key() << " size: " << handle->size());
    if (not handle->empty()) {
      makeLink(key, *(l1trHandle->back()), key.key(), eventContext);
    }
    return StatusCode::SUCCESS;
  };

  ATH_CHECK(retrieveAndLink(m_muRoIKey));
  ATH_CHECK(retrieveAndLink(m_eFexEMRoIKey));

  return StatusCode::SUCCESS;
}
