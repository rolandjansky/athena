/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "PassthroughComboHypo.h"

#include "xAODTrigger/TrigComposite.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

using namespace TrigCompositeUtils;


PassthroughComboHypo::PassthroughComboHypo(const std::string& name, ISvcLocator* pSvcLocator)
    : ::ComboHypo(name, pSvcLocator) {
}


StatusCode PassthroughComboHypo::initialize() {
  ATH_MSG_DEBUG( "PassthroughComboHypo::initialize()" );
  return ::ComboHypo::initialize();
}


StatusCode PassthroughComboHypo::execute(const EventContext& context) const {
  ATH_MSG_DEBUG( "PassthroughComboHypo::execute()" );
  for (size_t input_counter = 0; input_counter < decisionsInput().size(); ++input_counter ) {
    // New output collection to mirror input collection
    SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionsOutput().at(input_counter), context); 
    SG::ReadHandle<DecisionContainer> inputHandle = SG::makeHandle(decisionsInput().at(input_counter), context);
    if (inputHandle.isValid()) {
      ATH_MSG_DEBUG("Copying to output " << inputHandle->size() << " Decision objects from handle input handle #" 
        << input_counter << ", " << decisionsInput().at(input_counter).key());
      for (const Decision* inputDecision : *inputHandle) {
        // Get all passing IDs from inputDecision
        DecisionIDContainer inputDecisionIDs;
        decisionIDs(inputDecision, inputDecisionIDs);

        // Create 1-to-1 output Decision object, linked to inputDecision
        Decision* outputDecision = newDecisionIn(outputHandle.ptr(), inputDecision, comboHypoAlgNodeName(), context);
        // Copy in all the same passing IDs
        insertDecisionIDs(inputDecisionIDs, outputDecision);
      }
    } else {
      ATH_MSG_ERROR("Unable to resolve required Input Handle: " << decisionsInput().at(input_counter).key());
      ATH_MSG_ERROR("Check menu configuration. If this ComboHypo is executed then all of the producers of its inputs should have run.");
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}