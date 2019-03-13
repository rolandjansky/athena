/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Trigger includes
#include "PEBInfoWriterAlg.h"
#include "DecisionHandling/TrigCompositeUtils.h"

// Athena includes
#include "AthViews/View.h"

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
    auto roiELInfo = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>(previousDecision, "initialRoI");
    auto roiEL = roiELInfo.link;
    ATH_CHECK(roiEL.isValid());
    const TrigRoiDescriptor* roi = *roiEL;

    // Get View
    auto viewELInfo = TrigCompositeUtils::findLink<ViewContainer>(previousDecision, "view");
    ATH_CHECK(viewELInfo.isValid());
    auto viewEL = viewELInfo.link;

    // Create new decision
    Decision* newd = newDecisionIn(decisions);

    // Push_back to toolInput
    toolInputs.emplace_back(newd, eventContext, roi, previousDecision);

    // Link to new decision
    newd->setObjectLink("roi", roiEL);
    newd->setObjectLink("view", viewEL);
    linkToPrevious(newd, previousDecision);
    
    ATH_MSG_DEBUG("REGTEST:  View = " << (*viewEL));
    ATH_MSG_DEBUG("REGTEST:  RoI  = eta/phi = " << (*roiEL)->eta() << "/" << (*roiEL)->phi());
    ATH_MSG_DEBUG("Added view, roi, previous decision to new decision " << counter
                  << " for view " << (*viewEL)->name());
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
  if (msgLvl(MSG::DEBUG)) {
    DecisionIDContainer allPassingIDs;
    for (const Decision* decisionObject : *outputHandle) {
      decisionIDs(decisionObject, allPassingIDs);
    }
    for (DecisionID id : allPassingIDs) {
      ATH_MSG_DEBUG(" +++ " << HLT::Identifier(id));
    }
  }


  return StatusCode::SUCCESS;
}
