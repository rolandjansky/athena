/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "eFexEMRoIsUnpackingTool.h"
#include "StoreGate/ReadDecorHandle.h"

// =============================================================================
eFexEMRoIsUnpackingTool::eFexEMRoIsUnpackingTool(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent)
  : RoIsUnpackingToolBase(type, name, parent) {}

// =============================================================================
StatusCode eFexEMRoIsUnpackingTool::initialize() {
  ATH_CHECK(RoIsUnpackingToolBase::initialize());
  ATH_CHECK(m_l1MenuKey.initialize());
  ATH_CHECK(m_trigRoIsKey.initialize());
  ATH_CHECK(m_thresholdPatternsKey.initialize());
  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode eFexEMRoIsUnpackingTool::start() {
  ATH_CHECK(decodeMapping([](const std::string& name){return name.find("eEM")==0 or name.find(getProbeThresholdName("eEM"))==0;}));
  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode eFexEMRoIsUnpackingTool::unpack(const EventContext& ctx,
                                           const xAOD::TrigComposite& l1TriggerResult,
                                           const HLT::IDSet& activeChains) const {
  using namespace TrigCompositeUtils;

  // Create and record RoI descriptor and decision containers
  SG::WriteHandle<TrigRoiDescriptorCollection> roiDescriptors = createAndStoreNoAux(m_trigRoIsKey, ctx);
  SG::WriteHandle<DecisionContainer> decisionsMain = createAndStore(m_decisionsKey, ctx);
  SG::WriteHandle<DecisionContainer> decisionsProbe = createAndStore(m_decisionsKeyProbe, ctx);

  // Retrieve the xAOD RoI container from L1TriggerResult
  if (!l1TriggerResult.hasObjectLink(m_eFexEMRoILinkName, ClassID_traits<xAOD::eFexEMRoIContainer>::ID())) {
    ATH_MSG_DEBUG("No eFex EM RoIs in this event");
    return StatusCode::SUCCESS;
  }
  ElementLink<xAOD::eFexEMRoIContainer> roisLink = l1TriggerResult.objectLink<xAOD::eFexEMRoIContainer>(m_eFexEMRoILinkName);
  ATH_CHECK(roisLink.isValid());
  const xAOD::eFexEMRoIContainer& rois = roisLink.getStorableObjectRef();

  // Create threshold patterns decoration accessor
  auto thrPatternAcc = SG::makeHandle<uint64_t>(m_thresholdPatternsKey, ctx);
  ATH_CHECK(thrPatternAcc.isPresent());

  // Retrieve the L1 menu configuration
  SG::ReadHandle<TrigConf::L1Menu> l1Menu = SG::makeHandle(m_l1MenuKey, ctx);
  ATH_CHECK(l1Menu.isValid());
  std::optional<ThrVecRef> thresholds;
  ATH_CHECK(getL1Thresholds(*l1Menu, "eEM", thresholds));

  size_t linkIndex{0};
  for (const xAOD::eFexEMRoI* roi : rois) {
    // Create new RoI descriptor
    roiDescriptors->push_back(std::make_unique<TrigRoiDescriptor>(
      roi->eta(), roi->eta()-m_roiWidth, roi->eta()+m_roiWidth,
      roi->phi(), roi->phi()-m_roiWidth, roi->phi()+m_roiWidth
    ));

    // Create new decision and link the RoI objects
    Decision* decisionMain = TrigCompositeUtils::newDecisionIn(decisionsMain.ptr(), hltSeedingNodeName());
    decisionMain->setObjectLink(initialRoIString(),
                            ElementLink<TrigRoiDescriptorCollection>(m_trigRoIsKey.key(), linkIndex, ctx));
    decisionMain->setObjectLink(initialRecRoIString(),
                            ElementLink<xAOD::eFexEMRoIContainer>(m_eFexEMRoILinkName, linkIndex, ctx));

    Decision* decisionProbe = TrigCompositeUtils::newDecisionIn(decisionsProbe.ptr(), hltSeedingNodeName());
    decisionProbe->setObjectLink(initialRoIString(),
                            ElementLink<TrigRoiDescriptorCollection>(m_trigRoIsKey.key(), linkIndex, ctx));
    decisionProbe->setObjectLink(initialRecRoIString(),
                            ElementLink<xAOD::eFexEMRoIContainer>(m_eFexEMRoILinkName, linkIndex, ctx));

    // Add positive decisions for chains to be activated by this RoI object
    uint64_t thresholdPattern = thrPatternAcc(*roi);
    ATH_MSG_DEBUG("RoI #" << linkIndex << " threshold pattern: " << thresholdPattern);
    for (const std::shared_ptr<TrigConf::L1Threshold>& thr : thresholds.value().get()) {
      if (not (thresholdPattern & (1 << thr->mapping()))) {continue;}
      const std::string thresholdProbeName = getProbeThresholdName(thr->name());
      ATH_MSG_DEBUG("RoI #" << linkIndex << " passed threshold number " << thr->mapping()
                    << " names " << thr->name() << " and " << thresholdProbeName);
      addChainsToDecision(HLT::Identifier(thr->name()), decisionMain, activeChains);
      addChainsToDecision(HLT::Identifier(thresholdProbeName), decisionProbe, activeChains);
    }

    ++linkIndex;
  }

  return StatusCode::SUCCESS;
}
