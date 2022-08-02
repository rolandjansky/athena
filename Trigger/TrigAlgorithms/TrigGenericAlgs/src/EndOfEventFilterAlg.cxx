/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "EndOfEventFilterAlg.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTrigger/TrigComposite.h"
#include "StoreGate/ReadHandle.h"

#include <sstream>

namespace {
  std::string printVector(const std::vector<std::string>& vec) {
    std::ostringstream ss;
    ss << "[";
    bool first{true};
    for (const std::string& elem : vec) {
      if (first) {first=false;}
      else {ss << ", ";}
      ss << elem;
    }
    ss << "]";
    return ss.str();
  }
}

EndOfEventFilterAlg::EndOfEventFilterAlg(const std::string& name, ISvcLocator* pSvcLocator)
: AthReentrantAlgorithm(name,pSvcLocator) {}

StatusCode EndOfEventFilterAlg::initialize() {
  ATH_CHECK(m_hltSeedingSummaryKey.initialize());
  ATH_CHECK(m_streamsSummaryKey.initialize(!m_streamFilter.value().empty()));
  if (m_chainName.value().empty()) {
    ATH_MSG_ERROR("The " << m_chainName.name() << " property was not set");
    return StatusCode::FAILURE;
  }
  m_chainId = HLT::Identifier{m_chainName.value()};

  if (msgLvl(MSG::DEBUG)) {
    std::ostringstream ss{"This filter will pass when chain "};
    ss << m_chainId.numeric() << " / " << m_chainId.name() << " is active";
    if (m_streamFilter.value().empty()) {
      ss << ". No stream filtering will be applied.";
    } else {
      ss << " and the event is accepted to at least one of the following streams: ";
      ss << printVector(m_streamFilter.value());
    }
    ATH_MSG_DEBUG(ss.str());
  }

  return StatusCode::SUCCESS;
}

StatusCode EndOfEventFilterAlg::execute(const EventContext& context) const {
  using TrigCompositeUtils::Decision;
  using TrigCompositeUtils::DecisionContainer;

  if (!m_streamFilter.value().empty()) {
    SG::ReadHandle<std::vector<std::string>> streamsSummary{m_streamsSummaryKey, context};
    ATH_CHECK(streamsSummary.isValid());
    const auto it = std::find_first_of(streamsSummary->cbegin(), streamsSummary->cend(),
                                       m_streamFilter.value().cbegin(), m_streamFilter.value().cend());
    if (it == streamsSummary->cend()) {
      setFilterPassed(false, context);
      ATH_MSG_DEBUG("Setting filter status to failed because none of the listed streams passed: "
                    << printVector(m_streamFilter.value()));
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("At least one of the listed streams passed: " << printVector(m_streamFilter.value())
                  << ". The first stream found to pass is " << *it
                  << ". Continue to checking the chain prescale status.");
  }

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
