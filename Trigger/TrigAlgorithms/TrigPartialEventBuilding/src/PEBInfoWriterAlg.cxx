/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Trigger includes
#include "PEBInfoWriterAlg.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

// TrigCompositeUtils types used here
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::LinkInfo;

// TrigCompositeUtils methods used here
using TrigCompositeUtils::createAndStore;
using TrigCompositeUtils::decisionIDs;
using TrigCompositeUtils::linkToPrevious;
using TrigCompositeUtils::newDecisionIn;
using TrigCompositeUtils::featureString;
using TrigCompositeUtils::hypoAlgNodeName;
using TrigCompositeUtils::roiString;
using TrigCompositeUtils::findLink;

// =============================================================================

PEBInfoWriterAlg::PEBInfoWriterAlg(const std::string& name, ISvcLocator* pSvcLocator)
: ::HypoBase(name, pSvcLocator) {}

// =============================================================================

PEBInfoWriterAlg::~PEBInfoWriterAlg() {}

// =============================================================================

StatusCode PEBInfoWriterAlg::initialize() {
  ATH_MSG_DEBUG("Initialising " << name());
  ATH_CHECK(m_hypoTools.retrieve());
  return StatusCode::SUCCESS;
}

// =============================================================================

StatusCode PEBInfoWriterAlg::finalize() {
  ATH_MSG_DEBUG("Finalising " << name());
  return StatusCode::SUCCESS;
}

// =============================================================================

StatusCode PEBInfoWriterAlg::execute(const EventContext& eventContext) const {
  ATH_MSG_DEBUG("Executing " << name());

  // ---------------------------------------------------------------------------
  // Common hypo start - retrieve previous decisions and record new container
  // ---------------------------------------------------------------------------
  // Retrieve previous decisions
  SG::ReadHandle<DecisionContainer> previousDecisionsHandle = SG::makeHandle(decisionInput(), eventContext);
  if (not previousDecisionsHandle.isValid()) { // Implicit
    ATH_MSG_DEBUG("No implicit previous decisions with key " << decisionInput().key() << " - is this expected?");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("Running with " << previousDecisionsHandle->size() << " implicit previous decisions");

  // New output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), eventContext);
  DecisionContainer* decisions = outputHandle.ptr();

  // ---------------------------------------------------------------------------
  // Prepare new decision and hypo tool inputs
  // ---------------------------------------------------------------------------
  std::vector<PEBInfoWriterToolBase::Input> toolInputs;
  size_t counter = 0;
  for (const Decision* previousDecision: *previousDecisionsHandle) {
    // Get RoI
    auto roiELInfo = findLink<TrigRoiDescriptorCollection>(previousDecision, roiString());
    auto roiEL = roiELInfo.link;
    ATH_CHECK(roiEL.isValid());

    // Create new HypoAlg decision and link it to previousDecision
    Decision* newd = newDecisionIn(decisions, previousDecision, hypoAlgNodeName(), eventContext);

    // Attach empty PEB Info lists to the new decision
    ATH_CHECK(newd->setDetail(PEBInfoWriterToolBase::robListKey(), std::vector<uint32_t>()));
    ATH_CHECK(newd->setDetail(PEBInfoWriterToolBase::subDetListKey(), std::vector<uint32_t>()));

    // Push_back to toolInput
    toolInputs.emplace_back(newd, eventContext, roiEL, previousDecision);

    // Link to feature. Dummy link here
    ElementLink<DecisionContainer> dummyLink(*decisions, decisions->size()-1, eventContext);
    newd->setObjectLink(featureString(), dummyLink);
    newd->setObjectLink(roiString(), roiEL);

    ATH_MSG_DEBUG("RoI eta/phi = " << (*roiEL)->eta() << "/" << (*roiEL)->phi());
    ATH_MSG_DEBUG("Added RoI, previous decision and dummy feature to new decision " << counter);
    ++counter;
  }

  // ---------------------------------------------------------------------------
  // Call the hypo tools
  // ---------------------------------------------------------------------------
  for (const auto& tool: m_hypoTools) {
    ATH_MSG_DEBUG("Calling " << tool);
    ATH_CHECK(tool->decide(toolInputs));
  }

  // ---------------------------------------------------------------------------
  // Print the passing decisions
  // ---------------------------------------------------------------------------
  ATH_CHECK(outputHandle.isValid());
  ATH_MSG_DEBUG("Exit with " << outputHandle->size() << " decisions");
  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

  return StatusCode::SUCCESS;
}
