/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "eFexEMRoIsUnpackingTool.h"
#include "xAODTrigger/eFexEMRoIContainer.h"

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
  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode eFexEMRoIsUnpackingTool::start() {
  ATH_CHECK(decodeMapping([](const std::string& name){return name.find("eEM")==0;}));
  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode eFexEMRoIsUnpackingTool::unpack(const EventContext& ctx,
                                           const xAOD::TrigComposite& l1TriggerResult,
                                           const HLT::IDSet& activeChains) const {
  // Retrieve the L1 menu configuration
  SG::ReadHandle<TrigConf::L1Menu> l1Menu = SG::makeHandle(m_l1MenuKey, ctx);
  ATH_CHECK(l1Menu.isValid());

  // Retrieve the eEM thresholds vector from L1 menu configuration
  using ThrVec = const std::vector<std::shared_ptr<TrigConf::L1Threshold>>;
  using ThrVecRef = std::reference_wrapper<ThrVec>;
  std::optional<ThrVecRef> thresholds;
  try {
    thresholds = ThrVecRef(l1Menu->thresholds("eEM"));
  }
  catch (const std::exception& ex) {
    ATH_MSG_ERROR("Failed to retrieve eEM thresholds from L1 menu. Exception:" << ex.what());
    return StatusCode::FAILURE;
  }

  // Retrieve the xAOD RoI container from L1TriggerResult
  if (!l1TriggerResult.hasObjectLink(m_eFexEMRoILinkName, ClassID_traits<xAOD::eFexEMRoIContainer>::ID())) {
    ATH_MSG_DEBUG("No eFex EM RoIs in this event");
    return StatusCode::SUCCESS;
  }
  ElementLink<xAOD::eFexEMRoIContainer> roisLink = l1TriggerResult.objectLink<xAOD::eFexEMRoIContainer>(m_eFexEMRoILinkName);
  ATH_CHECK(roisLink.isValid());
  const xAOD::eFexEMRoIContainer& rois = roisLink.getStorableObjectRef();

  // Create and record RoI descriptor and decision containers
  using namespace TrigCompositeUtils;
  SG::WriteHandle<TrigRoiDescriptorCollection> roiDescriptors = createAndStoreNoAux(m_trigRoIsKey, ctx);
  SG::WriteHandle<DecisionContainer> decisions = createAndStore(m_decisionsKey, ctx);

  size_t linkIndex{0};
  for (const xAOD::eFexEMRoI* roi : rois) {
    // Create new RoI descriptor
    roiDescriptors->push_back(std::make_unique<TrigRoiDescriptor>(
      roi->eta(), roi->eta()-m_roiWidth, roi->eta()+m_roiWidth,
      roi->phi(), roi->phi()-m_roiWidth, roi->phi()+m_roiWidth
    ));

    // Create new decision and link the RoI objects
    Decision* decision = TrigCompositeUtils::newDecisionIn(decisions.ptr(), l1DecoderNodeName());
    decision->setObjectLink(initialRoIString(),
                            ElementLink<TrigRoiDescriptorCollection>(m_trigRoIsKey.key(), linkIndex, ctx));
    decision->setObjectLink(initialRecRoIString(),
                            ElementLink<xAOD::eFexEMRoIContainer>(m_eFexEMRoILinkName, linkIndex, ctx));
    ++linkIndex;

    // Add positive decisions for chains to be activated by this RoI object
    for (const std::shared_ptr<TrigConf::L1Threshold>& thr : thresholds.value().get()) {
      if (not (roi->thrPattern() & (1 << thr->mapping()))) {continue;}
      ATH_MSG_DEBUG("Threshold passed: " << thr->name());
      addChainsToDecision(HLT::Identifier(thr->name()), decision, activeChains);
      if (msgLvl(MSG::DEBUG)) {
        DecisionIDContainer ids;
        decisionIDs(decision, ids);
        ATH_MSG_DEBUG("Activated chains: " << std::vector<DecisionID>(ids.begin(), ids.end()));
      }
    }
  }

  return StatusCode::SUCCESS;
}
