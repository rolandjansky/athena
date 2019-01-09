/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MTCalibPebHypoAlg.h"
#include "DecisionHandling/HLTIdentifier.h"

// TrigCompositeUtils types used here
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::DecisionIDContainer;

// TrigCompositeUtils methods used here
using TrigCompositeUtils::createAndStore;
using TrigCompositeUtils::decisionIDs;
using TrigCompositeUtils::newDecisionIn;

// =============================================================================
// Standard constructor
// =============================================================================
MTCalibPebHypoAlg::MTCalibPebHypoAlg(const std::string& name, ISvcLocator* svcLoc)
: HypoBase(name, svcLoc) {}

// =============================================================================
// Standard destructor
// =============================================================================
MTCalibPebHypoAlg::~MTCalibPebHypoAlg() {}

// =============================================================================
// Implementation of AthReentrantAlgorithm::initialize
// =============================================================================
StatusCode MTCalibPebHypoAlg::initialize() {
  ATH_MSG_INFO("Initialising " << name());
  ATH_CHECK(m_hypoTools.retrieve());
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of AthReentrantAlgorithm::finalize
// =============================================================================
StatusCode MTCalibPebHypoAlg::finalize() {
  ATH_MSG_INFO("Finalising " << name());
  ATH_CHECK(m_hypoTools.release());
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of AthReentrantAlgorithm::execute
// =============================================================================
StatusCode MTCalibPebHypoAlg::execute(const EventContext& eventContext) const {
  ATH_MSG_DEBUG("Executing " << name());

  // New output decision container
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), eventContext);
  DecisionContainer* decisions = outputHandle.ptr();

  // Create new decision (DecisionContainer* decisions owns the new object)
  Decision* newd = newDecisionIn(decisions, "", eventContext);

  // Prepare input for hypo tools
  MTCalibPebHypoTool::Input toolInput(newd, eventContext);

  // Call the hypo tools
  for (const auto& tool: m_hypoTools) {
    ATH_MSG_DEBUG("Calling " << tool);
    ATH_CHECK(tool->decide(toolInput));
  }

  ATH_MSG_DEBUG( "Exiting with "<< outputHandle->size() <<" decisions");

  for (auto outh: *outputHandle) {
    DecisionIDContainer objDecisions;
    decisionIDs(outh, objDecisions);
    ATH_MSG_DEBUG("Number of positive decisions for this input: " << objDecisions.size());
    for ( DecisionID id : objDecisions ) {
      ATH_MSG_DEBUG(" --- found new decision " << HLT::Identifier(id));
    }
  }

  return StatusCode::SUCCESS;
}
