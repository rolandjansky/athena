/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "EndOfEventPrescaleCheckAlg.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTrigger/TrigComposite.h"
#include "StoreGate/ReadHandle.h"

EndOfEventPrescaleCheckAlg::EndOfEventPrescaleCheckAlg(const std::string& name, ISvcLocator* pSvcLocator)
: AthReentrantAlgorithm(name,pSvcLocator) {}

StatusCode EndOfEventPrescaleCheckAlg::initialize() {
  ATH_CHECK(m_hltSeedingSummaryKey.initialize());
  if (m_chainName.value().empty()) {
    ATH_MSG_ERROR("The " << m_chainName.name() << " property was not set");
    return StatusCode::FAILURE;
  }
  m_chainId = HLT::Identifier{m_chainName.value()};
  ATH_MSG_DEBUG("This filter will pass when chain " << m_chainId.numeric() << " / " << m_chainId.name() << " is active");
  return StatusCode::SUCCESS;
}

StatusCode EndOfEventPrescaleCheckAlg::execute(const EventContext& context) const {
  using TrigCompositeUtils::Decision;
  using TrigCompositeUtils::DecisionContainer;

  SG::ReadHandle<DecisionContainer> hltSeedingSummary{m_hltSeedingSummaryKey,context};
  ATH_CHECK(hltSeedingSummary.isValid());
  const auto it = std::find_if(hltSeedingSummary->begin(), hltSeedingSummary->end(),
                               [](const Decision* d){return d->name()=="unprescaled";});
  if (it==hltSeedingSummary->end()) {
    ATH_MSG_ERROR("Failed to find \"unprescaled\" Decision in " << m_hltSeedingSummaryKey.key());
    return StatusCode::FAILURE;
  }
  const Decision* activeChains = *it;
  TrigCompositeUtils::DecisionIDContainer activeChainIDs;
  TrigCompositeUtils::decisionIDs(activeChains, activeChainIDs);
  const bool filterStatus = TrigCompositeUtils::passed(m_chainId, activeChainIDs);
  ATH_MSG_DEBUG("Setting filter status to " << (filterStatus ? "passed" : "failed") << " because the chain "
                << m_chainId.name() << " is " << (filterStatus ? "unprescaled" : "prescaled") << " in this event");
  setFilterPassed(filterStatus, context);
  return StatusCode::SUCCESS;
}
